#include <iostream>
#include <fmt/core.h>
#include <mpi.h>
#include "fractal_mpi.h"
#include "palette.h"
#include <complex>
#include <cstdint>
#include <SFML/Graphics.hpp>
#include "fractal_mpi.h"

#ifdef _WIN32
    #include <windows.h>
#endif

//-- parametros Julia
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

int max_iteraciones = 10;

std::complex<double> c(-0.7, 0.27015);

//dimension de la imagen

#define WIDTH 1600
#define HEIGHT 900

uint32_t *pixel_buffer = nullptr;
uint32_t *texture_buffer = nullptr; //solo rank 0

void setup_ui(){
    texture_buffer = new uint32_t[WIDTH*HEIGHT];
    std::memset(texture_buffer, 0, WIDTH*HEIGHT*sizeof(uint32_t));

    //inicializar UI
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Fractal MPI");
}
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int nprocs;
    int rank;

    //-ranks
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int delta = std::ceil(HEIGHT*1.0/nprocs); // 1600/4 = 400

    /**
     * r0: start = 0*400=0, end=0+400=400
     * r1: start = 1*400=400, end400+400=800
     * r2: start = 2*400=800, end=800+400=1200
     * r3: start = 3*400=1200, end=1200+400=1600
     */
    int row_start = rank * delta;
    int row_end = row_start + delta;
    int padding = delta * nprocs - HEIGHT; // 1600 - 1600 = 0

    if(row_end > HEIGHT) {
        row_end = HEIGHT;
    }

    pixel_buffer = new uint32_t[WIDTH*delta];
    std::memset(pixel_buffer, 0, WIDTH*delta*sizeof(uint32_t));

    fmt::print("Rank {}: rows {} - {}\n", rank, row_start, row_end);

    MPI_Finalize();

    return 0;
}