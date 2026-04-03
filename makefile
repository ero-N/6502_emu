# 1. 指定编译器
CXX = g++

# 2. 编译选项：-Wall 开启警告，-Iinclude 告诉它头文件在 include 目录
CXXFLAGS = -g -Wall -Wextra -Iinclude -std=c++17

# 3. 目标文件名
TARGET = bin/6502_emu

# 4. 自动寻找所有的 .cpp 文件
# wildcard 会扫描 src 文件夹下所有的 cpp
SRCS = $(wildcard src/*.cpp)

# 5. 将 .cpp 的名字换成 .o (对象文件)，放在中间过程
OBJS = $(SRCS:.cpp=.o)



all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# %：通配符。
# $<：代表当前的 .cpp 文件。
# $@：代表生成的 .o 文件。
# -c：只编译，不链接（这样改了一个文件，下次只重编这一个，速度快）。
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean