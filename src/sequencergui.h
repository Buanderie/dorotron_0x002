#pragma once

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
    SequencerGUI( StepSequencer* seq = nullptr, int width = 1024, int height = 768 )
        :_seq(seq), _width(width), _height(height)
    {
        init();
        _targetWidth = 800;
        _targetHeight = 800;
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
//        ToggleFullscreen();

        shaders[0] = LoadShader(0, TextFormat("cross_hatching.fs", GLSL_VERSION));

        while (!WindowShouldClose())
        {

            if (IsKeyDown(KEY_UP)) _active_idx = min( _active_idx + 1, (ssize_t)_seq->tracks().size() - 1 );
            if (IsKeyDown(KEY_DOWN)) _active_idx = max( _active_idx - 1, (ssize_t)0 );

            BeginDrawing();

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
                    renderTrack(t, 400, 1.0);
                }
                else
                {
                    renderTrack(t, 400);
                }
                r += 50;
                //                EndShaderMode();
                EndTextureMode();
                i = i+1;
            }

            BeginMode3D(_camera);
            ClearBackground( CLITERAL(Color){ 0, 0, 0, 0 } );  // Clear texture background
            //            DrawGrid(10, 1.0f); // Draw a grid
            double y_step = 1.0;
            double y = -6.7 + (_tritems.size() - (_active_idx)) * y_step;
            for( auto t : _seq->tracks() )
            {
                Vector3 position = { 0.0f, y, 0.0f };
                DrawModel(_tritems[t].model, position, 1.0f, WHITE);
                y += y_step;
            }
            //            DrawBillboard( _camera, _tritems[ _seq->tracks()[0] ].target.texture, (Vector3){0,0,0}, 10.0, WHITE );
            EndMode3D();

                        DrawFPS(0,0);

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
            //            rlRotatef(30.0, 15.0f, 0.0f, 0.0f);
            Color c = LIGHTGRAY;
            Color c2 = DARKBLUE;

            if( i == t->active_step_idx() )
                c = RED;

            DrawRing( (Vector2){_targetWidth/2, _targetHeight/2}, radius - circle_width, radius, cur_angle, cur_angle + s_arc_len, 250, Fade(c, transp_value));
            for( double xx = -0.01; xx < 0.01; xx += 0.05 )
                for( double yy = -1.1; yy < 1.1; yy += 0.05 )
                    DrawRingLines( (Vector2){_targetWidth/2 + xx, _targetHeight/2 + yy }, radius - circle_width, radius, cur_angle, cur_angle + s_arc_len, 0, Fade(c2, transp_value) );

            cur_angle += s_arc_len + (double)space_width;
        }

        // DrawRing( (Vector2){1280/2, 720/2}, 100, 120, 0, 270, 0, Fade(BLACK, 0.5));
    }

protected:

};
