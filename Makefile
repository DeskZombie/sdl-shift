OBJS = main.cpp ../../LTimer.cpp ../../LTexture.cpp 

OBJ_NAME = hop 

LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -std=c++11

CC = g++ 

all:$(OBJS) 
	$(CC) $(OBJS) $(LINKER_FLAGS) -o $(OBJ_NAME) 
