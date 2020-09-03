
#include <iostream>
using namespace std;

#include "stepsequencer.h"
#include "debugtrack.h"
#include "sequencergui.h"

int main( int argc, char** argv )
{
    srand(time(NULL));

    StepSequencer sequencer;
    sequencer.start();

    for( int k = 0; k < 8; ++k )
    {
        Track * dt0 = new DebugTrack(&sequencer);
        sequencer.addTrack(dt0);
        dt0->set_property("steps", 32);
    }

//    Track * dt1 = new DebugTrack(&sequencer);
//    sequencer.addTrack(dt1);
//    dt1->set_property("steps", 17);

    SequencerGUI gui( &sequencer );
    gui.run();

	return 0;
}
