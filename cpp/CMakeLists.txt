cmake_minimum_required(VERSION 3.10)

project(fisheye_simulation)

set(CMAKE_CXX_STANDARD 11)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Add the include directories
include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})
message(${OpenCV_INCLUDE_DIRS})

# Add the source files to the project
add_executable(${PROJECT_NAME} src/main.cpp)

# Link against OpenCV
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Set the output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")