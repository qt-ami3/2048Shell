CXX = g++
CXXFLAGS = -std=c++17 -Wall

TARGET = 2048shell
PORTABLE_TARGET = $(TARGET)-portable
SRC = src/main.cpp libs/inih/INIReader.cpp libs/inih/ini.c
BUILD_DIR = build

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$(TARGET) $(SRC)

# Self-contained build: reads and writes every data file next to the binary,
# so it runs straight out of build/ with nothing installed under /usr.
portable: $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DPORTABLE_BUILD -o $(BUILD_DIR)/$(PORTABLE_TARGET) $(SRC)
	cp assets/copywriteNotice.txt $(BUILD_DIR)/copywriteNotice.txt
	cp LICENSE $(BUILD_DIR)/LICENSE
	@test -f $(BUILD_DIR)/leaderBoard.ini || printf '[leaderBoard]\nfirst=0\nfirstName=\nsecond=0\nsecondName=\nthird=0\nthirdName=\n' > $(BUILD_DIR)/leaderBoard.ini
	@echo "Portable build ready: ./$(BUILD_DIR)/$(PORTABLE_TARGET)"

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean portable
