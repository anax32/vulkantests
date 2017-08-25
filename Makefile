CXX = g++
CXXFLAGS = -std=c++14 $(INCLUDE)
INCLUDE = -I$(VULKAN_SDK)/include
LDLIBS = -L$(VULKAN_SDK)/lib -lvulkan

SOURCES=$(wildcard *.cpp)
OUT_DIR=bin/
OUTS=$(addprefix $(OUT_DIR), $(notdir $(SOURCES:.cpp=.o)))

bin/:
	mkdir -p $(OUT_DIR)

bin/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ common/utils.cpp $(LDLIBS) -o $@

all : $(OUT_DIR) $(SOURCES) $(OUTS)

test : all
	for i in $(OUT_DIR)*; do echo $$i && $$i; done

grind: all
	for i in $(OUT_DIR)*; do valgrind $$i; done

clean:
	rm -dr $(OUT_DIR)
