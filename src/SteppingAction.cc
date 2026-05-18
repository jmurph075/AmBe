/// \file AmBe/src/SteppingAction.cc
/// \brief Implementation of the AmBeStack::SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"

namespace AmBeStack
{
    SteppingAction::SteppingAction(EventAction* eventAction)
        // : starts "member intialiser list"
        // zone which runs before the code inside {} 
        : G4UserSteppingAction(), // - parent setup (from virtual geant class)
        // assign member variables (defined in .hh file)
        fEventAction(eventAction), // - for current event action 
        fScoringVolume(nullptr) // - for scoring volume
        {}

    // actually define the usersteppingaction function
    // returntype class::functionname(args)
    // called at each step in the simulation
    void SteppingAction::UserSteppingAction(const G4Step* step)
    {
        // 1. Get scoring volume
        if (!fScoringVolume) // - if still nullptr (hasn't been found yet) 
        {
            // get the detector construction from the runmanager
            const auto detConstruction = static_cast<const DetectorConstruction*>(
                G4RunManager::GetRunManager()->GetUserDetectorConstruction()
            );
            // grab scoring volume from detConstruction
            // and assign
            fScoringVolume = detConstruction->GetScoringVolume();
        }

        // 2. Check volume of the current step
        // need to know if in the scoring volume
        auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
        if (volume != fScoringVolume) return;

        // 3. Get the energy deposited in this step
        G4double edep = step->GetTotalEnergyDeposit();
        // pass this to fEventAction
        fEventAction->AddEdep(edep);

        // 4. Get particle associated with each track
        auto track = step->GetTrack();
        auto particle = track->GetDefinition();

        // 5. Check if recoil nucleus
        if (particle->GetBaryonNumber() >= 1)
        {   
            // if so, get type and its kinetic energy

            // get ID of track
            G4int trackID = track->GetTrackID(); 

            // if this is the first step, has just recoiled with set KE
            if (track->GetCurrentStepNumber() == 1)
            {
                // pass nuclei info to maps
                
                // get nuclei
                fEventAction->recoilNameMap[trackID] = particle->GetParticleName();
                // get initial KE
                fEventAction->recoilInitialKEMap[trackID] = step->GetPreStepPoint()->GetKineticEnergy();
            }

            // if not the first step, 
            // check if paricle going to leave volume or has stopped
            auto postPoint = step->GetPostStepPoint();
            auto postVolume = postPoint->GetTouchableHandle()->GetVolume;
            auto postLogVolume = postVolume->GetLogicalVolume();
            if (postLogVolume != fScoringVolume || track->GetTrackStatus == fStopAndKill)
            {
                // get final KE
                fEventAction->recoilFinalKEMap[trackID] = postPoint->GetKineticEnergy();
            }
        }

    }

}