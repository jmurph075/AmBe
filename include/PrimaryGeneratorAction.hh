// ********************************************************************
//
//
/// \file AmBe/src/PrimaryGeneratorAction.hh
/// \brief Definition of the AmBeStack::PrimaryGeneratorAction class

#ifndef AmBeStackPrimaryGeneratorAction_h 
#define AmBeStackPrimaryGeneratorAction_h 1
// included to guard from multiple inclusions of header file at compilation

#include "G4VUserPrimaryGeneratorAction.hh"

// forward declarations of some default classes
// we'll include full definitions in .cc
class G4GeneralParticleSource;
class G4Event;

namespace AmBeStack
{
    class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
    {
        public:
            // telling Geant we have something 
            // which follows the same rules as the 
            // virtual (V) PrimaryGeneratorAction class
            // the details of which will be in the
            // associated .cc file
            PrimaryGeneratorAction();
            ~PrimaryGeneratorAction() override;

            // we're going to have a method
            // to generate the primaries of each event
            // which follows the same rules again as in the 
            // virtual PrimaryGeneratorAction class
            void GeneratePrimaries(G4Event*) override;

            // no longer need a method to access the 
            // general particle source like for the gun

        private:
            // define general particle source through pointer
            // this is a member variable
            // should be defined exclusively in the header file
            // exists as long as the object (the specific instance of the class) lives
            // runmanager keeps primarygeneratoraction object alive for whole run
            // so this means anything stored in it is accessible
            G4GeneralParticleSource* fGPS = nullptr;
            
    };

}

#endif
