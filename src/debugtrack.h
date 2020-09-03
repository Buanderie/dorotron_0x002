#pragma once

#include "track.h"

class DebugTrack : public Track
{
public:
    DebugTrack( StepSequencer* parent )
        :Track(parent)
    {
        _active_step_idx = -1;
    }

    virtual ~DebugTrack()
    {

    }

    virtual int active_step_idx()
    {
        return _active_step_idx;
    }

    virtual void step(int istep) override
    {

        double t = getMilliseconds();
        double dt = t - _last_t;
        double bpm = 60.0 / (dt / 1000.0);
        cerr << "***** dt=" << dt << " bpm=" << bpm << endl;
        int i = istep % n_steps();
        _active_step_idx = i;
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
        _last_t = t;
    }

private:
    int _active_step_idx;
    double _last_t;

protected:

};
