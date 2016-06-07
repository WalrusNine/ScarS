src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
exe = ScarS

libs = assimp glew glfw3 freetype2
includes = -Iopengl-wrappers/glm/glm $(shell pkg-config --cflags $(libs))

build_dir = build
objs = $(foreach o, $(obj), $(build_dir)/$(o))

CXX = g++
CXXFLAGS = -Wall -std=c++11 -fpermissive $(includes)
LINKS = $(shell pkg-config --libs $(libs)) -lfreeimage


all : pasta projeto
	
projeto : $(objs)
	$(CXX) -o $(build_dir)/$(exe) $(CXXFLAGS) $^ $(LINKS)

pasta :
	@mkdir -p $(build_dir)

$(build_dir)/%.o : %.cpp
	$(CXX) -o $@ $(CXXFLAGS) $< -c

run :
	@$(build_dir)/$(exe)

clean :
	$(RM) $(build_dir)/*
