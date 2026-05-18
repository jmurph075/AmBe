/// \file AmBe/include/EventAction.hh
/// \brief Definition of the AmBeStack::EventAction class

#ifndef AmBeStackEventAction_h
#define AmBeStackEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <map> // !! NEED FOR MAPS !!

namespace AmBeStack
{
    // need to include RunAction class within EventAction header
    // for things to link (events->run)
    class RunAction;

    // EventAction class itself
    class EventAction : public G4UserEventAction
    {
        public:
            // telling Geant we have something 
            // which follows the same rules as the 
            // G4EventAction class
            // the details of which will be in the
            // associated .cc file
            EventAction(RunAction* runAction);
            ~EventAction() override = default;

            // first member function
            void BeginOfEventAction(const G4Event* event) override;

            // second member function
            void EndOfEventAction(const G4Event* event) override;

            // also need the AddEdep function
            // which is called inside SteppingAction::UserSteppingAction
            // to accumulate the total energy deposited in the event
            // needs to be public for this access
            void AddEdep(G4double edepStep)
            {
                fEdep += edepStep;
            }

            // finally define the member variables for the maps
            // again must be public so steppingaction can fill
            std::map<G4int, G4String> recoilNameMap;
            std::map<G4int, G4double> recoilEdepMap;

        private:
            // define member variables
            RunAction* fRunAction = nullptr;
            G4double fEdep = 0.; // no * as don't want a pointer just normal double for value 

    };
}

#endif