# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lmysqlcppconn -lhiredis

# Source files
SRCS = app.cpp con.cpp dbm.cpp tp.cpp

# Output binary
TARGET = server

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)

