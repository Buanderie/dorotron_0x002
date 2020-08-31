#pragma once

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

};