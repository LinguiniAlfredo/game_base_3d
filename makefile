CC = g++
OPTIONS = -Wall -g -pedantic -std=c++20
LINKER_FLAGS = -lSDL2 -lGL -lGLEW -lm
EXE = game

SRC_DIRS = ./ shaders shapes
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
