

CXX = g++
CXXFLAGS = -Wall -g

TARGET = hprof_parser

OBJS = ./src/main.o

all: $(TARGET)

hprof_parser: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f */*.o $(TARGET)