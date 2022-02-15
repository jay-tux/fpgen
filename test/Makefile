SOURCES=$(shell find $(SRCD) -name '*.cpp')
DEPS=$(SOURCES:$(SRCD)/%.cpp=$(OBJD)/%.d)
TESTS=generator

all: dirs conan/conanbuildinfo.mak $(BIND)/generator

-include $(DEPS)
-include conan/conanbuildinfo.mak
CXXXTRA=$(CONAN_INCLUDE_DIRS:%=-I%) $(CONAN_CXXFLAGS)
LDXTRA=$(CONAN_LIB_DIRS:%=-L%) $(CONAN_LIBS:%=-l%) $(CONAN_SYSTEM_LIBS:%=-l%)

dirs:
	@([ ! -d $(OBJD)/generator ] && mkdir -p $(OBJD)/generator) || true

$(BIND)/generator: $(OBJD)/generator/test_generator.o
	$(CC) $< $(LDXTRA) $(LDARGS) -o $@
	$(BIND)/generator --gtest_output="xml:$(BIND)/"

$(OBJD)/%.o: $(SRCD)/%.cpp Makefile
	$(CC) $(CXXARGS) $(CXXXTRA) $< -o $@

conan/conanbuildinfo.mak: conanfile.txt
	@([ ! -d conan/ ] && mkdir -p conan/) || true
	cd conan && CC=gcc CXX=$(CC) conan install .. --build=gtest

clean:
	find ./bin/ -type f | grep -v '.gitkeep' | xargs rm -rf
	find ./obj/ -type f | grep -v '.gitkeep' | xargs rm -rf
	find ./conan/ -type f | grep -v '.gitkeep' | xargs rm -rf

.PHONY: all dirs clean
