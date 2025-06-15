# 🎨 Mandelbrot Fractal Visualizer

> **Developed for the Bachelor's Degree in Computer Science**  
> **University:** Universidade do Vale do Rio dos Sinos (Unisinos)  
> **Subject:** High Performance Computing  
> **Semester:** 2025/01  
> **Students:** Guilherme Poleto, João Accorsi and Rafael Klauck  

---
## 🎯 Objective

The goal of this project is to create a parallelized Mandelbrot Set visualizer using the C programming language with pthreads for multithreading and SDL2 for graphical display.

The application computes and renders the Mandelbrot fractal onto the screen, allowing the user to configure key parameters:

- **Number of threads** used for computation (to explore parallel processing performance)
- **Image resolution (size)** to control display size and fractal detail
- **Complex plane boundaries** (`min_real`, `max_real`, `min_imaginary`, `max_imaginary`) to zoom and explore the Mandelbrot set

---
## 🚀 How to Run

### ✅ Prerequisites

Make sure the following are installed on your system:

- **GCC** (or any compatible C compiler)
- **SDL2 library**
- **pthread library** 

### ✅ Build
To build the project, please run:

```
make clean
make
```

### ✅ Run:
To run it, consider the below command line:

```
./mandelbrot <num_threads> <size> <min_real> <max_real> <min_imaginary> <max_imaginary>
```
The inputs mean:
| Parameter         | Description                                   |
|-------------------|-----------------------------------------------|
| `<num_threads>`    | Number of threads to execute the Mandelbrot Fractal |
| `<size>`          | Square size of the generated image with the Mandelbrot Fractal |
| `<min_real>`      | Minimal range of the real axis                 |
| `<max_real>`      | Maximum range of the real axis                  |
| `<min_imaginary>` | Minimal range of the imaginary axis            |
| `<max_imaginary>` | Maximum range of the imaginary axis            |

Two examples of the input would be:
```
./mandelbrot 10 400 -2.0 1.0 -1.0 1.0
./mandelbrot 10 400 -0.7 -0.5 0.0 0.2
```

---
## 📈 Output

![image](https://github.com/user-attachments/assets/c9f140a6-bb32-4278-9d97-9ba86ddd4930)




