#!/bin/bash
#SBATCH --time 00:10:00
#SBATCH --qos bbshort
#SBATCH --ntasks=1
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
echo "Compiled successfully"

# make a time-stamped output directory
#TIMESTAMP=$(date +"%Y_%m_%d__%H_%M_%S")
# still in build currently
#OUTDIR=$(realpath ../outputs/${TIMESTAMP}) # geant needs absolute path apparently
#mkdir -p "$OUTDIR"
# specify this output in the macro
#MACRO="./../run.mac" 
#sed "s|^/output/dir.*|/output/dir $OUTDIR|" "$MACRO" > "$OUTDIR/run1_temp.mac" 
# can do the same with the particle type and energy
#PARTICLE="neutron"
#sed -i "s|^/gun/particle.*|/gun/particle $PARTICLE|" "$OUTDIR/run1_temp.mac"  #-i edits the file in place
#ENERGY="5 MeV"
#sed -i "s|^/gun/energy.*|/gun/energy $ENERGY|" "$OUTDIR/run1_temp.mac"
# no. of events
#TOT_EVENTS="10"
#sed -i "s|^/run/beamOn.*|/run/beamOn $TOT_EVENTS|" "$OUTDIR/run1_temp.mac"

#echo "Successfully modified macro file contents"

# execute and provide macro file to Geant
./AmBeStack "run.mac"
# see if we got here
echo "Executed sim with macro"

# copy the slurm output files to the relevant output dir
# need to leave build directory
#cd ..
## Copy SLURM output and stats files into the run output directory
#cp "slurm-${SLURM_JOB_ID}.out" "$OUTDIR/job_${ENERGY// /_}_$PARTICLE.out" 2>/dev/null || true # can rename with the energy of the incident neutrons
#cp "slurm-${SLURM_JOB_ID}.stats" "$OUTDIR/job_${ENERGY// /_}_$PARTICLE.stats" 2>/dev/null || true # last part silences errors if the file doesn't exist (if the sim failed to run)
## see if this happened ok
#echo "Logs copied to $OUTDIR"