CXX = g++
CFLAGS = -std=c++14 -Wall

TARGET = webserver

OBJS = buffer/*.cpp http/*.cpp pool/*.h server/*.cpp main.cpp

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(TARGET) -pthread

clean:
	rm $(TARGET)