CC = gcc
CFLAGS = -Wall -I/opt/homebrew/opt/sdl2/include
LDFLAGS = -L/opt/homebrew/opt/sdl2/lib -lSDL2 -lpthread
TARGET = mandelbrot
SRC = mandelbrot.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)