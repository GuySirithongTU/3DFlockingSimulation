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
SRC=$(SRC_DIR)/context.c $(SRC_DIR)/init.c $(SRC_DIR)/input.c $(SRC_DIR)/monitor.c $(SRC_DIR)/vulkan.c $(SRC_DIR)/window.c $(SRC_DIR)/win32_init.c $(SRC_DIR)/win32_joystick.c $(SRC_DIR)/win32_monitor.c $(SRC_DIR)/win32_time.c $(SRC_DIR)/win32_thread.c $(SRC_DIR)/win32_window.c $(SRC_DIR)/wgl_context.c $(SRC_DIR)/egl_context.c $(SRC_DIR)/osmesa_context.c
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

LIB=glfw3.lib

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(LDFLAGS) $@ $(LDLIBS) $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CDEF) $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	$(RM) $(subst /,\,$(OBJ)) $(LIB)