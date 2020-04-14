CC=gcc
AR=ar
RM=del

OBJ_DIR=obj

CFLAGS=-c
CDEF=-D _CRT_SECURE_NO_WARNINGS -D _GLFW_WIN32
CPPFLAGS=-I../glfw-3.3.2/include -I../glad/include
LDFLAGS=rcs
LDLIBS=
SRC=imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_widgets.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp
OBJ=$(SRC:%.cpp=$(OBJ_DIR)/%.o)

LIB=imgui.lib

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(LDFLAGS) $@ $(LDLIBS) $^

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CC) $(CDEF) $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	$(RM) $(subst /,\,$(OBJ)) $(LIB)