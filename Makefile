VERSION		= 0.1

# toolchain
CC		= clang
LD		= clang
AR		= ar
TAR		= tar
TAG		= etags

# vpath
VPATH		= src:test

# build dir
BDIR		= build

# Flags
CFLAGS		= -Wall -Wextra -Werror
CPPFLAGS	= -DWLR_USE_UNSTABLE=1 -I/usr/include/libdrm -I/usr/include/pixman-1
LDFLAGS		= 
LDLIBS      = -lwayland-server -lwlroots

# architecture
ARCH		= -march=native

# build mode
ifneq ($(DEBUG), )
	CFLAGS  	+= -O0 -g
	CPPFLAGS	+= -DDEBUG
	BDIR	 	= build/debug
else
	CFLAGS  	+= -O3
	CPPFLAGS	+= -DNDEBUG
	BDIR	 	= build/release
endif

all: $(BDIR)/kurai

$(BDIR)/kurai: $(BDIR)/kurai.o $(BDIR)/kurai.a
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BDIR)/kurai.a: $(filter-out %/kurai.o,$(patsubst src/%.c,$(BDIR)/%.o,$(wildcard src/*.c)))

$(BDIR)/%.a:
	$(AR) rcs $@ $^

$(BDIR)/%.o: %.c $(BDIR)/%.d
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH) -c -o $@ $<

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
	@rm -f $(BDIR)/*.o
	@rm -f $(BDIR)/*.a
	@rm -f $(BDIR)/*.d
	@rm -f $(BDIR)/kurai

.PHONY: all check dist install dist-clean tags clean
