BUILD_DIR = build
RES_DIR = res
BIN = ms
SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)
RES = $(wildcard $(RES_DIR)/*)
BLOBS = $(RES:$(RES_DIR)/%=$(BUILD_DIR)/blobs/%.o)
DEP = $(OBJ:%.o=%.d)
CCFLAGS += -Wall
CPPFLAGS += -I/usr/include/SDL2 -D_REENTRANT -MMD -MP -Wall -g
LDFLAGS += -L/usr/lib
LDLIBS += -lSDL2 -lSDL2_image -lSDL2_ttf


all : $(BUILD_DIR)/$(BIN) src/tags
	cp -r $(RES_DIR) $(BUILD_DIR)

$(BUILD_DIR)/$(BIN) : $(OBJ) $(BLOBS)
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

-include  $(DEP)

$(BUILD_DIR)/blobs/%.o : $(RES_DIR)/%
	mkdir -p $(@D)
	ld -r -b binary -o $@ $<

$(BUILD_DIR)/%.o : %.c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $< -o $@

run : all
	$(BUILD_DIR)/$(BIN) 30 16 99 24

src/tags :
	ctags --tag-relative=yes -f src/tags -R src

.PHONY : run
