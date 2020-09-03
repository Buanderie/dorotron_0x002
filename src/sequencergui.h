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
    } TrackRenderingItem;

public:
    SequencerGUI( StepSequencer* seq = nullptr, int width = 1280, int height = 720 )
        :_seq(seq), _width(width), _height(height)
    {
        init();
        _targetWidth = 400;
        _targetHeight = 400;

    }

    virtual ~SequencerGUI()
    {

    }

    void run()
    {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(_width, _height, "dorotron_0x002" );
        SetTargetFPS(60);
        shaders[0] = LoadShader(0, TextFormat("cross_hatching.fs", GLSL_VERSION));

        while (!WindowShouldClose())
        {
            checkTracksBeforeRendering();
            for( auto t : _seq->tracks() )
            {
                BeginTextureMode(_tritems[t].target);
                ClearBackground( CLITERAL(Color){ 245, 245, 245, 0 } );  // Clear texture background
//                BeginShaderMode(shaders[0]);
                renderTrack(t, _targetWidth/2);
//                EndShaderMode();
                EndTextureMode();
            }

            BeginDrawing();
            ClearBackground(BLACK);
            // BeginShaderMode(shaders[0]);
            for( auto t : _seq->tracks() )
            {
                RenderTexture2D target = _tritems[t].target;
                DrawTextureRec(target.texture, (Rectangle){ 0, 0, target.texture.width, -target.texture.height }, (Vector2){ 0, 0 }, WHITE);
            }
            // EndShaderMode();
            EndDrawing();
        }
        CloseWindow();
    }

private:
    StepSequencer * _seq;
    int _width;
    int _height;

    int _targetWidth;
    int _targetHeight;

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
                _tritems.insert( std::make_pair( t, tritem ) );
            }
        }
    }

    void init()
    {

    }

    void renderTrack( Track* t, double radius )
    {

        double circle_width = 20.0;

        int nspaces = t->n_steps();
        int space_width = 5;
        int space_arc = space_width * nspaces;
        int rem_arc = 360 - space_arc;
        double s_arc_len = (double)rem_arc / (double)(t->n_steps());
        double cur_angle = 0.0;

        DrawCircleLines( _targetWidth/2, _targetHeight/2, radius - circle_width / 2.0, DARKBLUE);

        for( int i = 0; i < t->n_steps(); ++i )
        {
//            rlRotatef(30.0, 15.0f, 0.0f, 0.0f);
            DrawRing( (Vector2){_targetWidth/2, _targetHeight/2}, radius - circle_width, radius, cur_angle, cur_angle + s_arc_len, 50, Fade(LIGHTGRAY, 0.7));
            cur_angle += s_arc_len + (double)space_width;
        }

        // DrawRing( (Vector2){1280/2, 720/2}, 100, 120, 0, 270, 0, Fade(BLACK, 0.5));
    }

protected:

};
