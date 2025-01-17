CXX = g++
CXXFLAGS = -Wall -g -Iinclude -std=c++14 -stdlib=libc++

TARGET = hprof_parser

OBJS =  ./src/main.o

all: $(TARGET)

hprof_parser: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f */*.o $(TARGET)