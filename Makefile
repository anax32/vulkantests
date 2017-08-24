CXX = g++
CXXFLAGS = -std=c++14 $(INCLUDE)
INCLUDE = -I$(VULKAN_SDK)/include
LDLIBS = -L$(VULKAN_SDK)/lib -lvulkan

SOURCES=$(wildcard *.cpp)
OUTS=$(addprefix bin/,$(notdir $(SOURCES:.cpp=.o)))

bin/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ common/utils.cpp $(LDLIBS) -o $@

all : $(SOURCES) $(OUTS)

clean:
	$(RM) bin/*
	$(RM) *.o
