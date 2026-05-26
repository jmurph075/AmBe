/// \file AmBe/AmBeMain.cc
/// \brief Main execution master file for the AmBeStack model 

// include initialiser and detector construction header files
#include "ActionInitialization.hh" 
#include "DetectorConstruction.hh"

// core geant headers
#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh" 
#include "G4UImanager.hh"

// physics list headers 
#include "G4HadronicProcessStore.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4SystemOfUnits.hh"

#include <iostream>

// declare namespace being used
using namespace AmBeStack;

// make diagnostic function to check physics loaded correctly
void CheckHadronicData()
{
    // make hadronic process store instance
    // to see what processes are loaded in the model
    G4HadronicProcessStore* store = G4HadronicProcessStore::Instance();
    store->SetVerbose(1);

    // print out which databases and processes are loaded
    // and active in our model
    G4cout << "\n===== Checking Hadronic Process Store =====" << G4endl;
    store->Dump(1);
    G4cout << "\n=========================================\n" << G4endl;
}

// function to show the available physics lists 
// for this version of Geant on BlueBear
void PrintAvailablePhysLists()
{
    // get an instance of the physics list factory 
    // not a 'singleton' like other classes where we do ::Instance()
    // instead call it like a normal variable
    G4PhysListFactory factory;
    // since we instantiated as object rather than pointer
    // use . rather than ->
    std::vector<G4String> availableLists = factory.AvailablePhysLists();

    // print out available physics lists
    G4cout << "\n=== Available physics lists on BB ===" << G4endl;
    for (const auto& name: availableLists)
    {
        G4cout << "  " << name << G4endl;
    }
}

// main simulation function 
// links things between the computational resources, 
// macro, and model files
// argc is argument count - tot no. of args passed to the program
// e.g. ./AmBeStack run.mac is 2 args
// argv is argument vector - array of strings containing 
// actual text of the arguments
// argv[0] = "./AmBeStack" and argv[1] = "run.mac"
int main(int argc, char** argv)
{
    // first ensure a macro file was supplied
    // since on BB need a macro file to interface and instruct
    // the simulation to run
    if (argc < 2)
    {
        G4cerr << "Error: No macro file provided" << G4endl;
        return 1;
    }

    // print the available physics lists to the terminal
    // using the function we made above
    PrintAvailablePhysLists();

    // make choice on precision of run terminal output
    // for debugging purposes only
    G4int precision = 4;
    G4SteppingVerbose::UseBestUnit(precision);

    // define RunManager to register 
    // components of the simulation
    // Geant then knows how set things up correctly
    // and loop over things globally (over all events)
    auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    // 1. register the detector construction
    // do this through SetUserInitialization 
    runManager->SetUserInitialization(new DetectorConstruction());

    // 2. register the physics list to actually use
    // (must be one of those giuven by PrintAvailablePhysLists function)
    // make a factory object instance again
    G4PhysListFactory factory;
    // grab the physics list we want with high-precision neutron interactions
    G4VModularPhysicsList* physicsList = factory.GetReferencePhysList("QGSP_BIC_HP");
    // set verbosity to low
    physicsList->SetVerboseLevel(0);
    // register through SetUserInitialization
    runManager->SetUserInitialization(physicsList);

    // 3. register the data processing
    // defined in the action files
    runManager->SetUserInitialization(new ActionInitialization());

    // initialise runManager now that things are registered
    runManager->Initialize();

    // use CheckHadronicData function to verify data
    // we've registered
    CheckHadronicData();

    // actually read and execute the instructions 
    // provided by the slurm script
    // i.e. read the macro file supplied
    // need a UI manager instance
    // allows us to issue commands to Geant4 using
    // command line interface (supplied in macro)
    auto UImanager = G4UImanager::GetUIpointer();
    G4String command = "/control/execute ";
    G4String fileName = argv[1]; // our macro
    // apply the command
    UImanager->ApplyCommand(command + fileName);

    // clean up
    // deletes things across the code to free up resources
    delete runManager;
    // return success code (0)
    return 0;

}