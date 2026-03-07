TARGET_OS ?= linux
BUILD_DIR = build

SOURCES = main.cpp cexportentry.cpp cimportentry.cpp cimepackage.cpp cme3exportentry.cpp cmasseffect3package.cpp me3_decompress.cpp propertytypedef.cpp objectdef.cpp utils.cpp zblock.cpp 

OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

ifeq ($(TARGET_OS), windows)
	CXX = i686-w64-mingw32-g++-posix
	CXXFLAGS = -Wall -std=c++20
	LDFLAGS = -lz -lstdc++ -static
	TARGET = $(BUILD_DIR)/me3_csv_ekstraktor_Win32.exe
else
	CXX = g++
	CXXFLAGS = -Wall -std=c++20
	LDFLAGS = -no-pie -lz
	TARGET = $(BUILD_DIR)/me3_csv_ekstraktor_Linux
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
