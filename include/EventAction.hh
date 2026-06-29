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
            // G4UserEventAction class
            // the details of which will be in the
            // associated .cc file
            EventAction(RunAction* runAction);
            ~EventAction() override = default;

            // first member function
            void BeginOfEventAction(const G4Event* event) override;

            // second member function
            void EndOfEventAction(const G4Event* event) override;

        private:
            // define member variables
            // this is immediately overwritten anyway
            // once the member initialisation occurs
            RunAction* fRunAction = nullptr;
            // in this case the maps to structure the ntuples
            std::map<G4int, G4String> fTrackNameMap;
            std::map<G4int, std::pair<G4double, G4double>> fTrackEdepMap;
            std::map<G4int, std::pair<G4double, G4double>> fDetTimeSummaryMap;

    };
}

#endif