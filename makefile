# ==============================
# Compiler and flags
# ==============================
CXX := g++
CXXFLAGS := -std=c++17 -O3 -march=native -Iinclude
LDFLAGS := -pthread

# Profiling flags
PROFILE_CXXFLAGS := -std=c++17 -O2 -g -pg -Iinclude
PROFILE_LDFLAGS := -pthread -pg

# ==============================
# Directories
# ==============================
SRC_DIR := src
BUILD_DIR := build
PROFILE_BUILD_DIR := build_profile
INCLUDE_DIR := include

# ==============================
# Targets
# ==============================
TARGET := spritz
PROFILE_TARGET := spritz_profile
LIB := $(BUILD_DIR)/libspritz.a
PROFILE_LIB := $(PROFILE_BUILD_DIR)/libspritz.a

# ==============================
# Source discovery
# ==============================
SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
MAIN_SRC := main.cpp

SRC_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
PROFILE_SRC_OBJS := $(patsubst %.cpp,$(PROFILE_BUILD_DIR)/%.o,$(SRC_FILES))

MAIN_OBJ := $(BUILD_DIR)/main.o
PROFILE_MAIN_OBJ := $(PROFILE_BUILD_DIR)/main.o

# ==============================
# Examples
# ==============================
EXAMPLE_DIRS := $(wildcard *Example)
EXAMPLE_SRCS := $(addsuffix /example.cpp,$(EXAMPLE_DIRS))
EXAMPLE_BINS := $(addsuffix /example,$(EXAMPLE_DIRS))

# ==============================
# Default target
# ==============================
all: $(TARGET)

# ==============================
# Static library
# ==============================
$(LIB): $(SRC_OBJS)
	@echo "Archiving $@..."
	ar rcs $@ $^

$(PROFILE_LIB): $(PROFILE_SRC_OBJS)
	@echo "Archiving profile library $@..."
	ar rcs $@ $^

# ==============================
# Main executable
# ==============================
$(TARGET): $(LIB) $(MAIN_OBJ)
	@echo "Linking $(TARGET)..."
	$(CXX) $(MAIN_OBJ) $(LIB) -o $@ $(LDFLAGS)

$(PROFILE_TARGET): $(PROFILE_LIB) $(PROFILE_MAIN_OBJ)
	@echo "Linking $(PROFILE_TARGET)..."
	$(CXX) $(PROFILE_MAIN_OBJ) $(PROFILE_LIB) -o $@ $(PROFILE_LDFLAGS)

# ==============================
# Object compilation rules
# ==============================
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PROFILE_BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $< (profile)..."
	$(CXX) $(PROFILE_CXXFLAGS) -c $< -o $@

# ==============================
# Examples
# ==============================
examples: $(EXAMPLE_BINS)

%Example/example: %Example/example.cpp $(LIB)
	@echo "Linking example in $(@D)..."
	$(CXX) $(CXXFLAGS) $< $(LIB) -o $@ $(LDFLAGS)

# ==============================
# Profiling
# ==============================
profile: $(PROFILE_TARGET)

run-profile: $(PROFILE_TARGET)
	@echo "Running profiler..."
	./$(PROFILE_TARGET)
	@echo "Generating profile report..."
	gprof $(PROFILE_TARGET) gmon.out > profile_report.txt
	@echo "Profile report saved to profile_report.txt"
	@gprof $(PROFILE_TARGET) gmon.out | head -n 30

# ==============================
# Utilities
# ==============================
run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(PROFILE_BUILD_DIR) \
	       $(TARGET) $(PROFILE_TARGET) \
	       $(EXAMPLE_BINS) gmon.out profile_report.txt

.PHONY: all clean run profile run-profile examples
