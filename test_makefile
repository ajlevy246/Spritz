# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O3 -march=native -Iinclude 
LDFLAGS := -pthread

# Profiling flags
PROFILE_CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -g -Iinclude
PROFILE_LDFLAGS := -pthread -pg

# Directories
SRC_DIR := src
BUILD_DIR := build
PROFILE_BUILD_DIR := build_profile
INCLUDE_DIR := include

# Output executable
TARGET := spritz
PROFILE_TARGET := spritz_profile

# Find all .cpp files recursively in src and the root directory
SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp) main.cpp

# Generate corresponding .o paths under build/
OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
PROFILE_OBJ_FILES := $(patsubst %.cpp, $(PROFILE_BUILD_DIR)/%.o, $(SRC_FILES))

# Default rule
all: $(TARGET)

# Link all objects
$(TARGET): $(OBJ_FILES)
	@echo "Linking..."
	$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Compile each .cpp file into build/.../.o
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Profile build target
profile: $(PROFILE_TARGET)

$(PROFILE_TARGET): $(PROFILE_OBJ_FILES)
	@echo "Linking profile build..."
	$(CXX) $(PROFILE_OBJ_FILES) -o $@ $(PROFILE_LDFLAGS)

$(PROFILE_BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $< (profile)..."
	$(CXX) $(PROFILE_CXXFLAGS) -pg -c $< -o $@

# Run and analyze profile
run-profile: $(PROFILE_TARGET)
	@echo "Running profiler..."
	./$(PROFILE_TARGET)
	@echo "\nGenerating profile report..."
	gprof $(PROFILE_TARGET) gmon.out > profile_report.txt
	@echo "Profile report saved to profile_report.txt"
	@echo "\nTop 10 time consumers:"
	@gprof $(PROFILE_TARGET) gmon.out | head -n 30

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(PROFILE_BUILD_DIR) $(TARGET) $(PROFILE_TARGET) gmon.out profile_report.txt

# Run program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run profile run-profile