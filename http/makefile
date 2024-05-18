# 定义变量
CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = server
SRC = conn.cpp

# 目标：生成 server 可执行文件
all: $(TARGET)

# 链接并生成可执行文件
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 清理
clean:
	rm -f $(TARGET)

.PHONY: all clean
