# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g  # Enable C++11, include all warnings, and debugging symbols
LDFLAGS =  # Additional linker flags if necessary

# Directories
SRC_DIR = UDP
OBJ_DIR = obj

# Source files
SRC_FILES = \
	player.cpp \
	$(SRC_DIR)/exit/exit.cpp \
	$(SRC_DIR)/quit/quit.cpp \
	$(SRC_DIR)/debug/debug.cpp \
	$(SRC_DIR)/start/start.cpp \
	$(SRC_DIR)/try/try.cpp \
	$(SRC_DIR)/UDP.cpp

# Object files
OBJ_FILES = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Output executable
EXEC = player1

# Default target
all: $(EXEC)

# Rule to create the executable
$(EXEC): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $(EXEC) $(LDFLAGS)

# Rule to compile .cpp files to .o
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)  # Create the necessary subdirectories in the obj directory
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

# Phony targets
.PHONY: all clean
