CC        := gcc
CFLAGS    := -Wall -Wextra -Wpedantic
BUILD     := release
BUILD_DIR := build/$(BUILD)
TARGET    := $(BUILD_DIR)/splicer
SRCS      := $(wildcard src/*.c)
HDRS      := $(wildcard src/*.h)
OBJS      := $(SRCS:src/%.c=$(BUILD_DIR)/%.o)
PREFIX	  ?= /usr/local
BINDIR	  := $(PREFIX)/bin
MANDIR	  := $(PREFIX)/man

ifeq ($(BUILD),release)
	CFLAGS += -O3 -DNDEBUG
else
	CFLAGS += -g -O0
endif

.PHONY: all debug release clean

all: $(TARGET) $(BUILD_DIR)/splicer.1.gz

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@
	@ln -sf $(TARGET) $(BUILD)

$(BUILD_DIR)/%.o: src/%.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.gz: src/man/%
	gzip -c $< > $@

release:
	$(MAKE) BUILD=release

debug:
	$(MAKE) BUILD=debug

clean:
	rm -rf build
	find -type l -delete

install:
	@echo "Installing to $(DESTDIR)$(BINDIR)..."
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/splicer
	@echo "Installing man page to $(DESTDIR)$(MANDIR)..."
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 644 $(BUILD_DIR)/splicer.1.gz $(DESTDIR)$(MANDIR)/man1

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/splicer
	rm -f $(DESTDIR)$(MANDIR)/man1/splicer.1.gz
