CXX := g++

TARGET := webapp-monitor-tool

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build

SOURCES := $(SRC_DIR)/main.cpp $(SRC_DIR)/config.cpp $(SRC_DIR)/logger.cpp $(SRC_DIR)/i_endpoint_monitor.cpp $(SRC_DIR)/http_endpoint_monitor.cpp $(SRC_DIR)/health_check_runner.cpp
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -Iinclude -Iinclude/libraries


.PHONY: all clean run debug release

all: release

release: CXXFLAGS += -O2
release: $(TARGET)

debug: CXXFLAGS += -g -O0
debug: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)