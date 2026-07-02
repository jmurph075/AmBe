/// \file AmBe/src/DetectorHit.cc
/// \brief Implementation of the AmBeStack::DetectorHit class

#include "DetectorHit.hh"

namespace AmBeStack
{
    DetectorHit::DetectorHit()
        // : starts "member intialiser list"
        // zone which runs before the code inside {} 
        : G4VHit(),  // - parent setup (from virtual geant class)
        // assign member variables (defined in .hh file)
        fEdep(0.), fTime(0.), fCopyNo(-1), 
        fTrackID(-1), fParticleName(""), fProcessName(""),
        fIncidentEnergy(-1.0)
        {}

    // don't need to define destrcutor again here
    // since set it to default

}

