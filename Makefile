EXTRA_CXX=
EXTRA_LD=
BUILD_DIR=$(abspath ./test/obj)
BIN_DIR=$(abspath ./test/bin)
TEST_DIR=$(abspath ./test/src/)
INSTALL_DIR=/usr/include/fpgen
INCL_PATH=$(abspath ./inc)
DOC_DIR=$(abspath ./docs/)

JOB_COUNT=4

CC=g++
CXXARGS=-I$(abspath ./inc)
LDARGS=

all:
	@echo "Please choose a target:"
	@echo "  -> make install:   installs the headers under $(INSTALL_DIR)"
	@echo "  -> make uninstall: uninstalls the headers if they are under $(INSTALL_DIR)"
	@echo "  -> make docs:      generates the documentation using doxygen under $(DOC_DIR)"
	@echo "  -> make test:      builds and runs the tests in $(BUILD_DIR) and $(BIN_DIR) from $(TEST_DIR)"
	@echo "  -> make clean:     cleans up test builds and documentation (from $(BUILD_DIR), $(BIN_DIR), $(DOC_DIR))"
	@echo ""
	@echo "Some targets accept additional arguments in the form of KEY=VALUE pairs:"
	@echo "  -> CC (for test): sets the command for the C++ compiler (g++ by default)"
	@echo "  -> CXXARGS (for test): current arguments to the compiler - not recommended to change"
	@echo "  -> EXTRA_CXX (for test): additional compilation flags/arguments"
	@echo "  -> LDARGS (for test): current arguments to the linker - not recommended to change"
	@echo "  -> EXTRA_LD (for test): additional linker flags/arguments"
	@echo "  -> BUILD_DIR (for test): build directory"
	@echo "  -> BIN_DIR (for test): binary directory"
	@echo "  -> TEST_DIR (for test): test sources directory"
	@echo "  -> INSTALL_DIR (for install and uninstall): sets the installation directory"
	@echo "  -> INCL_PATH (for install, docs and test): the path to the directory with the headers, if you run make from another directory"
	@echo "  -> DOC_DIR (for docs): the path to the build directory for the documentation"
	@echo "  -> JOB_COUNT (for test): the amount of threads to start"
	@echo " Current/default arguments: "
	@echo "  CC=$(CC) CXXARGS=$(CXXARGS) LDARGS=$(LDARGS) EXTRA_CXX=$(EXTRA_CXX) EXTRA_LD=$(EXTRA_LD)"
	@echo "  BUILD_DIR=$(BUILD_DIR) BIN_DIR=$(BIN_DIR) TEST_DIR=$(TEST_DIR) INSTALL_DIR=$(INSTALL_DIR) INCL_PATH=$(INCL_PATH) DOC_DIR=$(DOC_DIR) JOB_COUNT=$(JOB_COUNT)"

install:
	[ ! -d $(INSTALL_DIR) ] && mkdir -p $(INSTALL_DIR)
	cp $(INCL_PATH)/*.hpp $(INSTALL_DIR)

uninstall:
	[ ! -d $(INSTALL_DIR) ] && rm -rf $(INSTALL_DIR)

docs:
	[ ! -d $(DOC_DIR) ] && mkdir -p $(DOC_DIR)
	OUTDIR=$(DOC_DIR) INDIR=$(INCL_PATH) doxygen doxyfile.mk

test:
	make CC="$(CC)" OBJD="$(BUILD_DIR)" BIND="$(BIN_DIR)" SRCD="$(TEST_DIR)" CXXARGS="$(CXXARGS) $(EXTRA_CXX) -I$(INCL_PATH)" LDARGS="$(LDARGS) $(EXTRA_LD)" -C $(TEST_DIR)/.. -j $(JOB_COUNT)

clean:
	rm -rf $(DOC_DIR)/* $(BUILD_DIR)/* $(BIN_DIR)/*


.PHONY: install uninstall test clean
