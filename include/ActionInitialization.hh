/// \file AmBe/include/ActionInitialization.hh
/// \brief Definition of the AmBeStack::ActionInitialization class

#ifndef AmBeStackActionInitialization_h
#define AmBeStackActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

namespace AmBeStack
{
    class ActionInitialization : public G4VUserActionInitialization
    {
        public:
            // telling Geant we have something 
            // which follows the same rules as the 
            // virtual (V) ActionInitialization class
            // the details of which will be in the
            // associated .cc file
            ActionInitialization() = default;
            ~ActionInitialization() override = default;

            // member functions of our class
            void BuildForMaster() const override;
            void Build() const override;
    };
}

#endif