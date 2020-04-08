CC=g++
RM=del

SRC_DIR=src
OBJ_DIR=obj
INC_DIR=-Idependencies/glfw-3.3.2/include -Idependencies/glad/include
LIB_DIR=-Ldependencies/glfw-3.3.2 -Ldependencies/glad

CFLAGS=-c -std=c++11
CDEF=-D _CRT_SECURE_NO_WARNINGS -D _GLFW_WIN32
CPPFLAGS=$(INC_DIR) -l.
LDFLAGS=$(LIB_DIR)
LDLIBS=-lglfw3 -lopengl32 -lgdi32
MODS=dependencies/glfw-3.3.2 dependencies/glad

SRC=$(wildcard $(SRC_DIR)/*.cpp)
OBJ=$(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

EXE=FlockingSimulation.exe

.PHONY: all makerun clean $(MODS)

all: $(EXE)
	@echo BUILD SUCCESSFUL: $(EXE)

makerun: all
	$(EXE)

$(EXE): $(OBJ) $(MODS)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) -o $@ $< $(CDEF) $(CPPFLAGS) $(CFLAGS)

$(MODS):
	$(MAKE) --directory=$@

define NEWLINE


endef

clean:
	$(RM) $(subst /,\,$(OBJ)) $(EXE)
	$(foreach mod,$(MODS),$(MAKE) -C $(mod) -f makefile clean$(NEWLINE))