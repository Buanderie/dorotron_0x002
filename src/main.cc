
#include <iostream>
using namespace std;

#include "stepsequencer.h"
#include "debugtrack.h"
#include "sequencergui.h"

int main( int argc, char** argv )
{
    StepSequencer sequencer;
    sequencer.start();

    Track * dt0 = new DebugTrack(&sequencer);
    sequencer.addTrack(dt0);

    dt0->set_property("steps", 8);

//    Track * dt1 = new DebugTrack(&sequencer);
//    sequencer.addTrack(dt1);
//    dt1->set_property("steps", 17);


    SequencerGUI gui( &sequencer );
    gui.run();

	return 0;
}
