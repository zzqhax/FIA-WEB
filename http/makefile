# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall
LIBS = -lmysqlclient

# Source files
SRCS = conn.cpp

# Output binary
TARGET = server

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)

