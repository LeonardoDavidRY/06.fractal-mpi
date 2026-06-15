#include <mpi.h>
#include <fmt/core.h>
#include <complex>
#include <iostream>
#include "fractal_mpi.h"
#include <SFML/Graphics.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;
int max_iteraciones = 10;

std::complex<double> c(-0.7, 0.27015);

uint32_t *pixel_buffer = nullptr;
uint32_t *texture_buffer = nullptr; // solo RANK_0

int running = 1;

//-- dimension imagen
#define WIDTH 1600
#define HEIGHT 900

void setup_ui()
{
    texture_buffer = new uint32_t[WIDTH * HEIGHT];
    std::memset(texture_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Julia Set - SFML");

#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif

    sf::Texture texture({WIDTH, HEIGHT});
    sf::Sprite sprite(texture);

    // textos

    // FPS
    int frames = 0;
    int fps = 0;
    sf::Clock clock;

    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
            {
                running = 0;
                window.close();
            }

            else if (event->is<sf::Event::KeyReleased>())
            {
                auto evt = event->getIf<sf::Event::KeyReleased>();

                switch (evt->scancode)
                {
                case sf::Keyboard::Scan::Up:
                    max_iteraciones += 10;
                    break;

                case sf::Keyboard::Scan::Down:
                    max_iteraciones -= 10;
                    if (max_iteraciones < 10)
                        max_iteraciones = 10;
                    {
                    }
                    break;
                default:
                    break;
                }
                std::memset(texture_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));
            }
        }

        // Notificar a los otros RANKS que la app se esta cerrando
        std::vector<int> dummy = {max_iteraciones, running};
        MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);
        if (running == 0)
        {
            break;
        }

        // Crear textura
        // texture.update((const uint8_t *)pixel_buffer);

        // contar fps
        frames++;
        if (clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frames;
            frames = 0;
            clock.restart();
        }

        // actualizar el titulo de la ventana con el fps
        // auto msg = fmt::format("Julia Set: Iterations: {}, FPS: {}, Mode: {}", max_iteraciones, fps, mode);
        // text.setString(msg);

        // Clear screen
        window.clear();
        {
            //  window.draw(sprite);
            //  window.draw(text);
            //  window.draw(textOptions);
        }

        // Update the window
        window.display();
    }
}

int main(int argc, char **argv)
{

    MPI_Init(&argc, &argv);

    int nprocs;
    int rank;

    // Ranks
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int delta = std::ceil((HEIGHT * 1.0) / nprocs); // 1600/4 = 400
    /**
     * r0: start = 0*400, end=0+400 = 400
     * r1: start = 1*400, end=400+400 = 800
     * r2: start = 2*400, end=800+400 = 1200
     * r3: start = 3*400, end=1200+400 = 1600
     */
    int row_start = rank * delta;
    int row_end = row_start + delta;
    int padding = delta * nprocs - HEIGHT;

    if (row_end > HEIGHT)
    {
        row_end = HEIGHT;
    }

    pixel_buffer = new uint32_t[WIDTH * delta];
    std::memset(pixel_buffer, 0, WIDTH * delta * sizeof(uint32_t));
    fmt::println("RANK_{}: rows {} to {}", rank, row_start, row_end);
    if (rank == 0)
    {
        setup_ui();
    }
    else
    {
        // dibujar

        while (true)
        {
            std::vector<int> dummy = {max_iteraciones, 0};
            MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);

            max_iteraciones = dummy[0];
            running = dummy[1];

            if (running == 0)
            {
                fmt::println("RANK_{}: received shutdown signal, exiting...", rank);
                break;
            }

            julia_mpi(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, row_start, row_end, pixel_buffer);
            if (rank == 1)
            {
                fmt::println("RANK_{}: max_iteraciones = {}", rank, max_iteraciones);
                std::cout.flush();
            }
        }
    }

    MPI_Finalize();
    return 0;
}