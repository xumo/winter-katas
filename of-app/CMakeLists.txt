cmake_minimum_required(VERSION 3.31)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the location of the openFrameworks directory:
# Make it absolute:
get_filename_component(OF_DIRECTORY $ENV{OPENFRAMEWORKS_PATH} ABSOLUTE)

message("OF DIRECTORY: ${OF_DIRECTORY}")

# Declare your project
project(of-app)

# Include the main CMakeLists, this adds OF as a target and includes the helper macros
include(${OF_DIRECTORY}/cmake/of_macros.cmake)

# This adds the of source to the build, but excludes it from compilation.
# Very useful for debugging.
add_subdirectory(${OF_DIRECTORY} "of_build" EXCLUDE_FROM_ALL)

# This will find the openFrameworks library, headers and dependencies
find_package(openFrameworks REQUIRED)

# Add the source files that you want to compile with your project
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# This creates your app! Give it a name and pass it the list of sources
ofApp(${PROJECT_NAME} "${SOURCES}")

#that's it!
