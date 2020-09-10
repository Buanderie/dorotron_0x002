#pragma once

#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

#include "stepsequencer.h"

#include "raylib.h"
#include "rlgl.h"
#include "easings.h"

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
        // SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(_width, _height, "dorotron_0x002" );
        SetTargetFPS(60);
        _font = LoadFont("../resources/MODENINE.TTF");


        //        _blurredTarget = LoadRenderTexture(_width, _height);
        //        _clearTarget = LoadRenderTexture(_width, _height);
        shaders[0] = LoadShader(0, TextFormat("../resources/pute.fs", GLSL_VERSION));
        shaders[1] = LoadShader(0, TextFormat("../resources/blur.fs", GLSL_VERSION));

        _bgColor = CLITERAL(Color){ 20, 20, 20, 255 };
        _primaryColor = DARKBLUE;

        while (!WindowShouldClose())
        {

            if (IsKeyPressed(KEY_UP)) _active_idx = min( _active_idx + 1, (ssize_t)_seq->tracks().size() - 1 );
            if (IsKeyPressed(KEY_DOWN)) _active_idx = max( _active_idx - 1, (ssize_t)0 );

            if( IsKeyPressed(KEY_SPACE) )
            {
                _seq->tracks()[ _active_idx ]->trigger();
            }

            BeginDrawing();
            ClearBackground( _bgColor );  // Clear texture background

            int r = 120;
            int i = 0;
            for( auto t : _seq->tracks() )
            {
                if( i == _active_idx )
                {
                    renderTrack(t, r, true);
                }
                else
                {
                    renderTrack(t, r, false);
                }
                r += 50;
                i = i+1;
            }

            DrawRectangle( 0, GetScreenHeight() - 40, GetScreenWidth(), 40, DARKGRAY );
            stringstream sstr;
            sstr << "CHANNEL_0x" << std::setfill('0') << std::setw(2) << hex << _active_idx;
            DrawTextEx(_font, sstr.str().c_str(), (Vector2){ (float)(GetScreenWidth() / 2 - 120), _height - 33.0f }, _font.baseSize * 1.0, 2, RAYWHITE);
            DrawFPS(0,0);

            EndDrawing();

        }
        CloseWindow();
    }

private:
    StepSequencer * _seq;
    int _width;
    int _height;

    Color _bgColor;
    Color _primaryColor;

    Camera _camera;
    int _targetWidth;
    int _targetHeight;

    ssize_t _active_idx;

    Shader shaders[16] = { 0 };
    Font _font;

    void init()
    {

    }

    void DrawCircleLinesThick(int centerX, int centerY, float radius, float thickness, Color color)
    {
        float thickness_2 = thickness / 2.0f;
        for( float qq = radius - thickness_2; qq < radius + thickness_2; qq += (thickness / 10.0f) )
            DrawCircleLines( centerX, centerY, qq, color );
    }

    double randMToN(double M, double N)
    {
        return M + (rand() / ( RAND_MAX / (N-M) ) ) ;
    }

    Color lighter( Color c, float percentLighter )
    {
        float r = (percentLighter / 100.0f);
        float chibre = (float)(c.r + c.g + c.b) / 3.0f;
        chibre *= r;

        c.r += chibre;
        c.g += chibre;
        c.b += chibre;
        return c;
    }

    void renderTrack( Track* t, double radius, bool isTrackSelected )
    {

        double circle_width = 30.0;

        int nspaces = t->n_steps();
        int space_width = 5;
        int space_arc = space_width * nspaces;
        int rem_arc = 360 - space_arc;
        double s_arc_len = (double)rem_arc / (double)(t->n_steps());
        double cur_angle = 0.0;

        if( isTrackSelected )
        {
            DrawCircleLinesThick( _width/2, _height/2, radius - circle_width / 2.0, 2.0, RAYWHITE );
        }
        else
        {
            DrawCircleLinesThick( _width/2, _height/2, radius - circle_width / 2.0, 2.0, _primaryColor );
        }

        for( int i = 0; i < t->n_steps(); ++i )
        {

            Color c = lighter(_primaryColor, 100.0f);
            double bgThickness = 4.0;

            double pos_noise_x = 0.0;
            double pos_noise_y = 0.0;

            if( t->is_step_current( i ) )
            {
                pos_noise_x = randMToN( -3.0, 3.0 );
                pos_noise_y = randMToN( -3.0, 3.0 );
            }

            if( t->step_state( i ) == Track::STEP_IDLE )
            {
                c = _bgColor;
                if( t->is_outside_range( i ) )
                {
                    c = Fade( c, 0.2 );
                }
                bgThickness = 8.0;
            }

            double cRadius = 10.0;
            if( t->recently_activated( i ) )
            {
                cRadius = 20.0;
                pos_noise_x *= 1.5;
                pos_noise_y *= 1.5;
            }

            double x = cos( (cur_angle * M_PI) / 180.0 ) * (radius - circle_width/2.0) + _width/2;
            double y = sin( (cur_angle * M_PI) / 180.0 ) * (radius - circle_width/2.0) + _height/2;
            x += pos_noise_x;
            y += pos_noise_y;

            DrawCircle( x, y, 10.0, c );

            Color circleColor = _primaryColor;
            if( t->is_step_active( i ) )
            {
                circleColor = lighter(_primaryColor, 90.0f);
            }

            if( t->is_outside_range( i ) )
            {
                circleColor = Fade( circleColor, 0.2 );
            }

            DrawCircleLinesThick( x, y, cRadius, 4.0, circleColor );

            cur_angle += s_arc_len + (double)space_width;
        }
    }

protected:

};
