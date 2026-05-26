# AmBe Stack Simulation 

Geant4 simulation for an AmBe source and C6D6 scintillator within a graphite stack

## Prerequisites

Before building, need to ensure local Geant variables and data are sourced. 

Run the following:

```bash
source /path/to/your/geant4-install/bin/geant4.sh
```
## Compilation
To compile using cmake, follow these steps:

Navigate to the root project directory:
```bash
cd AmBeStack
```
Create and enter a separate build directory:
```bash
mkdir build && cd build
```
Configure the project with CMake:
```bash
cmake ..
```
Compile the source code against N threads:
```bash
make -jN
```

## Running the simulation

Once compiled. can execute the simulation binary file inside the build directory.

If the Geant4 installation supports interactive mode, may launch the executable directly:
```bash
./AmBeStack
```

Otherwise, run in batch mode with a macro file containing the configuration for the simulation:
```bash
./AmBeStack run.mac
```

## Output data
The output of the simulation will be contained in a thread-merged .root file containing an Ntuple structure called AmBeStackData for post-processing.

