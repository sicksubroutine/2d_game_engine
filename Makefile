#######################################################################
CC = g++
LANG = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -I"./libs/"
SRC_FILES = ./src/*.cpp \
 			./src/game/*.cpp \
			./src/logger/*.cpp \
			./src/ecs/*.cpp \
			./src/asset_store/*.cpp
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.4
OBJECT_NAME = game_engine
#######################################################################
build:
	$(CC) $(COMPILER_FLAGS) $(LANG) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FLAGS) -o $(OBJECT_NAME)

run:
	./$(OBJECT_NAME)

clean:
	rm $(OBJECT_NAME)