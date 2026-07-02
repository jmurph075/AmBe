/// \file AmBe/AmBeMain.cc
/// \brief Main execution master file for the AmBeStack model 

// include initialiser and detector construction header files
#include "ActionInitialization.hh" 
#include "DetectorConstruction.hh"

// core geant headers
#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh" 
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

// physics list headers 
#include "G4HadronicProcessStore.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4SystemOfUnits.hh"

#include <iostream>
#include <sstream>

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

// function to read the csv file with the spectrum data
// into a macro file ready for the generalparticlesource
// returntype name(args)
// takes name of file with data and ui manager to interact 
// with geant and macro
void ReadSourceSpec(const std::string& filename, G4UImanager* uiManager)
{
    // try to open the file
    std::ifstream csvFile(filename);
    if (!csvFile.is_open())
    {
        G4cerr << "Error: could not open the data file: " << filename << G4endl;
        return;
    }

    // read the header line and ignore it
    std::string line;
    // getline reads a line from the file
    // then its internal pointer moves to the next line
    // so after this statement, line is the header
    // and the next call of getline will be the first line of data
    if (std::getline(csvFile, line))
    {
        G4cout << "Skipping header line: " << line << G4endl;
    }

    // read the data lines
    // loops over each line for the full length of the file
    while (std::getline(csvFile, line))
    {
        // if reach end, break out of loop
        if (line.empty()) continue;

        // split the line into energy and emission rate
        // using stringstream to parse the line
        // allows use to read line as if 
        // it was a stream of data
        // (like cin and cout)
        std::stringstream ss(line);
        // initialise vars to hold energy and rate
        std::string energy_str, rate_str;
        
        // split by comma delimiter
        // if can get the 'lines' energy_str and rate_str,
        // can proceed and add them to the macro
        if (std::getline(ss, energy_str, ',') &&
            std::getline(ss, rate_str)) // no trailing comma, so just go to line end
        {
            // construct the command to pass to apply
            // using the UI manager
            std::string command = "/gps/hist/point " + energy_str + " " + rate_str;
            uiManager->ApplyCommand(command);
        }
    }

    // close the csv file
    csvFile.close();
    G4cout << "--> Loaded in the source spectrum data from: " << filename << G4endl; 
}

// gemini suggestion for sorting energy bins

void GemReadSourceSpec(const std::string& filename, G4UImanager* uiManager)
{
    // try to open the file
    std::ifstream csvFile(filename);
    if (!csvFile.is_open())
    {
        G4cerr << "Error: could not open the data file: " << filename << G4endl;
        return;
    }

    
    std::vector<double> energies;
    std::vector<double> weights;
    std::string line;

    while (std::getline(csvFile, line))
    {
        if (line.empty()) continue;

        // Strip trailing carriage returns (\r) from Windows files
        if (line.back() == '\r') 
        {
            line.pop_back();
        }

        std::stringstream ss(line);
        std::string energy_str, rate_str;
    
        if (std::getline(ss, energy_str, ',') && std::getline(ss, rate_str))
        {
            try 
            {
                energies.push_back(std::stod(energy_str));
                weights.push_back(std::stod(rate_str));
            }
            catch (const std::invalid_argument& e) 
            {
                // Safely skips header rows like "E_i [MeV], Emission_rate..."
                continue; 
            }
        }
    }

    // Translate the Lower-Bound layout into Geant4's Upper-Bound layout
    if (!energies.empty() && energies.size() == weights.size()) 
    {
        // 1. Establish the absolute lower limit of the spectrum using E_0
        std::string base_command = "/gps/hist/point " + std::to_string(energies[0]) + " 0";
        uiManager->ApplyCommand(base_command);

        // 2. Loop through and pair the upper edge (i+1) with the correct bin weight (i)
        for (size_t i = 0; i < energies.size() - 1; ++i) 
        {
            double upper_edge = energies[i+1];
            double bin_weight = weights[i];
        
            std::string command = "/gps/hist/point " + std::to_string(upper_edge) + " " + std::to_string(bin_weight);
            uiManager->ApplyCommand(command);
        }
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

    // first need to check what type of run - batch or interactive
    // if macro file supplied, in batch
    
    // instantiate UIExecutive pointer 
    G4UIExecutive* ui = nullptr;
    // instantiate runmanager type variable
    G4RunManagerType runManagerType = G4RunManagerType::Default;

    if (argc == 1)
    {
        G4cout << "No macro file provided, attempting to start interactive session..." << G4endl;
        // instantiate a UI executive to start the interactive session
        ui = new G4UIExecutive(argc, argv);
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

    // use type decided by run mode
    auto runManager = G4RunManagerFactory::CreateRunManager(runManagerType);

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

    // get a UIManager instance to issue commands to geant
    // using command line interface i.e. the macro file
    auto UImanager = G4UImanager::GetUIpointer();

    // need to configure the general particle source settings
    UImanager->ApplyCommand("/gps/particle neutron");
    UImanager->ApplyCommand("/gps/pos/type Point");
    UImanager->ApplyCommand("/gps/pos/centre 0 0 50 cm");
    //UImanager->ApplyCommand("/gps/ang/type beam1d");
    // UImanager->ApplyCommand("/gps/ang/type direction");
    UImanager->ApplyCommand("/gps/direction 0 0 1");
    // now specify that the energy spectra data type is user defined
    UImanager->ApplyCommand("/gps/ene/type User");
    // specify the type of user defined histogram
    // in this case is energy spec, so type is energy
    UImanager->ApplyCommand("/gps/hist/type energy");

    // now use the ReadSourceSpec function to read in the
    // data for the histogram
    // since compilation copied over the csv,
    // can just give file name rather than path
    //ReadSourceSpec("ISO_2021_small.csv", UImanager);
    GemReadSourceSpec("ISO_2021_small.csv", UImanager);
    // if a macro file (running in batch mode) supplied, execute it
    if (argc > 1)
    {
        // actually read and execute the instructions 
        // provided by the slurm script
        // i.e. read the macro file supplied
        G4String command = "/control/execute ";
        G4String fileName = argv[1]; // our macro
        // temp. monoenergetic source option
        UImanager->ApplyCommand("/gps/ene/type Mono");
        UImanager->ApplyCommand("/gps/ene/mono 10 MeV");
        //UImanager->ApplyCommand("/gps/energy 10");
        // apply the command
        UImanager->ApplyCommand(command + fileName);
    }
    
    // otherwise we are in interactive mode so start a UI session
    else
    {
        G4cout << "No macro file provided, attemptiong to start interactive session..." << G4endl;
        // start interactive session
        // initialise visualisation and UI executive
        G4VisManager* visManager = new G4VisExecutive(argc, argv);
        visManager->Initialize();
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();

    }

    // clean up
    // deletes things across the code to free up resources
    delete runManager;
    // return success code (0)
    return 0;

}