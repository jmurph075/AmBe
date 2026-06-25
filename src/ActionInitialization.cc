/// \file AmBe/src/ActionInitialization.cc
/// \brief Implementation of the AmBeStack::ActionInitialization class

// this links all the other physics-defining classes together
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"

#include "globals.hh"


namespace AmBeStack
{
    // make the member functions which work 
    // to organise the differewnt parts of the simulation

    // 1. BuildForMaster():
    // runs once on the MASTER thread (when multi-threading)
    // class::function
    void ActionInitialization::BuildForMaster() const
    {
                // !!!!!!!!!!!!! for debugging !!!!!!!!!!
        //G4cout << "Coming from BuildForMaster" << G4endl;

        // the master thread will just manage global data 
        // which we'll access through RunAction (accumulates total stats)
        SetUserAction(new RunAction);
    }

    // 2. Build():
    // runs for each WORKER thread 
    // class::function
    void ActionInitialization::Build() const
    {
        // the worker threads perform the physics 
        // as such they'll need access to all the different actions
        // in our simulation
        // (Primary->(Event->Stepping))->Run
                // !!!!!!!!!!!!! for debugging !!!!!!!!!!
        //G4cout << "Coming from Build" << G4endl;


        // register primary generator action (for the neutron gun physics) 
        SetUserAction(new PrimaryGeneratorAction);

        // register run action (for total accumulated stats start to finish)
        auto runAction = new RunAction;
        SetUserAction(runAction);

        // register event action (for access to each event (particle history))
        auto eventAction = new EventAction(runAction);
        SetUserAction(eventAction);

        // register sensitive detector class now 
        // instead of stepping action like before
        
    }
}
                
