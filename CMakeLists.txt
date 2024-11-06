cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(LAB4GC VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)  
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the build type if it's not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Profiling with gprof (uncomment if profiling is needed)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/headers)

# Find all source files in the src directory
file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Add the executable based on the source files
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Finding and linking SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Since GLM is usually header-only, find_package might not be necessary, adjust based on your setup
# find_package(glm REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/extern/glm) # Adjust this path as necessary

# Link libraries to the executable
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Uncomment and use if additional libraries such as SDL2_image are needed
# find_package(SDL2_image REQUIRED)
# include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} SDL2_image)

# If using TBB or other threading libraries, link them as well
# find_package(TBB REQUIRED)
# target_link_libraries(${PROJECT_NAME} TBB::tbb)
