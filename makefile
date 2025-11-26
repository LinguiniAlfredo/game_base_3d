CC = g++
WIN_CC = x86_64-w64-mingw32-g++
OPTIONS = -Wall -g -pedantic -std=c++20
LINKER_FLAGS = -lSDL2 -lGL -lGLEW -lm -lassimp
MINGW_LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32 -lm -mwindows
EXE = game

SRC_DIRS = ./ shaders shapes entities
SRCS = $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))

BUILD_DIR = ./build
OBJ_FILES = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

$(shell mkdir -p $(BUILD_DIR) $(addprefix $(BUILD_DIR)/, $(SRC_DIRS)))

all : $(OBJ_FILES)
	$(CC) $(OPTIONS) $(OBJ_FILES) $(LINKER_FLAGS) -o $(EXE)

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(OPTIONS) -c $< -o $@

clean :
	rm -r $(BUILD_DIR)
	rm -f $(EXE)

count :
	find . -type f -not -path "./.git/*" -not -path "./resources/*" -print0 | xargs -0 wc -l
