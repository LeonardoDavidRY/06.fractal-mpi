cmake_minimum_required(VERSION 3.16)
 
project(FractalMPI)
 
find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS System Graphics Window CONFIG REQUIRED)

# config MPI
set(MPI_ROOT "c:/oneAPI/mpi/latest")
set(MPI_INCLUDE "${MPI_ROOT}/include")
set(MPI_LIB "${MPI_ROOT}/lib")
 
include_directories(
    ${MPI_INCLUDE}
)
 
add_executable(fractal-mpi 
main.cpp
fractal_mpi.cpp
palette.cpp
)
 
target_link_libraries(fractal-mpi PRIVATE 
fmt::fmt
SFML::System 
SFML::Graphics 
SFML::Window
${MPI_LIB}/impi.lib
)