#pragma once

#include "track.h"

class EuclideanTrack : public Track
{
public:
    EuclideanTrack( StepSequencer* parent = nullptr )
        :Track(parent)
    {

    }

    virtual ~EuclideanTrack()
    {

    }

private:

protected:

};
