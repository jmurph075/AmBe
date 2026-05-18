/// \file AmBe/include/RunAction.hh
/// \brief Definition of the AmBeStack::RunAction class

#ifndef AmBeStackRunAction_h
#define AmBeStackRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4Run.hh"

namespace AmBeStack
{
    // define userunaction class
    class RunAction : public G4UserRunAction
    {
        public:
            // telling Geant we have something 
            // which follows the same rules as the 
            // G4UserRunAction class
            // the details of which will be in the
            // associated .cc file
            RunAction();
            ~RunAction() override = default;

            // first member function
            void BeginOfRunAction(const G4Run* run) override;

            // second member function
            void EndOfRunAction(const G4Run* run) override;

    };
}

#endif