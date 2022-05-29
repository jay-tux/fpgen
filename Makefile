EXTRA_CXX=
EXTRA_LD=
BUILD_DIR=$(abspath ./test/obj)
BIN_DIR=$(abspath ./test/bin)
TEST_DIR=$(abspath ./test/src/)
INSTALL_DIR=/usr/include/fpgen
INCL_PATH=$(abspath ./inc)
DOC_DIR=$(abspath ./docs/)
HTMLDIR=$(abspath ./cov/)
BROWSER=firefox

CC=g++
CONAN_CC=gcc
CXXARGS=-I$(abspath ./inc) -g -c -std=c++20 -MMD -fprofile-arcs -ftest-coverage
LDARGS=-fprofile-arcs -ftest-coverage

all:
	@echo "Please choose a target:"
	@echo "  -> make install:         installs the headers under $(INSTALL_DIR)"
	@echo "  -> make uninstall:       uninstalls the headers if they are under $(INSTALL_DIR)"
	@echo "  -> make docs:            generates the documentation using doxygen under $(DOC_DIR)"
	@echo "  -> make test:            builds and runs the tests in $(BUILD_DIR) and $(BIN_DIR) from $(TEST_DIR)"
	@echo "  -> make test-clang:      builds and runs the tests in $(BUILD_DIR) and $(BIN_DIR) from $(TEST_DIR) (using clang++)"
	@echo "  -> make clean:           cleans up test builds and documentation (from $(BUILD_DIR), $(BIN_DIR), $(DOC_DIR))"
	@echo "  -> make coverage:        builds and runs the tests, then generates a coverage report in $(HTMLDIR) and opens it in $(BROWSER)"
	@echo ""
	@echo "Some targets accept additional arguments in the form of KEY=VALUE pairs:"
	@echo "  -> CC (for test and coverage): sets the command for the C++ compiler (g++ by default)"
	@echo "  -> CONAN_CC (for test and coverage): sets the conan compiler name (gcc by default)"
	@echo "  -> CXXARGS (for test and coverage): current arguments to the compiler - not recommended to change"
	@echo "  -> EXTRA_CXX (for test and coverage): additional compilation flags/arguments"
	@echo "  -> LDARGS (for test and coverage): current arguments to the linker - not recommended to change"
	@echo "  -> EXTRA_LD (for test and coverage): additional linker flags/arguments"
	@echo "  -> EXTRA_CONAN (for test and coverage): additional arguments for conan"
	@echo "  -> BUILD_DIR (for test and coverage): build directory"
	@echo "  -> BIN_DIR (for test and coverage): binary directory"
	@echo "  -> TEST_DIR (for test and coverage): test sources directory"
	@echo "  -> INSTALL_DIR (for install and uninstall): sets the installation directory"
	@echo "  -> INCL_PATH (for install, docs, test and coverage): the path to the directory with the headers, if you run make from another directory"
	@echo "  -> DOC_DIR (for docs): the path to the build directory for the documentation"
	@echo "  -> BROWSER (for coverage): the browser in which to open the coverage report"
	@echo "  -> HTMLDIR (for coverage): the directory in which to generate the coverage report"
	@echo " Current/default arguments: "
	@echo "  CC=$(CC) CXXARGS=$(CXXARGS) LDARGS=$(LDARGS) EXTRA_CXX=$(EXTRA_CXX) EXTRA_LD=$(EXTRA_LD)"
	@echo "  BUILD_DIR=$(BUILD_DIR) BIN_DIR=$(BIN_DIR) TEST_DIR=$(TEST_DIR) INSTALL_DIR=$(INSTALL_DIR) INCL_PATH=$(INCL_PATH) DOC_DIR=$(DOC_DIR)"
	@echo "  BROWSER=$(BROWSER) HTMLDIR=$(HTMLDIR)"
	@echo ""

install:
	([ ! -d $(INSTALL_DIR) ] && mkdir -p $(INSTALL_DIR)) || true
	cp $(INCL_PATH)/*.hpp $(INSTALL_DIR)

uninstall:
	[ -d $(INSTALL_DIR) ] && rm -rf $(INSTALL_DIR)

docs:
	@([ ! -d $(DOC_DIR) ] && mkdir -p $(DOC_DIR)) || true
	OUTDIR=$(DOC_DIR) INDIR=$(INCL_PATH) doxygen Doxyfile.mk
	$(BROWSER) $(DOC_DIR)/html/index.html

test:
	make CC="$(CC)" CONAN_CC="$(CONAN_CC)" CONARGS="$(EXTRA_CONAN)" OBJD="$(BUILD_DIR)" BIND="$(BIN_DIR)" SRCD="$(TEST_DIR)" CXXARGS="$(CXXARGS) $(EXTRA_CXX) -I$(INCL_PATH)" LDARGS="$(LDARGS) $(EXTRA_LD)" -C $(TEST_DIR)/..

test-clang:
	make CC="clang++" CONAN_CC="clang" CONARGS="$(EXTRA_CONAN) -s compiler.libcxx=libc++" OBJD="$(BUILD_DIR)" BIND="$(BIN_DIR)" SRCD="$(TEST_DIR)" CXXARGS="$(CXXARGS) $(EXTRA_CXX) -I$(INCL_PATH) -stdlib=libc++" LDARGS="$(LDARGS) $(EXTRA_LD) -stdlib=libc++" -C $(TEST_DIR)/..

clean:
	rm -rf $(DOC_DIR)/*
	cd $(TEST_DIR)/.. && make clean OBJD="$(BUILD_DIR)" BIND="$(BIN_DIR)" SRCD="$(TEST_DIR)"

coverage:
	make CC="$(CC)" CONAN_CC="$(CONAN_CC)" OBJD="$(BUILD_DIR)" BIND="$(BIN_DIR)" SRCD="$(TEST_DIR)" CXXARGS="$(CXXARGS) $(EXTRA_CXX) -I$(INCL_PATH)" LDARGS="$(LDARGS) $(EXTRA_LD)" -C $(TEST_DIR)/..
	lcov --directory "$(BUILD_DIR)" --output-file coverage.info -c --exclude '*gmock' --exclude '*doctest*' --exclude '/usr/*'
	genhtml coverage.info --output-directory "$(HTMLDIR)"
	$(BROWSER) $(HTMLDIR)/index.html

.PHONY: install uninstall test clean docs coverage
