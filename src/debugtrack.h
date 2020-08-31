#pragma once

#include "track.h"
#include <curses.h>

class DebugTrack : public Track
{
public:
    DebugTrack( StepSequencer* parent )
        :Track(parent)
    {

    }

    virtual ~DebugTrack()
    {

    }

    virtual void step(int istep) override
    {
        int i = istep % n_steps();
        if( i == 0 )
        {
            cout << "\a" << flush;
        }
        std::cerr << "[";
        for( int k = 0; k < n_steps(); ++k )
        {
            if( k == i )
            {
                std::cerr << "#";
            }
            else
            {
                std::cerr << "_";
            }
        }
        std::cerr << "]" << std::endl;
    }

private:

protected:

};
