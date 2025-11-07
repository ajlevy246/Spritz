# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude

# Directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include

# Output executable
TARGET := spritz

# Find all .cpp files recursively in src and the root directory
SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp) main.cpp

# Generate corresponding .o paths under build/
OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Default rule
all: $(TARGET)

# Link all objects
$(TARGET): $(OBJ_FILES)
	@echo "Linking..."
	$(CXX) $(OBJ_FILES) -o $@

# Compile each .cpp file into build/.../.o
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Run program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
