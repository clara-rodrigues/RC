# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra

# Root directories for player and server files
PLAYER_SRC_DIR = player
SERVER_SRC_DIR = server

# Find all .cpp files in the player and server directories
PLAYER_SRC_FILES = $(shell find $(PLAYER_SRC_DIR) -name '*.cpp')
SERVER_SRC_FILES = $(shell find $(SERVER_SRC_DIR) -name '*.cpp')

# Object files for the player1 executable
PLAYER_OBJ_FILES1 = $(patsubst %.cpp, %.player1.o, $(PLAYER_SRC_FILES))

# Object files for the GS executable
SERVER_OBJ_FILES = $(SERVER_SRC_FILES:.cpp=.o)

# Output executable names
PLAYER_OUTPUT1 = player1
SERVER_OUTPUT = GS

# Default target: build both executables
all: $(PLAYER_OUTPUT1) $(SERVER_OUTPUT)

# Rule to create the player1 executable
$(PLAYER_OUTPUT1): $(PLAYER_OBJ_FILES1)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to create the GS executable
$(SERVER_OUTPUT): $(SERVER_OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile .cpp files into .player1.o files for the player1 executable
%.player1.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile .cpp files into .o files for the GS executable
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(PLAYER_OBJ_FILES1) $(SERVER_OBJ_FILES) $(PLAYER_OUTPUT1) $(SERVER_OUTPUT)
