#ifndef FRACTAL_MPI_H
#define FRACTAL_MPI_H

#include <cstdint>

#define WIDTH 1600
#define HEIGHT 900

// se necesita la imagen real e imaginaria
// cada rank debe sar donde empieza y donde fianliza

void julia_mpi(double x_min, double y_min, double x_max, double y_max,
               uint32_t width, uint32_t height,
               uint32_t row_start, uint32_t row_end,
               uint32_t* pixel_buffer);
#endif