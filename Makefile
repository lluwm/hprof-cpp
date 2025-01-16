

CXX = g++
CXXFLAGS = -Wall -g -Iinclude

TARGET = hprof_parser

OBJS =  ./src/main.o \
		./include/cxxtopt.o

all: $(TARGET)

hprof_parser: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f */*.o $(TARGET)