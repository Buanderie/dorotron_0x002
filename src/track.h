#pragma once

#include <chrono>

#include "parameterizable.h"

class StepSequencer;
class Track : public Parameterizable
{

public:
    Track( StepSequencer* parent = nullptr )
        :_parent(parent)
    {
        declare_property( "steps", 16, [=](nlohmann::json v){
            onNumberOfStepsChanged( n_steps() );
        });
    }

    virtual ~Track()
    {

    }

    virtual int active_step_idx()
    {
        return -1;
    }

    int n_steps()
    {
        return this->property( "steps" ).get<int>();
    }

    virtual void step( int istep )
    {

    }

private:

protected:
    StepSequencer * _parent;

    //
    virtual void onNumberOfStepsChanged( int steps )
    {

    }
    //

    double getMilliseconds()
    {
        double milliseconds_since_epoch =
                std::chrono::high_resolution_clock::now().time_since_epoch() /
                std::chrono::nanoseconds(1);
        return milliseconds_since_epoch / 1000000.0;
    }

};
