# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lmysqlclient -lhiredis

# Source files
SRCS = DBM.cpp

# Output binary
TARGET = DBM

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)

