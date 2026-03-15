CXX = g++
CXXFLAGS = -std=c++17 -Wall

TARGET = 2048
SRC = src/main.cpp libs/inih/INIReader.cpp libs/inih/ini.c
BUILD_DIR = build

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$(TARGET) $(SRC)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
