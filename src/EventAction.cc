/// \file AmBe/src/EventAction.cc
/// \brief Implementation of the AmBeStack::EventAction class

#include "EventAction.hh"
#include "DetectorHit.hh"
#include "ScintillatorSD.hh"
#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "globals.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

namespace AmBeStack
{
    // like in SteppingAction.cc, start with the constructor
    // this works to link EventAction to RunAction
    EventAction::EventAction()
        // : starts "member intialiser list"
        // zone which runs before the code inside {} 
        : G4UserEventAction() // - parent setup (from geant class)
        // assign member variables (defined in .hh file)
        {}

    // first member function
    // actually define the BeginOfEventAction function
    // returntype class::functionname(args)
    // called at start of each event in the simulation
    // acts as a hard RESET
    void EventAction::BeginOfEventAction(const G4Event* event)
    {
                // !!!!!!!!!!!!! for debugging !!!!!!!!!!
        //G4cout << "Coming from BeginOfEventAction" << G4endl;

        // since writing directly from hits to ntuples,
        // no need to reset maps at start of event 
        // (we don't have any)
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

        // first get the hits collection ID for this event 
        G4HCofThisEvent* hce = event->GetHCofThisEvent();

        // from this can call the hcID (again)
        // within the eventaction class 
        // previous static assignment within scintillatorSD 
        // was only local to the class, even though it exists 
        // throughout lifetime of the simulation.
        static G4int hcID = -1;
        if (hcID < 0)
        {
            hcID = G4SDManager::GetSDMpointer()->GetCollectionID(
                ScintillatorSD::fHitsCollectionName);
        }

        // can then get the hits collection itself by referencing
        // with the ID just found 
        // check hce exists first
        if (!hce) return;

        auto hitsCollection = static_cast<DetectorHitsCollection*>(hce->GetHC(hcID));


        // make an analysis manager instance
        // allows us to write from the map values to the output file
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        // get the eventID aswell for referencing later
        G4int eventID = event->GetEventID();

        // need to grab the primary energy from within this class now
        // rather than from primarygeneratoraction,
        // since each row is a hit, not an event, 
        G4double primaryEnergy = 0.0;
        // get primary vertex for the event
        // will update this later to include simulataneous primary gammas
        // first 0 is for the first vertex (only going to be one in this sim)
        // second 0 is for the first kind of primary particle
        // this will be only a neutron for now,
        // but will update to include gammas 
        primaryEnergy = event->GetPrimaryVertex(0)->GetPrimary(0)->GetKineticEnergy();

        // can then loop through the number of hits in collection
        // (as long as it exists)
        if (hitsCollection)
        {
            G4int nHit = hitsCollection->entries();

            // loop over this
            for (G4int iHit = 0; iHit < nHit; iHit++)
            {
                // get the hit for this index
                // get pointer to hitsCollection 
                DetectorHit* hit = (*hitsCollection)[iHit];

                // from this, can get all the quantities of interest
                // for this hit
                // through calling getter functions
                G4double edep = hit->GetEdep();
                G4double time = hit->GetTime();
                G4int copyNo = hit->GetCopyNo();
                G4int trackID = hit->GetTrackID();
                G4String particleName = hit->GetParticleName();
                G4String processName = hit->GetProcessName();
                G4double incidentNeutronEnergy = hit->GetIncidentEnergy();
                
                // write these quantities to ntuple
                // column 0 is eventID
                analysisManager->FillNtupleIColumn(0, eventID);
                // column 1 is primary particle energy
                analysisManager->FillNtupleDColumn(1, primaryEnergy); // ensure in MeV
                // column 2 is copyNo
                analysisManager->FillNtupleIColumn(2, copyNo);
                // column 3 is time of hit
                analysisManager->FillNtupleDColumn(3, time);
                // column 4 is energy deposition of hit
                analysisManager->FillNtupleDColumn(4, edep);
                // column 5 is particle name
                analysisManager->FillNtupleSColumn(5, particleName);
                // column 6 is process name
                analysisManager->FillNtupleSColumn(6, processName);
                // column 7 is trackID
                analysisManager->FillNtupleIColumn(7, trackID);
                // column 8 is incident neutron energy
                analysisManager->FillNtupleDColumn(8, incidentNeutronEnergy);

                // after each hit, start new row
                analysisManager->AddNtupleRow();

            }
            
        }


    }
}

