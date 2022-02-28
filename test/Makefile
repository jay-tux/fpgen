SOURCES=$(shell find $(SRCD) -name '*.cpp')
DEPS=$(SOURCES:$(SRCD)/%.cpp=$(OBJD)/%.d)
TESTS=generator sources manip aggreg chain
TESTOBJ=$(TESTS:%=$(OBJD)/test_%.o)

CONAN_PKG_OVERRIDE=
CONAN_MODIFY=$(CONAN_PKG_OVERRIDE:%=-s %:compiler.version=11.2)

all: dirs conan/conanbuildinfo.mak $(BIND)/_test

-include $(DEPS)
-include conan/conanbuildinfo.mak
CXXXTRA=$(CONAN_INCLUDE_DIRS:%=-I%) $(CONAN_CXXFLAGS)
LDXTRA=$(CONAN_LIB_DIRS:%=-L%) $(CONAN_LIBS:%=-l%) $(CONAN_SYSTEM_LIBS:%=-l%)

dirs:
	@([ ! -d $(OBJD)/generator ] && mkdir -p $(OBJD)/generator) || true
	@([ ! -d $(OBJD)/sources ] && mkdir -p $(OBJD)/sources) || true

$(BIND)/_test: $(TESTOBJ) $(OBJD)/test_main.o
	$(CC) $(LDARGS) $(LDXTRA) $^ -o $@
	$(BIND)/_test

$(OBJD)/%.o: $(SRCD)/%.cpp Makefile
	$(CC) $(CXXARGS) $(CXXXTRA) $< -o $@

conan/conanbuildinfo.mak: conanfile.txt
	@([ ! -d conan/ ] && mkdir -p conan/) || true
	cd conan && CC=gcc CXX=$(CC) conan install .. $(CONAN_MODIFY)

clean:
	find ./bin/ -type f | grep -v '.gitkeep' | xargs rm -rf
	find ./obj/ -type f | grep -v '.gitkeep' | xargs rm -rf
	find ./conan/ -type f | grep -v '.gitkeep' | xargs rm -rf

.PHONY: all dirs clean $(TESTS:%=$(BIND)/%) $(BIND)/_test
