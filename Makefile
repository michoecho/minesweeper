BUILD_DIR = ./build
RES_DIR = ./res
BIN = main
SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)
DEP = $(OBJ:%.o=%.d)
CCFLAGS += -Wall
CPPFLAGS += -I/usr/include/SDL2 -D_REENTRANT -MMD -MP -Wall
LDFLAGS += -L/usr/lib
LDLIBS += -lSDL2 -lSDL2_image

all : $(BUILD_DIR)/$(BIN)
	cp -r $(RES_DIR) $(BUILD_DIR)

$(BUILD_DIR)/$(BIN) : $(OBJ)
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

-include  $(DEP)

$(BUILD_DIR)/%.o : %.c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $< -o $@

run : all
	$(BUILD_DIR)/$(BIN)

.PHONY : run
