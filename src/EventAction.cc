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
    EventAction::EventAction(RunAction* runAction)
        // : starts "member intialiser list"
        // zone which runs before the code inside {} 
        : G4UserEventAction(), // - parent setup (from geant class)
        // assign member variables (defined in .hh file)
        fRunAction(runAction) // - for current run action 
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

        // wipe the maps containing the information about the event
        fTrackNameMap.clear(); // new recoil name
        fTrackEdepMap.clear(); // new energy deposited by this recoil from event
        fDetTimeSummaryMap.clear(); // new time when hit occurs in detector 
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

        // make a varible to hold the total energy deposited 
        // across the event
        G4double totalEdep = 0.0;
        G4double totalDet0Edep = 0.0;
        G4double totalDet1Edep = 0.0;

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
                G4bool isRecoil = hit->GetIsRecoil();
                
                // add the edep in this hit to the total event edep
                totalEdep += edep;
                if (copyNo == 0)
                {
                    totalDet0Edep += edep;
                    // do the same for the energy deposited by the track
                    // for each detector based on the copyNo
                    fTrackEdepMap[trackID].first += edep;
                }
                else if (copyNo == 1)
                {
                    totalDet1Edep += edep;
                    fTrackEdepMap[trackID].second += edep;
                }

                // now we look at the type of interaction

                // make a new entry in the name map and edep map
                // if not made already (still cleared from beginofeventaction)
                // .find grabs the value associated with the track ID
                // if returns end, means this trackID is new to the map
                if (fTrackNameMap.find(trackID) == fTrackNameMap.end())
                {
                    // so we assign it for the first time
                    fTrackNameMap[trackID] = particleName;
                }

                // need to record the time associated with
                // the copy number
                // fill map for this aswell
                
                // !!!!! FOR NOW JUST RECORD FIRST TIME HIT
                // IN THIS DETECTOR RECORDED !!!!!
                // !!!!! SHOULD REALLY REFINE SO HAS TIME 
                // OF FIRST AND LAST HIT IN DETECTOR TO BE CORRECT !!!
                if (fDetTimeSummaryMap.find(copyNo) == fDetTimeSummaryMap.end())
                {   
                    fDetTimeSummaryMap[copyNo] = std::make_pair(time, time);
                }

                // can include else statement that
                // updates the pair
                // till it contains the first and last time in the event
                // since we know we already have an entry for this copy no.
                // apparently geant doesn't go through these hits monotonically
                // in time, so need to just call min/max when looping
                // over them
                else
                {
                    // is the new time smaller than the current 
                    // first entry in the map for this copy no.?
                    // if yes, redefine
                    fDetTimeSummaryMap[copyNo].first = std::min(fDetTimeSummaryMap[copyNo].first, time);
                    // is the new time larger than the current 
                    // second entry in the map for this copy no.?
                    // if yes, redefine
                    fDetTimeSummaryMap[copyNo].second = std::max(fDetTimeSummaryMap[copyNo].second, time);
                }
                

            }
            
        }

        // have collected all quantities across all hits collections
        // associated with the event
        // now need to write this to the ntuple
        // defined in runaction 

        // determine if both detectors were involved in event
        // set some default values for false cases
        std::pair<G4double, G4double> timeDet0 = std::make_pair(-1.0, -1.0);
        std::pair<G4double, G4double> timeDet1 = std::make_pair(-1.0, -1.0);
        G4double tof = 0.0;

        // check for 1st detector
        // if copy no. 0 IS found in the map for this event
        if (fDetTimeSummaryMap.find(0) != fDetTimeSummaryMap.end())
        {
            timeDet0 = fDetTimeSummaryMap[0];
        }
        // analogous for copy no. 1
        if (fDetTimeSummaryMap.find(1) != fDetTimeSummaryMap.end())
        {
            timeDet1 = fDetTimeSummaryMap[1];
        }

        // can now calculate the time difference between each
        if (timeDet0.first >= 0.0 && timeDet1.first >= 0.0)
        {
            tof = timeDet1.first - timeDet0.first;
        }

        // deal with case where no energy was deposited 
        // in either detector
        if (fTrackEdepMap.empty())
        {
            // fill the ntuple with default values
            // first add eventID
            analysisManager->FillNtupleIColumn(0, eventID);
            // then add the total energy for the event
            analysisManager->FillNtupleDColumn(1, totalEdep);
            // add total energy for each detector (will just be 0)
            analysisManager->FillNtupleDColumn(2, totalDet0Edep);
            analysisManager->FillNtupleDColumn(3, totalDet1Edep);
            // specify no recoil for recoil type
            analysisManager->FillNtupleSColumn(4, "No_recoil");
            // specify no recoil energy for both detectors
            analysisManager->FillNtupleDColumn(5, 0.0);
            analysisManager->FillNtupleDColumn(6, 0.0);
            // specify the time into each detector and TOF
            // will be the default values set above in this case
            analysisManager->FillNtupleDColumn(7, timeDet0.first);
            analysisManager->FillNtupleDColumn(8, timeDet0.second);
            analysisManager->FillNtupleDColumn(9, timeDet1.first);
            analysisManager->FillNtupleDColumn(10, timeDet1.second);
            analysisManager->FillNtupleDColumn(11, tof);
            // add new row now that event info been written
            analysisManager->AddNtupleRow();
        }

        // otherwise, have some energy deposition
        // the positive case!
        else
        {
            // loop over each track within the event
            for (auto const& [trackID, recoilEdep] : fTrackEdepMap)
            {
                // get associated recoilname
                G4String recoilName = fTrackNameMap[trackID];
                // first add eventID
                analysisManager->FillNtupleIColumn(0, eventID);
                // then add the total energy for the event
                analysisManager->FillNtupleDColumn(1, totalEdep);
                // add total energies in each detector
                analysisManager->FillNtupleDColumn(2, totalDet0Edep);
                analysisManager->FillNtupleDColumn(3, totalDet1Edep);
                // specify recoil type
                analysisManager->FillNtupleSColumn(4, recoilName);
                // specify no recoil energy for both detectors
                analysisManager->FillNtupleDColumn(5, recoilEdep.first);
                analysisManager->FillNtupleDColumn(6, recoilEdep.second);
                // specify the time into each detector and TOF
                analysisManager->FillNtupleDColumn(7, timeDet0.first);
                analysisManager->FillNtupleDColumn(8, timeDet0.second);
                analysisManager->FillNtupleDColumn(9, timeDet1.first);
                analysisManager->FillNtupleDColumn(10, timeDet1.second);
                analysisManager->FillNtupleDColumn(11, tof);
                // add new row now that event info been written
                analysisManager->AddNtupleRow();
            }
        }

    }
}

