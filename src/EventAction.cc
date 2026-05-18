/// \file AmBe/src/EventAction.cc
/// \brief Implementation of the AmBeStack::EventAction class

#include "EventAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "globals.hh"

namespace AmBeStack
{
    // like in SteppingAction.cc, start with the constructor
    // this works to link EventAction to RunAction
    EventAction::EventAction(RunAction* runAction)
        // : starts "member intialiser list"
        // zone which runs before the code inside {} 
        : G4UserEventAction(), // - parent setup (from virtual geant class)
        // assign member variables (defined in .hh file)
        fRunAction(runAction), // - for current run action 
        fEdep(0.) // - for energy deposited in this event
        {}

    // first member function
    // actually define the BeginOfEventAction function
    // returntype class::functionname(args)
    // called at each start of each event in the simulation
    // acts as a hard RESET
    void EventAction::BeginOfEventAction(const G4Event*)
    {
        fEdep = 0.; // reset total event energy back to 0

        // wipe the maps containing the information about the interaction
        // of the neutron
        recoilNameMap.clear();
        recoilInitialKEMap.clear();
        recoilFinalKEMap.clear();
    }

    void 
}

