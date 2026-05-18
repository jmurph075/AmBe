/// \file AmBe/include/SteppingAction.hh
/// \brief Definition of the AmBeStack::SteppingAction class

#ifndef AmBeStackSteppingAction_h
#define AmBeStackSteppingAction_h 1

#include "G4UserSteppingAction.hh"

#include "globals.hh"

namespace AmBeStack
{

    // need to include EventAction class within SteppingAction header
    // for things to link (steps->event)
    class EventAction;

    // SteppingAction class itself
    class SteppingAction : public G4UserSteppingAction
    {
        public:
            // telling Geant we have something 
            // which follows the same rules as the 
            // G4UserSteppingAction class
            // the details of which will be in the
            // associated .cc file
            SteppingAction(EventAction* eventAction);
            ~SteppingAction() override = default;

            // member function
            void UserSteppingAction(const G4Step* step) override;

        private:
            // define the member variables 
            EventAction* fEventAction = nullptr;
            G4LogicalVolume* fScoringVolume = nullptr;
    }

}

#endif

