#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <time.h>

// Maximum Iterations
// The maximum number of iterations to perform for each pixel before deciding:
    // If the pixel "escaped" (not part of the Mandelbrot set) — color it based on how fast it escaped
    // If the pixel "didn't escape" — assume it's part of the Mandelbrot set (black)
#define MAX_ITER 1000

typedef struct {
    int thread_id;
    int num_threads;
    int size;
    double min_real, max_real, min_imaginary, max_imaginary;
    Uint32 *pixels;
} thread_data_t;

// Defines the color of a pixel based on the number of iterations
Uint32 get_color(int iter) {
    if (iter == MAX_ITER) 
        return SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888), 0, 0, 0);
    
    Uint8 r = iter % 256;
    Uint8 g = (iter * 5) % 256;
    Uint8 b = (iter * 13) % 256;
    return SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888), r, g, b);
}

// Each thread computes part of the Mandelbrot Fractal
void *mandelbrot_thread(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    for (int y = data->thread_id; y < data->size; y += data->num_threads) {
        for (int x = 0; x < data->size; x++) {
            double real = data->min_real + x * (data->max_real - data->min_real) / (data->size - 1);
            double imaginary = data->max_imaginary - y * (data->max_imaginary - data->min_imaginary) / (data->size - 1);

            double z_real = real, z_imaginary = imaginary;
            int iter;

            for (iter = 0; iter < MAX_ITER; iter++) {
                double z_re2 = z_real * z_real;
                double z_im2 = z_imaginary * z_imaginary;
                if (z_re2 + z_im2 > 4.0) break;
                double new_re = z_re2 - z_im2 + real;
                double new_im = 2.0 * z_real * z_imaginary + imaginary;
                z_real = new_re;
                z_imaginary = new_im;
            }

            data->pixels[y * data->size + x] = get_color(iter);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    // To measure how many time the program runs
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    if (argc != 7) {
        printf("Usage: %s <num_threads> <size> <min_real> <max_real> <min_imaginary> <max_imaginary>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int size = atoi(argv[2]);
    double min_real = atof(argv[3]);
    double max_real = atof(argv[4]);
    double min_imaginary = atof(argv[5]);
    double max_imaginary = atof(argv[6]);

    // Vizualization setup
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Mandelbrot Fractral",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          size, size, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             size, size);

    Uint32 *pixels = malloc(size * size * sizeof(Uint32));
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t *thread_data = malloc(num_threads * sizeof(thread_data_t));

    // Create the threads
    for (int i = 0; i < num_threads; i++) {
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

    // JOIN all the threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Execution time
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Execution Time: %.4f seconds\n\n", cpu_time_used);

    // Render the image
    SDL_UpdateTexture(texture, NULL, pixels, size * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Wait until the vizualization window is closed
    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = 1;
        }
        SDL_Delay(100);
    }

    free(pixels);
    free(threads);
    free(thread_data);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
