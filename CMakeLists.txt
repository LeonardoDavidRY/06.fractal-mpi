cmake_minimum_required(VERSION 3.20)

project(FractalMPI)


find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

#---config MPI
set(MPI_ROOT    "c:/oneAPI/mpi/latest")
set(MPI_INCLUDE "${MPI_ROOT}/include")
set(MPI_LIB     "${MPI_ROOT}/lib")

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
 ${MPI_LIB}/impi.lib
 SFML::Network SFML::Graphics SFML::Window SFML::Audio SFML::System
) 
