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

# Optimizations
CFLAGS += -O3

#files
SRCDIR = src

INCDIR = inc

SERVER_SRC = $(addsuffix /$(SERVER).c,$(SRCDIR))
CLIENT_SRC = $(addsuffix /$(CLIENT).c,$(SRCDIR))

SRC += $(wildcard $(addsuffix /*.c,$(SRCDIR)))
SRC := $(filter-out $(SERVER_SRC), $(SRC))
SRC := $(filter-out $(CLIENT_SRC), $(SRC))

OBJS += $(addprefix $(OUTDIR)/,$(patsubst %.s,%.o,$(SRC:.c=.o)))

INCLUDES = $(addprefix -I,$(INCDIR))

DEP = $(OBJS:.o=.d)

#MAKDIR = mk
#MAK = $(wildcard $(MAKDIR)/*.mk)

#include $(MAK)

all: $(EXECUTABLE)_$(SERVER) $(EXECUTABLE)_$(CLIENT)

$(EXECUTABLE)_$(SERVER): $(OBJS) $(SERVER_SRC)
	@echo "[ LD ]    "$@
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(EXECUTABLE)_$(CLIENT): $(OBJS) $(CLIENT_SRC)
	@echo "[ LD ]    "$@
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OUTDIR)/%.o: %.c
	@echo "[ CC ]    "$@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MF $(patsubst %.o,%.d,$@) -c $(INCLUDES) $< -o $@

.PHONY: clean
clean:
	@rm -rf $(OUTDIR)/*
	@echo "Removing All Object Files"

dbg: $(EXECUTABLE)
	openocd -f board/stm32f429discovery.cfg >/dev/null & \
    echo $$! > $(OUTDIR)/openocd_pid && \
    $(CROSS_COMPILE)gdb -x $(TOOLDIR)/gdbscript && \
    cat $(OUTDIR)/openocd_pid |`xargs kill 2>/dev/null || test true` && \
    rm -f $(OUTDIR)/openocd_pid

-include $(DEP)
