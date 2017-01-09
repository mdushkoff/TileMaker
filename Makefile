# Target executables
TARGET = tilemaker
TEST   = test

# Extensions
SRCEXT = c

# Folder structure
SRCDIR = src
TSTDIR = test
BULDIR = build
INCDIR = include

# Sources and directories
SRCS    := $(shell find $(SRCDIR) -name '*.$(SRCEXT)')
SRCDIRS := $(shell find $(SRCDIR) -name '*.$(SRCEXT)' -exec dirname {} \; | uniq)
TSTS    := $(shell find $(TSTDIR) -name '*.$(SRCEXT)')
TSTDIRS := $(shell find $(TSTDIR) -name '*.$(SRCEXT)' -exec dirname {} \; | uniq)
OBJS    := $(patsubst %.$(SRCEXT),$(BULDIR)/%.o,$(SRCS))

# Flags and compiler definition
CC       = gcc
INCLUDES = -I./$(INCDIR)
CFLAGS   = -Wall -c $(INCLUDES)
LDFLAGS  = -lm -lpng
DEBUG    = -d


all: $(TARGET)

$(TARGET): buildrepo $(OBJS)
	@echo "Linking $@..."
	@$(CC) $(OBJS) $(LDFLAGS) -o $@

$(TEST): buildrepo $(OBJS)
	@echo "Not yet implemented..."

$(BULDIR)/%.o: %.$(SRCEXT)
	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BULDIR)
	rm -f $(TARGET)

buildrepo:
	@$(call make-repo)

define make-repo
    for dir in $(SRCDIRS); \
    do \
        mkdir -p $(BULDIR)/$$dir; \
    done
endef
