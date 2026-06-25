/// \file AmBe/include/DetectorHit.hh
/// \brief Definition of the AmBeStack::DetectorHit class

#ifndef AmBeStackDetectorHit_h
#define AmBeStackDetectorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "globals.hh"

namespace AmBeStack
{
    class DetectorHit : public G4VHit
    {
        public: 
            // telling Geant we have something 
            // which follows the same rules as the 
            // G4VHit class
            // the details of which will be in the
            // associated .cc file
            // constructor needs no argument
            // we'll initialise members in the associated .cc file
            // just like in the other classes.
            DetectorHit();
            // destructor doesn't either
            // G4VHit is a virtual class,
            // so when we destruct, we specify it in the same way
            // makes sure entire object is wiped at the end of the event
            ~DetectorHit() override = default;

            // member functions
            // consists of 'setters' and 'getters'
            // should contain all the functions needed to capture
            // recoil nuclei physics 
            // lots of which contained previously within 
            // steppingaction

            // SETTERS (used by ScintillatorSD to save data)
            // 1. edepStep
            void SetEdep(G4double edep) {fEdep = edep;}
            // 2. time
            void SetTime(G4double time) {fTime = time;}
            // 3. copy number (which detector)
            void SetCopyNo(G4int copyNo) {fCopyNo = copyNo;}
            // 4. trackID
            void SetTrackID(G4int trackID) {fTrackID = trackID;}
            // 5. particleName
            void SetParticleName(G4String particleName) {fParticleName = particleName;}
            // 6. isRecoil
            void SetIsRecoil(G4bool isRecoil) {fIsRecoil = isRecoil;}

            // GETTERS (used by EventAction to read data to Ntuple)
            // 1. edepStep
            G4double GetEdep() const {return fEdep;}
            // 2. time
            G4double GetTime() const {return fTime;}
            // 3. copy number (which detector)
            G4int GetCopyNo() const {return fCopyNo;}
            // 4. trackID
            G4int GetTrackID() const {return fTrackID;}
            // 5. particleName
            G4String GetParticleName() const {return fParticleName;}
            // 6. isRecoil
            G4bool GetIsRecoil() const {return fIsRecoil;}

            // define each of these f (member) variables in private
            private:
                G4double fEdep; 
                G4double fTime;
                G4int fCopyNo;
                G4int fTrackID;
                G4String fParticleName;
                G4bool fIsRecoil;

    };

    // make a type definition (alias) for this custom DetectorHit class
    // means can just call DetectorHitsCollection in ScintillatorSD or EventAction
    // to mean this class
    typedef G4THitsCollection<DetectorHit> DetectorHitsCollection;
}

#endif