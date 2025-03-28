CC = g++
CFLAGS = -Isrc/include/SDL2
LDFLAGS = -Lsrc/include/SDL2 -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
SOURCES = main.cpp config.cpp graphics.cpp player.cpp enemy.cpp game.cpp
TARGET = main.exe

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS) -Wl,-rpath,src/include/SDL2

clean:
	del $(TARGET)
