/// \file AmBe/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the AmBeStack::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "globals.hh"

// for sampling from AmBe spectrum move to 
// general particle source
#include "G4GeneralParticleSource.hh"
#include "G4AnalysisManager.hh"

namespace AmBeStack
{

// constructor to set up source particle generation (i.e. AmBe source spectra)
// class::function/constructor 
// defining the code for the constructor that was 
// initialised in the header file
// like defining class functions in python with class.function
PrimaryGeneratorAction::PrimaryGeneratorAction()
    // : starts "member intialiser list"
    // zone which runs before the code inside {} 
    : G4VUserPrimaryGeneratorAction(), // - parent setup (run the basics __init__ of the native geant class first)
        fGPS(nullptr) // initialise general particle source
{
    // initialise particle source
    // notice "new"
    // makes the object "on the heap" (large memory pool)
    // means permanent  - lives until manually deleted
    // (see destructor definition below)
    fGPS = new G4GeneralParticleSource();

    // no other details are specified in the constructor
    // as for a gps, the details are provided 
    // in the macro file supplied
}

// destructor to clean up particle source when simulation has ran
// class::function/destructor here
PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fGPS; 
}

// function to be called at the start of each event
// this is used to place the particles generated
// class::
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    // !!!!!!!!!!!!! for debugging !!!!!!!!!!
    //G4cout << "Coming from GeneratePrimaries" << G4endl;

    // the generalparticlesource has its own
    // method for generating the primary vertex 
    // based on the details supplied by the macro
    fGPS->GeneratePrimaryVertex(event);

    // want to extract the primary particle energy 
    // and pass to the analysis manager
    // for storage in ntuple
    G4double primaryEnergy = fGPS->GetParticleEnergy();
    
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    // built as column 12 within our ntuple
    // so column index is 12 (ensure in MeV)
    analysisManager->FillNtupleDColumn(12, primaryEnergy / MeV);

}

}