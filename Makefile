VERSION		= 0.1

# toolchain
CC		= cc
LD		= cc
AR		= ar
TAR		= tar
TAG		= etags

# vpath
VPATH		= src:test

# include
INCLUDE     = include

# build dir
BDIR		= build

# Flags
CFLAGS		= -Wall
CPPFLAGS	= -DWLR_USE_UNSTABLE=1 -I/usr/include/libdrm -I/usr/include/pixman-1 -I$(INCLUDE)
LDFLAGS		= 
LDLIBS      = -lwayland-server -lwlroots

# architecture
ARCH		= -march=native

# build mode
ifneq ($(DEBUG), )
	CFLAGS  	+= -Wextra -Werror -g -O0 -fsanitize=address
	CPPFLAGS	+= -DDEBUG -Wno-unused-variable -Wno-unused-function -Wno-unused-parameter
	BDIR	 	= build/debug
else
	CFLAGS  	+= -O3
	CPPFLAGS	+= -DNDEBUG
	BDIR	 	= build/release
endif

all: $(patsubst protocol/%.xml, $(INCLUDE)/%.h, $(wildcard protocol/*.xml)) $(BDIR)/kurai 

include/%.h: protocol/%.xml
	wayland-scanner server-header $< $@

$(BDIR)/kurai: $(patsubst src/%.c,$(BDIR)/%.o,$(wildcard src/*.c))
	$(LD) $(LDFLAGS) $(CFLAGS) $^ $(LDLIBS) -o $@

$(BDIR)/%.o: %.c $(BDIR)/%.d
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(BDIR)/%.d: %.c
	@mkdir -p $(@D)
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PRECIOUS: $(BDIR)/%.d

# alternatively supress errors with `-include`
include $(wildcard $(patsubst src/%.c, $(BDIR)/%.d, $(wildcard src/*.c)))

dist:
	@mkdir kurai-$(VERSION)
	@cp -r kurai -r man -r test Makefile README.md LICENSE kurai-$(VERSION)
	@$(TAR) cvzf kurai-$(VERSION).tar.gz kurai-$(VERSION)
	@rm -rf kurai-$(VERSION)

# TODO: use PREFIX instead of hardcoded path
install: $(BDIR)/kurai/kurai
	@cp $(BDIR)/kurai/kurai /usr/local/bin/
	@gzip < man/kurai.1 > /usr/local/man/man1/kurai.1.gz

dist-clean:
	@rm -rf $(BDIR)
	@rm -f kurai-$(VERSION).tar.gz

tags:
	$(TAG) **/*.c **/*.h

format:
	clang-format -i -style=file **/*.c **/*.h

clean:
	@rm -f $(patsubst protocol/%.xml, $(INCLUDE)/%.h, $(wildcard protocol/*.xml))
	@rm -rf build/*

.PHONY: all check dist install dist-clean tags clean
