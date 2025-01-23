CXX = g++
CXXFLAGS = -std=c++11

TARGET = fs

SRCS = main.cpp fs.cpp persistency.cpp debug.cpp misc.cpp repair.cpp

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)
