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

        declare_property( "range", 16, [=](nlohmann::json v){
            _range;
        });

        _states.resize( property("steps").get<double>() );
        for( int i = 0; i < n_steps(); ++i )
            _states[ i ] = STEP_IDLE;

        _range = 16;
        set_property( "range", property( "steps" ).get<double>() / 2.0 );

    }

    virtual ~Track()
    {

    }

    virtual bool is_step_current( int i )
    {
        return _active_step_idx == i;
    }

    virtual bool is_step_active( int i )
    {
        return (_states[ i % _range ] == STEP_ACTIVE);
    }

    virtual bool is_outside_range( int i )
    {
//        cerr << "_range=" << _range << endl;
        return i >= _range;
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
        if( _states[ i % _range ] == STEP_ACTIVE )
        {
            for( auto& cb : _cbs )
            {
                cb();
            }
        }
    }

    virtual void trigger()
    {
        if( _active_step_idx < _range )
        {
            _states[ _active_step_idx ] = STEP_ACTIVE;
            _lastActivatedIdx = _active_step_idx;
            _lastActivationTimestamp = getMilliseconds();
        }
    }

    virtual StepState step_state( size_t step_idx )
    {
        return _states[ step_idx % _range ];
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

    size_t _range;

    int _lastActivatedIdx;
    double _lastActivationTimestamp;

};
