CC=gcc
AR=ar
RM=del

SRC_DIR=src
OBJ_DIR=obj

CFLAGS=-c
CDEF=-D _CRT_SECURE_NO_WARNINGS -D _GLFW_WIN32
CPPFLAGS=
LDFLAGS=rcs
LDLIBS=
SRC=$(SRC_DIR)/stb_image.c
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

LIB=stb_image.lib

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(LDFLAGS) $@ $(LDLIBS) $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CDEF) $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	$(RM) $(subst /,\,$(OBJ)) $(LIB)