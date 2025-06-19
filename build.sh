FLAGS="-g -Wall -Werror"
LIBS="`pkg-config --cflags --libs freetype2`-lglfw -lGL -lGLEW -lm -ldl"
WARNINGS="-Wno-unused-variable -Wno-unused-result -Wno-unused-function -Wno-unused-but-set-variable"
DEFINES="-DENABLE_BINARY"

g++ $FLAGS $WARNINGS -o Tetris3D src/platform.cpp $LIBS
