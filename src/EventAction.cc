/// \file AmBe/src/EventAction.cc
/// \brief Implementation of the AmBeStack::EventAction class

#include "EventAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "globals.hh"
#include "G4Event.hh"

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
    // called at start of each event in the simulation
    // acts as a hard RESET
    void EventAction::BeginOfEventAction(const G4Event* event)
    {
        fEdep = 0.; // reset total event energy back to 0

        // wipe the maps containing the information about the interaction
        // of the neutron (event)
        eventIDMap.clear(); // new event ID
        totEventEdepMap.clear(); // new tot energy deposited from the event
        recoilNameMap.clear(); // new recoil name
        recoilEdepMap.clear(); // new energy deposited by this recoil from event
    }

    // second member function
    // actually define EndOfEventAction function
    // returntype class::functionname(args)
    // called at end of each event in simulation
    // may contain information about multiple particles
    // all stemming from same event
    // need to go through and process each of these
    void EventAction::EndOfEventAction(const G4Event* event)
    {
        // make an analysis manager instance
        // allows us to write from the map values to the output file
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        // get the eventID
        G4int eventID = event->GetEventID();

        // if we have generated no recoil nuclei within the c6d6
        // recoilEdepMap will still be cleared from the BeginOfEventAction
        // since the if (particle->GetBaryonNumber() >= 1) loop in
        // steppingaction.cc won't have ran
        // still need to record data though as event still occurred
        if (recoilEdepMap.empty())
        {
            analysisManager->FillNTupleIColumn(0, eventID); // column 0, unique event ID (integer)
            analysisManager->FillNTupleDColumn(1, fEdep); // column 1, total event energy deposition
            analysisManager->FillNTupleSColumn(2, "No_recoil"); // column 2, would've been recoil nuclei name
            analysisManager->FillNTupleDColumn(3, 0.0); // column 3, recoil energy (none here)
            // move to new row
            analysisManager->AddNTupleRow();
        }
        // otherwise, recoil nuclei must've been tracked
        else
        {
            // go through each of them and add extract
            // maps have following structure:
            // map[Key, Value]
            // for each pair in recoilEdepMap: {...}
            // this is because we may have multiple pairs
            // [trackID, recoilEdep] for multiple recoils
            // associated with the same event
            // auto sorts typing
            // const means things can't be modified in the map
            // & means reference (look directly at data inside map, don't copy)
            for (auto const& [trackID, recoilEdep] : recoilEdepMap)
            {
                // get corresponding recoilNameMap value
                G4String recoilName = recoilNameMap[trackID];
                // write to analysis manager
                analysisManager->FillNTupleIColumn(0, eventID);
                analysisManager->FillNTupleDColumn(1, fEdep);
                analysisManager->FillNTupleSColumn(2, recoilName);
                analysisManager->FillNTupleDColumn(3, recoilEdep);

                // move to new row for either new event or new recoil nuclei
                analysisManager->AddNTupleRow();
            }
        }
    }
}

