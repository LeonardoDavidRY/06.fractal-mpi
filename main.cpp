#include <iostream>
 
#include <complex>
#include <fmt/core.h>
#include <SFML/Graphics.hpp>
#include <mpi.h>
#include "fractal_mpi.h"
#include "arial_ttf.h"
 
#include "draw_text.h"
 
#ifdef _WIN32
    #include <windows.h>
#endif
 
namespace arial_ttf
{
    extern size_t data_len;
    extern unsigned char data[];
}
 
//parammetros julia
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;
 
int max_iteraciones = 10;
 
//dimension de la imagen
#define WIDTH 1600
#define HEIGHT 900

uint32_t* pixel_buffer = nullptr;
uint32_t* texture_buffer = nullptr; //solo RANK_0

int running = 1;

int nprocs;
int rank;

int row_start;
int row_end;
int padding;
int delta;

std::string machine_name() {
    //--machine name
    std::string mname = "";
#ifdef _WIN32
    char hostname[256];
    DWORD size = sizeof(hostname);
    GetComputerNameA(hostname, &size);
    mname = hostname;
#endif
    return mname;
}


void dibujar_texto(int rank, int h) {
    auto texto = fmt::format("Rank {} ==> {}", rank, machine_name());
    draw_text_to_texture (
        (unsigned char*) pixel_buffer,
        WIDTH, h,
        texto.c_str(),
        10, 25, 20
    );
}
 
void setup_ui()
{
    texture_buffer = new uint32_t[WIDTH * HEIGHT];
    std::memset(texture_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));
   
    // inicializar la ui
    //sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Fractal MPI");
#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
    sf::Texture texture({WIDTH, HEIGHT});
    sf::Sprite sprite(texture);
 
    // textos
    sf::Font font(arial_ttf::data, arial_ttf::data_len);
    sf::Text text(font, "Julia Set", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});
    text.setStyle(sf::Text::Bold);
 
    std::string options = "UP/DOWN -> Iterations" ;
    sf::Text textOptions(font, options, 20);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setStyle(sf::Text::Bold);
    textOptions.setPosition({10, window.getView().getSize().y - 40});
 
    // fps
    int frames = 0;
    int fps = 0;
    sf::Clock clock;
 
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            running=0;
           
        }
        else if (event->is<sf::Event::KeyReleased>()) {
            auto evt = event->getIf<sf::Event::KeyReleased>();
 
            switch(evt->scancode) {
                case sf::Keyboard::Scan::Up:
                    max_iteraciones += 10;
                    break;
                case sf::Keyboard::Scan::Down:
                    max_iteraciones -= 10;
                    if(max_iteraciones < 10) max_iteraciones = 10;
                    break;
            }
            std::memset(texture_buffer, 0, WIDTH*HEIGHT*sizeof(uint32_t));
        }
    }
        //notificar a los otros ransk que la app se esta cerrando
        std::vector<int> dummy = {max_iteraciones,running};
        MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);
 
        if (running==0)
        {
            break;
        }
 
        //dibujar la procion del rank 0
        julia_mpi(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, row_start, row_end, pixel_buffer);
 
        dibujar_texto(0, delta);
        //copiar el pixelbuffer a la textura
        std::memcpy(texture_buffer, pixel_buffer, WIDTH * delta * sizeof(uint32_t));
 
 
        //recibir las imagenes parciales de los otros procesos
        for (int i = 1; i < nprocs; i++)
        {
            int new_delta = delta;
            if (i == nprocs - 1)
            {
                new_delta = delta - padding;
            }
 
            MPI_Recv(pixel_buffer, WIDTH * new_delta, MPI_UNSIGNED,
            i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
 
            dibujar_texto(i, new_delta);
 
            std::memcpy(texture_buffer + i * delta * WIDTH, pixel_buffer, WIDTH * new_delta * sizeof(uint32_t));
        }
 
        // Dibuja Fractal dependera de la velocidad
        texture.update((const uint8_t *)texture_buffer);
 
        frames++;
 
        if (clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frames;
            frames = 0;
            clock.restart();
        }
        //Actualizar el titulo de la ventana con el FPS
        auto msg = fmt::format("julia: iteraciones; {}.fps{}", max_iteraciones, fps);
        text.setString(msg);
 
        // Clear screen
        window.clear();
        {
            window.draw(sprite);
            window.draw(text);
            window.draw(textOptions);
        }
 
        // Update the window
        window.display();
    }
}
 
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
 
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
    init_freetype();
    delta = std::ceil(HEIGHT * 1.0 / nprocs); //1600/4=400
 
    /*
    r0: start = 0*400 = 0, end = 0*400+400 = 400
    r1: start = 1*400 = 400, end = 1*400+400 = 800
    r2: start = 2*400 = 800, end = 2*400+400 = 1200
    r3: start = 3*400 = 1200, end = 3*400+400 = 1600
 
    */
   
    row_start = rank * delta;
    row_end = row_start + delta;
    padding = delta * nprocs - HEIGHT;
 
    if (row_end > HEIGHT)
    {
        row_end = HEIGHT;
    }
 
    pixel_buffer = new uint32_t[WIDTH * delta];
    std::memset(pixel_buffer, 0, WIDTH * delta * sizeof(uint32_t));
    fmt::print("Rank {}: rows {} to {}\n", rank, row_start, row_end);
 
    if (rank == 0)
    {
        setup_ui();
    }
    else
    {
        //dibujar
        while (true)
        {
            std::vector<int> dummy = {max_iteraciones,0};
            MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);
            max_iteraciones = dummy[0];
            running = dummy[1];
            if (running == 0)
            {
                fmt::print("Rank {}: received shutdown signal, exiting...\n", rank);
                break;
            }
            julia_mpi(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, row_start, row_end, pixel_buffer);
           
 
            //Enviar la porcion de la imagen 1600x225
            MPI_Send(
                pixel_buffer,
                WIDTH * (row_end - row_start),
                MPI_UNSIGNED,
                0, //rank destino
                0, //tag
                MPI_COMM_WORLD
            );
        }
    }
 
    MPI_Finalize();
    return 0;
}