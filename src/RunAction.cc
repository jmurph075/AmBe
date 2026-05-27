/// \file AmBe/src/RunAction.cc
/// \brief Implementation of the AmBeStack::RunAction class

#include "RunAction.hh"
#include "EventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4Threading.hh"

namespace AmBeStack
{
    // start with the constructor
    // no argument this time as runaction is at uppermost level
    // of step->event->run hierarchy.
    RunAction::RunAction() 
        // : starts "member intialiser list"
        // zone which runs before the code inside {} 
        : G4UserRunAction() // - parent setup (from geant class)
        {
            // make an analysis manager instance
            G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

            // make some choices for the file output
            analysisManager->SetVerboseLevel(1);
            //if (!G4Threading::IsWorkerThread())  // master only
            //{
            //analysisManager->SetNtupleMerging(true);
            //analysisManager->SetDefaultFileType("root");
            //}
            // multi-threading safe merging
            // only works with ROOT file output apparently
            analysisManager->SetNtupleMerging(true);
            // does by default but will be explicit here
            analysisManager->SetDefaultFileType("root");

            // make the Ntuple with the structure we need
            analysisManager->CreateNtuple("AmBeStackData", "Energy deposition data");
            // define the columns for the Ntuple
            analysisManager->CreateNtupleIColumn("EventID");
            analysisManager->CreateNtupleDColumn("TotalEdep");
            analysisManager->CreateNtupleSColumn("RecoilName");
            analysisManager->CreateNtupleDColumn("RecoilEdep");

            analysisManager->FinishNtuple(); // finish ntuple

        }

    // first member function
    // actually define the BeginOfRunAction function
    // returntype class::functionname(args)
    // called at start of each run of simulation
    // acts as a hard RESET
    void RunAction::BeginOfRunAction(const G4Run* run)
    {
                // !!!!!!!!!!!!! for debugging !!!!!!!!!!
        //G4cout << "Coming from BeginOfRunAction" << G4endl;

        // analysis manager instance
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        // open output file for the run
        G4String fileName = "AmBeStackOutput.root";
        analysisManager->OpenFile(fileName);
    }

    // second member function 
    // actually define the EndOfRunAction function
    // returntype class::functionname(args)
    // called at end of each run of simulation
    void RunAction::EndOfRunAction(const G4Run* run)
    {
        // analysis manager instance
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        // save the rows from the memory into the file we've specified
        // at start of run
        analysisManager->Write();

        // close file after done so no hanging or overwriting
        analysisManager->CloseFile(); 

    }
}