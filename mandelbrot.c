#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <sys/time.h>

// Maximum Iterations
// The maximum number of iterations to perform for each pixel before deciding:
// If the pixel "escaped" (not part of the Mandelbrot set) — color it based on how fast it escaped
// If the pixel "didn't escape" — assume it's part of the Mandelbrot set (black)
#define MAX_ITER 1000

typedef struct
{
    int thread_id;
    int num_threads;
    int size;
    double min_real, max_real, min_imaginary, max_imaginary;
    Uint32 *pixels;
} thread_data_t;

double min_real, max_real, min_imaginary, max_imaginary;
int *thread_calls;

Uint32 get_color(int iter)
{
    SDL_PixelFormat *fmt = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
    if (iter == MAX_ITER)
        return SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888), 0, 0, 0);

    Uint8 r = iter % 256;
    Uint8 g = (iter * 5) % 256;
    Uint8 b = (iter * 13) % 256;
    return SDL_MapRGB(fmt, r, g, b);
}

void *mandelbrot_thread(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;

    printf("Thread %d: iniciando cálculo...\n", data->thread_id);
    fflush(stdout);
    __sync_fetch_and_add(&thread_calls[data->thread_id], 1);

    for (int y = data->thread_id; y < data->size; y += data->num_threads)
    {
        for (int x = 0; x < data->size; x++)
        {
            double real = data->min_real + x * (data->max_real - data->min_real) / (data->size - 1);
            double imaginary = data->max_imaginary - y * (data->max_imaginary - data->min_imaginary) / (data->size - 1);

            double z_real = real, z_imaginary = imaginary;
            int iter;
            for (iter = 0; iter < MAX_ITER; iter++)
            {
                double z_re2 = z_real * z_real;
                double z_im2 = z_imaginary * z_imaginary;
                if (z_re2 + z_im2 > 4.0)
                    break;
                double new_re = z_re2 - z_im2 + real;
                double new_im = 2.0 * z_real * z_imaginary + imaginary;
                z_real = new_re;
                z_imaginary = new_im;
            }

            data->pixels[y * data->size + x] = get_color(iter);
        }
    }

    printf("Thread %d: finalizou cálculo!\n", data->thread_id);
    fflush(stdout);
    return NULL;
}

void calculate_and_draw(int num_threads, int size, Uint32 *pixels)
{
    clock_t start = clock();
    struct timeval timeStart, timeEnd;
    gettimeofday(&timeStart, NULL);

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t *thread_data = malloc(num_threads * sizeof(thread_data_t));

    for (int i = 0; i < num_threads; i++)
    {
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_threads;
        thread_data[i].size = size;
        thread_data[i].min_real = min_real;
        thread_data[i].max_real = max_real;
        thread_data[i].min_imaginary = min_imaginary;
        thread_data[i].max_imaginary = max_imaginary;
        thread_data[i].pixels = pixels;
        pthread_create(&threads[i], NULL, mandelbrot_thread, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // CPU Time
    clock_t end = clock();
    printf("Tempo de CPU gasto: %.2f segundos\n", (double)(end - start) / CLOCKS_PER_SEC);

    // Real time
    gettimeofday(&timeEnd, NULL);
    double real_time_used = (timeEnd.tv_sec - timeStart.tv_sec) + (timeEnd.tv_usec - timeStart.tv_usec) / 1000000.0;
    printf("Tempo real de execução: %.4f segundos\n", real_time_used);

    fflush(stdout);

    for (int i = 0; i < num_threads; i++)
    {
        printf("Thread %d foi chamada %d vez(es)\n", i, thread_calls[i]);
    }
    fflush(stdout);

    free(threads);
    free(thread_data);
}

int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        printf("Usage: %s <num_threads> <size> <min_real> <max_real> <min_imaginary> <max_imaginary>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int size = atoi(argv[2]);
    min_real = atof(argv[3]);
    max_real = atof(argv[4]);
    min_imaginary = atof(argv[5]);
    max_imaginary = atof(argv[6]);

    thread_calls = malloc(num_threads * sizeof(int));
    for (int i = 0; i < num_threads; i++)
    {
        thread_calls[i] = 0;
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Mandelbrot Fractal com Zoom",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          size, size, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             size, size);
    Uint32 *pixels = malloc(size * size * sizeof(Uint32));

    calculate_and_draw(num_threads, size, pixels);
    SDL_UpdateTexture(texture, NULL, pixels, size * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    int quit = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = 1;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int x = event.button.x;
                int y = event.button.y;

                double clicked_real = min_real + x * (max_real - min_real) / (size - 1);
                double clicked_imag = max_imaginary - y * (max_imaginary - min_imaginary) / (size - 1);

                double zoom_factor = (event.button.button == SDL_BUTTON_LEFT) ? 0.5 : 2.0;

                double real_range = (max_real - min_real) * zoom_factor;
                double imag_range = (max_imaginary - min_imaginary) * zoom_factor;

                min_real = clicked_real - real_range / 2;
                max_real = clicked_real + real_range / 2;
                min_imaginary = clicked_imag - imag_range / 2;
                max_imaginary = clicked_imag + imag_range / 2;

                calculate_and_draw(num_threads, size, pixels);
                SDL_UpdateTexture(texture, NULL, pixels, size * sizeof(Uint32));
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }
        }
        SDL_Delay(16);
    }

    free(pixels);
    free(thread_calls);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
