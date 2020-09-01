#pragma once

#include "stepsequencer.h"

#include "raylib.h"
#include "rlgl.h"

class SequencerGUI
{
public:
    SequencerGUI( StepSequencer* seq = nullptr, int width = 1280, int height = 720 )
    :_seq(seq), _width(width), _height(height)
    {
        init();
    }

    virtual ~SequencerGUI()
    {

    }

    void run()
    {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(_width, _height, "dorotron_0x002" );
        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            renderTrack(nullptr);
            EndDrawing();
        }
        CloseWindow();
    }

private:
    StepSequencer * _seq;
    int _width;
    int _height;

    void init()
    {

    }

    void renderTrack( Track* t )
    {
        DrawCircleLines( 1280/2, 720/2, 110, DARKBLUE);
        DrawRing( (Vector2){1280/2, 720/2}, 100, 120, 0, 270, 0, Fade(BLACK, 0.3));
    }

protected:

};
