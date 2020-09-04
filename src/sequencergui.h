#pragma once

#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

#include "stepsequencer.h"

#include "raylib.h"
#include "rlgl.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

class SequencerGUI
{
    typedef struct
    {
        RenderTexture2D target;
        Model model;
    } TrackRenderingItem;

public:
    SequencerGUI( StepSequencer* seq = nullptr, int width = 1280, int height = 720 )
        :_seq(seq), _width(width), _height(height)
    {
        init();
        _targetWidth = 1024;
        _targetHeight = 1024;
        _active_idx = 0;
        _camera = { { 0.0f, 9.0f, 11.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 40.0f, 0 };
    }

    virtual ~SequencerGUI()
    {

    }

    void run()
    {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(_width, _height, "dorotron_0x002" );
        SetTargetFPS(60);
        _font = LoadFont("../resources/MODENINE.TTF");


        _blurredTarget = LoadRenderTexture(_width, _height);
        _clearTarget = LoadRenderTexture(_width, _height);

        shaders[0] = LoadShader(0, TextFormat("../resources/pute.fs", GLSL_VERSION));
        shaders[1] = LoadShader(0, TextFormat("../resources/blur.fs", GLSL_VERSION));

        while (!WindowShouldClose())
        {

            if (IsKeyPressed(KEY_UP)) _active_idx = min( _active_idx + 1, (ssize_t)_seq->tracks().size() - 1 );
            if (IsKeyPressed(KEY_DOWN)) _active_idx = max( _active_idx - 1, (ssize_t)0 );

            if( IsKeyPressed(KEY_SPACE) )
            {
                _seq->tracks()[ _active_idx ]->trigger();
            }

            BeginDrawing();
            ClearBackground( CLITERAL(Color){ 0, 0, 0, 0 } );  // Clear texture background

            checkTracksBeforeRendering();
            int r = 50;
            int i = 0;
            for( auto t : _seq->tracks() )
            {
                BeginTextureMode(_tritems[t].target);
                ClearBackground( CLITERAL(Color){ 245, 245, 245, 0 } );  // Clear texture background
                //                BeginShaderMode(shaders[0]);

                if( i == _active_idx )
                {
                    renderTrack(t, 380, 1.0);
                }
                else
                {
                    renderTrack(t, 380, 0.3);
                }
                r += 50;
                //                EndShaderMode();
                EndTextureMode();
                i = i+1;
            }

            // Blurred shit
            BeginTextureMode( _blurredTarget );
            ClearBackground( CLITERAL(Color){ 10, 10, 10, 255 } );  // Clear texture background
            BeginMode3D(_camera);
            double y_step = 1.5;
            double y = 1.0;
            for( int i = 0; i < _active_idx; ++i )
            {
                y -= y_step;
            }
            int j = 0;
            for( auto t : _seq->tracks() )
            {
                if( j != _active_idx )
                {
                    Vector3 position = { 0.0f, y, 0.0f };
                    _tritems[t].model.materials[0].shader = shaders[0];
                    DrawModel(_tritems[t].model, position, 1.0f, WHITE);
                }
                y += y_step;
                j++;
            }
            EndMode3D();
            EndTextureMode();

            // Not blurred shit
            BeginTextureMode( _clearTarget );
            ClearBackground( CLITERAL(Color){ 0, 0, 0, 0 } );  // Clear texture background
            BeginMode3D(_camera);
            y = 1.0;
            Vector3 position = { 0.0f, y, 0.0f };
            _tritems[ _seq->tracks()[_active_idx] ].model.materials[0].shader = shaders[0];
            DrawModel(_tritems[ _seq->tracks()[_active_idx] ].model, position, 1.0f, WHITE);
            EndMode3D();
            EndTextureMode();
            //

            // Draw blurred shit
            BeginShaderMode( shaders[1] );
            DrawTextureRec(_blurredTarget.texture, (Rectangle){ 0, 0, _blurredTarget.texture.width, -_blurredTarget.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();
            DrawTextureRec( _clearTarget.texture, (Rectangle){ 0, 0, _clearTarget.texture.width, -_clearTarget.texture.height }, (Vector2){ 0, 0 }, WHITE);

            DrawRectangle( 0, GetScreenHeight() - 40, GetScreenWidth(), 40, DARKGRAY );
            stringstream sstr;
            sstr << "CHHANEL_0x" << std::setfill('0') << std::setw(2) << hex << _active_idx;
            DrawTextEx(_font, sstr.str().c_str(), (Vector2){ GetScreenWidth() / 2 - 120, _height - 33.0f }, _font.baseSize * 1.0, 2, RAYWHITE);

            EndDrawing();

        }
        CloseWindow();
    }

private:
    StepSequencer * _seq;
    int _width;
    int _height;

    Camera _camera;
    int _targetWidth;
    int _targetHeight;

    ssize_t _active_idx;

    Shader shaders[16] = { 0 };
    Font _font;

    std::map< Track*, TrackRenderingItem > _tritems;
    void checkTracksBeforeRendering()
    {
        for( auto t : _seq->tracks() )
        {
            if( _tritems.find(t) == _tritems.end() )
            {
                TrackRenderingItem tritem;
                tritem.target = LoadRenderTexture(_targetWidth, _targetHeight);
                tritem.model = LoadModelFromMesh(GenMeshPlane( 10.0, 10.0, 10, 10 ));
                tritem.model.materials[0].maps[MAP_DIFFUSE].texture = tritem.target.texture;
                _tritems.insert( std::make_pair( t, tritem ) );
            }
        }
    }

    RenderTexture2D _blurredTarget;
    RenderTexture2D _clearTarget;

    void init()
    {

    }

    void renderTrack( Track* t, double radius, double transp_value = 0.25 )
    {

        double circle_width = 50.0;

        int nspaces = t->n_steps();
        int space_width = 5;
        int space_arc = space_width * nspaces;
        int rem_arc = 360 - space_arc;
        double s_arc_len = (double)rem_arc / (double)(t->n_steps());
        double cur_angle = 0.0;

        for( double qq = radius - circle_width / 2.0 - 1.0; qq < radius - circle_width / 2.0 + 1.0; qq+= 0.1 )
            DrawCircleLines( _targetWidth/2, _targetHeight/2, qq, Fade(DARKBLUE, transp_value) );

        for( int i = 0; i < t->n_steps(); ++i )
        {
            Color c = Fade(LIGHTGRAY, transp_value);
            Color c2 = DARKBLUE;

            if( t->step_state(i) == Track::STEP_IDLE )
                c = Fade( BLACK, transp_value * 0.3 );
            //                c = Fade( RAYWHITE, transp_value );

            else if( t->step_state(i) == Track::STEP_ACTIVE )
                c = Fade( RAYWHITE, transp_value );

            if( i == t->active_step_idx() )
                c = Fade(RED, transp_value * 2.0);

            DrawRing( (Vector2){_targetWidth/2, _targetHeight/2}, radius - circle_width, radius, cur_angle, cur_angle + s_arc_len, 250, c);

            cur_angle += s_arc_len + (double)space_width;
        }
    }

protected:

};
