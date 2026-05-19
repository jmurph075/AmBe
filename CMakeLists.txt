# 1. Setup the project name and CMake version baseline
cmake_minimum_required(VERSION 3.16...3.31)
project(AmBeStack)

# 2. Find Geant and ROOT packages loaded in job file
find_package(Geant4 REQUIRED)

# 3. Locate custom src and header files
file(GLOB sources ./src/*.cc)
file(GLOB headers ./include/*.hh)

# 4. Construct the executable from linking the sources and headers
add_executable(AmBeStack AmBeSimulation.cc 
    ${sources} 
    ${headers}
)

# 5. Tell compiler to use custom header files defined
target_include_directories(AmBeSimulation PRIVATE include)

# 6. Link the simulation to the Geant libraries
target_link_libraries(AmBeSimulation PRIVATE
    ${Geant4_LIBRARIES}
)

# 7. Copy over macro script to build directory
# since Geant needs these to be in current working directory
set(AMBESTACK_SCRIPTS
    run.mac
)

# for each script in AMBESTACK_SCRIPTS (listed above)
foreach(_script ${AMBESTACK_SCRIPTS})
    configure_file(
        ${PROJECT_SOURCE_DIR}/${_script}
        ${PROJECT_BINARY_DIR}/${_script}
        COPYONLY
    )
endforeach()