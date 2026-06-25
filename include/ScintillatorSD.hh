/// \file AmBe/include/ScintillatorSD.hh
/// \brief Definition of the AmBeStack::ScintillatorSD class

#ifndef AmBeStackScintillatorSD_h
#define AmBeStackScintillatorSD_h 1

// not a global user action class like g4userrunaction etc.
// instead a virtual class which inherits from G4VSensitiveDetector
#include "G4VSensitiveDetector.hh"
// include detector hit header 
#include "DetectorHit.hh"
#include "globals.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

namespace AmBeStack
{
    class ScintillatorSD : public G4VSensitiveDetector
    {
        public:
            // telling Geant we have something 
            // which follows the same rules as the 
            // G4VSensitiveDetector class
            // the details of which will be in the
            // associated .cc file
            // constructor needs name as argument
            ScintillatorSD(const G4String& name);
            // destructor can be set to default
            ~ScintillatorSD() override = default;

            // can include the collection name as 
            // a public variable so that other classes
            // (eventaction) can see it and doesn't need
            // to be hardcoded
            static const G4String fHitsCollectionName;

            // member functions
            // SD by default has three
            // first is initialize at the start of the event
            // has one argument which is a pointer to 
            // G4HCofThisEvent class
            // ('Hits collection of this event')
            // sets up the structure of the hitcollection
            // ready for processhits to assign things into
            void Initialize(G4HCofThisEvent* hce) override;

            // next is the ProcessHits method
            // this is a bool since it acts like a filter 
            // returns true if particle takes step inside 
            // sensitive volume telling geant 
            // meaningful interaction registered 
            // sets variables within hitcollection for an event
            // according to structure assigned in initialize
            G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;


        private:
            // member variables
            // pointer for storing this events hit array
            // links to DetectorHit.hh header file
            DetectorHitsCollection* fHitsCollection;
    
    };
}

#endif
