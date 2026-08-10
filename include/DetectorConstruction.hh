// ********************************************************************
//
//
/// \file AmBe/src/DetectorConstruction.hh
/// \brief Definition of the AmBeStack::DetectorConstruction class

#ifndef AmBeStackDetectorConstruction_h 
#define AmBeStackDetectorConstruction_h 1
// included to guard from multiple inclusions of header file at compilation

// need access to builtin geant materials and detector construction classes
#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"

// initialise default geant geometry construction classes
// known as 'forward declarations' - just tells compiler these classes exist
// will be define later in .cc, but allows us to use pointers below in methods 
// without including full definitions (faster compilation essentially)
class G4VPhysicalVolume;
class G4LogicalVolume;

namespace AmBeStack // keeps things in AmBeStack namespace to avoid conflicts with any other geant sims
{
    // define detector construction class,
    // contents of which is implemented in DetectorConstruction.cc
    class DetectorConstruction : public G4VUserDetectorConstruction
    {
        public:
            // section tells compiler to use default constructor and destructor 
            // 
            DetectorConstruction() = default; 
            // set as default constructor for now, can add arguments later if want more flexibility 
            // i.e to be able to change dimensions of geometry without having to edit source code .cc
            ~DetectorConstruction() override = default;
            // this part tells the compiler to use default destructor
            // override keyword tells compiler this method is overriding 'virtual'
            // method in base class G4VUserDetectorConstruction,
            // which doesn't have any implementation but defines interface
            // we want the specific implementation we provide in this model to be used

            G4VPhysicalVolume* Construct() override; // method by which we construct geometry
            // will be called by geant runmanager
            // telling compiler this method is overriding virtual method 
            // with specific implementation of Construct() provided in DetectorConstruction.cc

            // NOTE: override keyword acts as safety so know the default method is actually being replaced
            // otherwise, if there was a typo for example, compilation would still occur but with a new name
            // which wouldn't be linked to rest of code and cause other errors - harder to spot

            // define method to return pointer to scoring volume
            // used in eventaction to assign energy deposition etc. 
            // to correct volume within geom
            G4LogicalVolume* GetScoringVolume() const 
            {
                return fScoringVolume; 
            }

            // initialise another member function 
            // for assigning the sensitive detector
            void ConstructSDandField() override;

            // include member functions to get the exact positions
            // of detectors for output and post-processing
            G4ThreeVector GetDet0Pos() const {return fDet0Pos;}
            G4ThreeVector GetDet1Pos() const {return fDet1Pos;}

        protected:
            // ensure this can only be accessed by this class and not from outside
            // don't want scoring volume itself to be altered from outside 
            G4LogicalVolume* fScoringVolume = nullptr; 
            // pointer to logical volume being used for scoring

        private:
            // definition of the detector position member variables
            // keep in private so no other classes can alter them externally
            G4ThreeVector fDet0Pos;
            G4ThreeVector fDet1Pos;

    };

}

#endif