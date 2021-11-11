CXXFLAGS = -I include/ -O0 -g -Wall -fmessage-length=0 -std=c++11

SRC_DIR_LIB=src
SRC_DIR_EXE=main
OBJ_DIR_LIB=obj/lib
OBJ_DIR_EXE=obj/exe
BIN_DIR=bin
HEAD_DIR=include

SRC_FILES_LIB = $(wildcard $(SRC_DIR_LIB)/*.cpp)
SRC_FILES_EXE = $(wildcard $(SRC_DIR_EXE)/*.cpp)
HEAD_FILES    = $(wildcard $(HEAD_DIR)/*.h)

OBJ_FILES_LIB = $(patsubst $(SRC_DIR_LIB)/%.cpp,$(OBJ_DIR_LIB)/%.o,$(SRC_FILES_LIB))
OBJ_FILES_EXE = $(patsubst $(SRC_DIR_EXE)/%.cpp,$(OBJ_DIR_EXE)/%.o,$(SRC_FILES_EXE))

EXEC_FILES  = $(patsubst $(SRC_DIR_EXE)/%.cpp,$(BIN_DIR)/%,$(SRC_FILES_EXE))

$(OBJ_DIR_EXE)/%.o:	$(SRC_DIR_EXE)/%.cpp $(OBJ_FILES_LIB) $(HEAD_FILES)
	$(CXX) -o $@ -c $< $(CXXFLAGS)

$(OBJ_DIR_LIB)/%.o:	$(SRC_DIR_LIB)/%.cpp $(HEAD_FILES)
	$(CXX) -o $@ -c $< $(CXXFLAGS)
	
$(BIN_DIR)/%:	$(OBJ_DIR_EXE)/%.o
	#$(CXX) -o $@ -s $(subst $(BIN_DIR)/,$(OBJ_DIR_EXE)/,$@).o $(OBJ_FILES_LIB) $(HEAD_FILES) $(LDFLAGS) $(CXXFLAGS)
	$(CXX) -o $@ $(subst $(BIN_DIR)/,$(OBJ_DIR_EXE)/,$@).o $(OBJ_FILES_LIB) $(HEAD_FILES) $(LDFLAGS) $(CXXFLAGS)

all:	PRE_REQ $(EXEC_FILES) $(OBJ_FILES_LIB)
	@echo "Cleaning and Symlinking."
	rm -rf ./obj
	cp ./bin/sim_proc ./sim_proc
	@echo " "
	@echo "************************************************************************************************"
	@echo "********************************** BUILD COMPLETE **********************************************"
	@echo "************************************************************************************************"

PRE_REQ:
	rm -rf ./obj
	rm -rf ./bin
	rm -rf ./sim
	mkdir bin
	mkdir obj
	mkdir obj/lib
	mkdir obj/exe

show:
	@echo "SRC_FILES_LIB=$(SRC_FILES_LIB)"
	@echo "HEADERS=$(HEAD_FILES)"
	@echo "SRC_FILES_EXE=$(SRC_FILES_EXE)"
	@echo "$(EXEC_FILES)"
	# Debugger Enable in BIN_DIR: $(CXX) -o $@ $(subst $(BIN_DIR)/,$(OBJ_DIR_EXE)/,$@).o $(OBJ_FILES_LIB) $(HEAD_FILES) $(LDFLAGS) $(CXXFLAGS)

clean:
	rm -rf *.o
	rm -rf ./bin
	rm -rf ./obj
	rm -f sim_proc
