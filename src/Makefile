CXXFLAGS = -shared -fPIC --no-gnu-unique -s -std=c++23 -O3 -DNDEBUG -Wno-c++11-narrowing
INCLUDES = `pkg-config --cflags pixman-1 libdrm hyprland`
LIBS = `pkg-config --libs pixman-1 libdrm hyprland`

SRC = main.cpp
TARGET = hyprdrag.so

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ $> -o $@ $(LIBS) -O2

clean:
	rm ./$(TARGET)

.PHONY: all clean
