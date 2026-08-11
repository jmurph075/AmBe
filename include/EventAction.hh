/// \file AmBe/include/EventAction.hh
/// \brief Definition of the AmBeStack::EventAction class

#ifndef AmBeStackEventAction_h
#define AmBeStackEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <map> // !! NEED FOR MAPS !!
#include <vector>
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
            // G4UserEventAction class
            // the details of which will be in the
            // associated .cc file
            EventAction();
            ~EventAction() override = default;

            // first member function
            void BeginOfEventAction(const G4Event* event) override;

            // second member function
            void EndOfEventAction(const G4Event* event) override;

    };
}

#endif