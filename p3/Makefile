CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -O2

# 所有的 .cpp 檔案
SRCS := main.cpp Pet.cpp pet_battle.cpp BoardingSystem.cpp

# 產生對應的 .o 檔案
OBJS := $(SRCS:.cpp=.o)

# 最終的執行檔
TARGET := game

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

