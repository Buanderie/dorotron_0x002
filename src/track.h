#pragma once

#include <chrono>

#include "parameterizable.h"

class StepSequencer;
class Track : public Parameterizable
{

public:

    typedef enum
    {
        STEP_IDLE = 0,
        STEP_ACTIVE
    } StepState;

    Track( StepSequencer* parent = nullptr )
        :_parent(parent)
    {
        declare_property( "steps", 16, [=](nlohmann::json v){
            onNumberOfStepsChanged( n_steps() );
        });
        _states.resize( property("steps").get<double>() );
        for( int i = 0; i < n_steps(); ++i )
            _states[ i ] = STEP_IDLE;
        _states[ 3 ] = STEP_ACTIVE;
    }

    virtual ~Track()
    {

    }

    virtual int active_step_idx()
    {
        return _active_step_idx;
    }

    virtual bool recently_activated( int i )
    {
        if( getMilliseconds() - _lastActivationTimestamp <= 100.0 )
        {
            if( _lastActivatedIdx >= 0 && _lastActivatedIdx < this->n_steps() && _lastActivatedIdx == i )
            {
                return true;
            }
        }
        return false;
    }

    int n_steps()
    {
        return this->property( "steps" ).get<int>();
    }

    virtual void step( int istep )
    {
        int i = istep % n_steps();
        _active_step_idx = i;
        if( _states[i] == STEP_ACTIVE )
        {
            for( auto& cb : _cbs )
            {
                cb();
            }
        }
    }

    virtual void trigger()
    {
        _states[ _active_step_idx ] = STEP_ACTIVE;
        _lastActivatedIdx = _active_step_idx;
        _lastActivationTimestamp = getMilliseconds();
    }

    virtual StepState step_state( size_t step_idx )
    {
        return _states[ step_idx ];
    }

    void add_trigger_callback( std::function<void(void)> f )
    {
        _cbs.push_back( f );
    }

private:

protected:
    StepSequencer * _parent;
    std::vector< std::function<void(void)> > _cbs;

    //
    virtual void onNumberOfStepsChanged( int steps )
    {
        _states.resize( steps );
        for( int i = 0; i < steps; ++i )
            _states[ i ] = STEP_IDLE;
    }
    //

    double getMilliseconds()
    {
        double milliseconds_since_epoch =
                std::chrono::high_resolution_clock::now().time_since_epoch() /
                std::chrono::nanoseconds(1);
        return milliseconds_since_epoch / 1000000.0;
    }

    std::vector< Track::StepState > _states;
    int _active_step_idx;

    int _lastActivatedIdx;
    double _lastActivationTimestamp;

};
