#!/bin/bash
#SBATCH --time 00:10:00
#SBATCH --qos bbshort
#SBATCH --ntasks 56
set -e

module purge; module load bluebear
module load bear-apps/2022a
module load Geant4/11.1.2-GCC-11.3.0
module load Geant4-data/11.1
module load CMake/3.24.3-GCCcore-11.3.0

# check modules loaded
echo "Loaded Geant and CMake"

# remove previous build dir (if exists)
rm -r build
# make new one and move inside
mkdir build && cd build
# link this build dir to the project root dir
cmake ..
# specify to compile with no. of cpus
make -j${SLURM_CPUS_PER_TASK:-1} # if unset gives the default of 1

# see if get this far
echo "Compiled successfully