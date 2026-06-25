/// \file AmBe/src/ScintillatorSD.cc
/// \brief Implementation of the AmBeStack::ScintillatorSD class

#include "ScintillatorSD.hh"

#include "G4SDManager.hh"

namespace AmBeStack 
{
    // assign the public hitscollectionname explicitly 
    // here globally
    const G4String ScintillatorSD::fHitsCollectionName = "scintillatorHits";


    ScintillatorSD::ScintillatorSD(const G4String& name)
        // : starts "member intialiser list"
        // zone which runs before the code inside {} 
        : G4VSensitiveDetector(name),  // - parent setup (from virtual geant class)
        // in this case just the name of the specific sensitive detector
        // assign member variables (defined in .hh file)
        // only the hitscollection variable in this case
        fHitsCollection(nullptr) // assign as nullptr initially
    {
        // going to register the name of the data table detector generates
        // do this in the constructor so that the assignment is done 
        // before any events start, when the SDManager looks for 
        // the data collectoins ScintillatorSD will make
        // if this was done in the Initialize function, 
        // would be assigned at start of each event
        // which would be after the SDManager has been told
        // (through lack of this line)
        // that there will be no data collections made
        collectionName.insert(fHitsCollectionName);
    }

    // member function definitions
    // 1. Initialize (runs at start of each event)
    void ScintillatorSD::Initialize(G4HCofThisEvent* hce)
    {
        // this assigns the correct layout of the hitscollection for 
        // each event
        // start by initialising a hitcollection
        // remember DetectorHitsCollection was a typedef for 
        // the default G4THitsCollection<DetectorHit> class
        // G4THitsCollection takes two arguments
        // detName (SensitiveDetectorName), and colName (collectionName[0])
        // SensitiveDetectorName is inheritted for free from the parent
        // G4VSensitiveDetector class (setup already in the constructor)
        // collectionName[0] is the specific data table registered in the constructor
        // "scintillatorHits" in this case
        // together make a new hitcollection table with a unique ID 
        // mapped to the SD/scintillatorHits
        fHitsCollection = new DetectorHitsCollection(SensitiveDetectorName, collectionName[0]);

        // next step is to get the collectionID from the SDManager
        // first create a static variable (exists in memory across function calls)
        static G4int hcID = -1;
        
        // then can guard against calling the heavy GetCollectionID operation
        // when already have it
        if (hcID < 0)
        {
            // lookup the collectionID if don't already have it 
            // (should just run at the start of event 1)
            // hcID should stay the same for all events
            // since the category of data - scintllationHits should be the same
            // acros the entire run, we don't want this to change
            // and so we make sure the hcID doesn't change with this if{}
            hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
        }

        // assign the hcID with the event specific 
        // fHitsCollection to the 
        // hitscollection for this event (hce)
        hce->AddHitsCollection(hcID, fHitsCollection);
    }

    // now for the ProcessHits member function
    // runs dynamically through the event
    // step level function inside volume
    // returns a boolean depending on if particle exists 
    // within sensitive detector volume
    G4bool ScintillatorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
    {
        // this deals with the setters and getter variables and functions
        
        // get energy deposited in the step
        G4double edep = step->GetTotalEnergyDeposit();

        // filter out zero energy passes (for now)
        if (edep == 0.) return false;

        // create a hit object otherwise
        // a row within the hitcollection for this event (hce)
        auto newHit = new DetectorHit();

        // fill with various setters 
        newHit->SetEdep(edep);
        newHit->SetTime(step->GetPreStepPoint()->GetGlobalTime());
        newHit->SetCopyNo(step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
        newHit->SetTrackID(step->GetTrack()->GetTrackID());
        newHit->SetParticleName(step->GetTrack()->GetDefinition()->GetParticleName());
        newHit->SetIsRecoil(step->GetTrack()->GetDefinition()->GetBaryonNumber() >= 1);
    
        // do not need to guard against different interactions types here
        // no if statements apart form to get bool for IsRecoil
        // simply write everthing as it is
        // then in events action we can apply some filtering 
        // and output to the ntuple like before

        // sent to hits collection
        fHitsCollection->insert(newHit);

        // need to return a bool for function contents to match
        // definition
        return true;
    }
}


