CC        := gcc
CFLAGS    := -Wall -Wextra -Wpedantic
BUILD     := debug
BUILD_DIR := build/$(BUILD)
TARGET    := $(BUILD_DIR)/splicer
SRCS      := $(wildcard src/*.c)
OBJS      := $(SRCS:src/%.c=$(BUILD_DIR)/%.o)

ifeq ($(BUILD),release)
	CFLAGS += -O3 -DNDEBUG
else
	CFLAGS += -g -O0
endif

.PHONY: all debug release clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@
	@ln -sf $(TARGET) $(BUILD)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

release:
	$(MAKE) BUILD=release

debug:
	$(MAKE) BUILD=debug

clean:
	rm -rf build
	find -type l -delete
