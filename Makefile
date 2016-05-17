PROJECT = SCR
CLIENT = client
SERVER = server

OUTDIR = bin
EXECUTABLE = $(OUTDIR)/$(PROJECT)
MAP_FILE = $(OUTDIR)/$(PROJECT).map
LIST_FILE = $(OUTDIR)/$(PROJECT).lst

# Toolchain configurations
CROSS_COMPILE ?=
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size

# Basic configurations
CFLAGS += -std=c99 -Wall  
LDFLAGS += -lpthread -pthread -lcurses

# Optimizations
CFLAGS += -O0 -g

#files
TOOL = tool

SETTING = $(TOOL)/setting

SRCDIR = src

INCDIR = inc

SRC += $(wildcard $(addsuffix /*.c,$(SRCDIR)))
SERVER_SRC = $(SRC) $(wildcard $(addsuffix /$(SERVER)/*.c,$(SRCDIR)))
CLIENT_SRC = $(SRC) $(wildcard $(addsuffix /$(CLIENT)/*.c,$(SRCDIR)))

SERVER_OBJS += $(addprefix $(OUTDIR)/,$(patsubst %.s,%.o,$(SERVER_SRC:.c=.o)))
CLIENT_OBJS += $(addprefix $(OUTDIR)/,$(patsubst %.s,%.o,$(CLIENT_SRC:.c=.o)))

INCLUDES = $(addprefix -I,$(INCDIR))
INCLUDES += $(addprefix -I,$(INCDIR)/$(SERVER))
INCLUDES +=  $(addprefix -I,$(INCDIR)/$(CLIENT))

DEP = $(OBJS:.o=.d)

#MAKDIR = mk
#MAK = $(wildcard $(MAKDIR)/*.mk)

#include $(MAK)

all: $(EXECUTABLE)_$(SERVER) $(EXECUTABLE)_$(CLIENT)
	@echo "[ CP ]    "$(SETTING)" -> "$(OUTDIR)/
	@cp -a $(SETTING) $(OUTDIR)

$(EXECUTABLE)_$(SERVER): $(SERVER_OBJS)
	@echo "[ LD ]    "$@
	@$(CC) $(CFLAGS)  -o $@ $^ $(LDFLAGS)

$(EXECUTABLE)_$(CLIENT): $(CLIENT_OBJS)
	@echo "[ LD ]    "$@
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OUTDIR)/%.o: %.c
	@echo "[ CC ]    "$@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MF $(patsubst %.o,%.d,$@) -c $(INCLUDES) $< -o $@

.PHONY: clean
clean:
	@rm -rf $(OUTDIR)/*
	@echo "Removing All Object Files"

dbg: $(EXECUTABLE)_$(SERVER)
	$(CROSS_COMPILE)gdb -x $(TOOL)/gdbscript

-include $(DEP)
