#
# Kurzanleitung
# =============
#
# +++ General Targets +++
#
# make                  -- siehe -> make all
#
# make all              -- Baut kernel.elf und generiert einen dump
#
# make install          -- Baut kernel.img und transferiert ihn auf den Server.
#                          Das Board holt sich diesen Kernel beim nächsten Reset.
#
# make qemu             -- Baut den Kernel und führt ihn unter QEMU aus
#
# make qemu_debug       -- Baut den Kernel und führt ihn unter QEMU mit debug
#                          Optionen aus. Zum debuggen in einem zweiten Terminal
#                          folgendes ausführen:
#                          $ arm-none-eabi-gdb build/kernel.elf
#                          $ target remote localhost:1234
# make qemu_trace       -- wie qemu, gibt aber zusätzlich bei bestimmten MMIO-Zugriffen
#                          Meldungen aus
# make gdb			    -- Startet gdb und verbindet sich mit qemu
#
# make gdb_window 	    -- Führt `make gdb` in einem neuen terminal aus
#
# make debug 		    -- Führt qemu_debug gdb_window aus
#
# make clean            -- Löscht alle erzeugten Dateien.
#
# make lint             -- Führt den Linter aus. Dies _kann_ Programmierfehler finden
#
# make format           -- Formatiert alle .c und .h Dateien im Stil von kernel.org
#
# make submission       -- Verpackt alle Dateien die von git getrackt sind
#                          Hierbei wird immer der Branch genutzt, der unter
#                          SUBMISSION_BRANCH eingetragen ist.
#
# make home             -- kopiert das fertige image nach $TFTP_PATH, für die
#                          Arbeit zuhause einfach den Pfad eintragen
#
# +++ Single File Targets +++
#
# make kernel           -- Baut kernel.elf im build Ordner
#
# make kernel.bin       -- Baut kernel.bin im build Ordner
#
# make kernel.img       -- Baut kernel.img im build Ordner
#
# make kernel_only      -- Baut den kernel aber nur mit den Dateien aus SRC
#                          Zum testen, ob der Kernel unabhängig kompiliert
#
# make user_only        -- Baut nur die User Dateien aus USRC
#                          Zum testen, ob Userland unabhängig kompiliert

# Binäre Lsg (falls verwendet, ansonsten leer lassen)
BIN_LSG = e4

# Hier eure source files hinzufügen
SRC = arch/cpu/entry.S kernel/start.c arch/bsp/yellow_led.c arch/cpu/mmu_init.c arch/cpu/mmu_asm.S

# Hier separate user source files hinzufügen
USRC = user/main.c user/syscalls.S

# Hier können eigene GCC flags mit angegeben werden.
# Die vorgegebenen Flags können weiter unten gefunden werden unter
# CFLAGS_ALL und dürfen nicht verändert werden
# Bsp: CFLAGS = -Wpedantic -Werror -O2
CFLAGS = -Iinclude -Wall -Wextra -ffreestanding -mcpu=cortex-a7 -O2 -ggdb

# Hier den git branch angeben, der für die Abgabe benutzt werden soll
SUBMISSION_BRANCH = master

# Wenn ihr zuhause auf der Hardware arbeitet, hier das TFTP-Verzeichnis eintragen
TFTP_PATH = /srv/tftp

# Standardinstallationspunkt der Toolchain
PREFIX ?= $(HOME)/arm

# +------------------------------------+
# |                                    |
# |   Ab hier nichts mehr angepasst!   |
# |                                    |
# +------------------------------------+

# Source files, die fürs Testen einkompiliert werden.
TSRC =
SRC += $(TSRC)

# build directory
BUILD_DIR = build

# separate kernel files and add build prefix
KSRC_C = $(filter %.c, $(SRC))
KSRC_S = $(filter %.S, $(SRC))
KOBJ_C = $(addprefix $(BUILD_DIR)/,$(KSRC_C:%.c=%.o))
KOBJ_S = $(addprefix $(BUILD_DIR)/,$(KSRC_S:%.S=%.o))
KOBJ =  $(KOBJ_C) $(KOBJ_S)

# separate user files and add build prefix
USRC_C = $(filter %.c, $(USRC))
USRC_S = $(filter %.S, $(USRC))
UOBJ_C = $(addprefix $(BUILD_DIR)/,$(USRC_C:%.c=%.o))
UOBJ_S = $(addprefix $(BUILD_DIR)/,$(USRC_S:%.S=%.o))
UOBJ =  $(UOBJ_C) $(UOBJ_S)

# accumulate
OBJ_C = $(KOBJ_C) $(UOBJ_C)
OBJ_S = $(KOBJ_S) $(UOBJ_S)
OBJ = $(KOBJ) $(UOBJ)

# all header files
HEDAER_FILES = $$(find . -name '*.h')

# auto generated dep files
DEP = $(OBJ:.o=.d)

# linker script
LSCRIPT = kernel.lds

# static lib location for binary solution
BIN_LSG_DIR = ./
BIN_LSG_FILE = $(BIN_LSG_DIR)/lib$(BIN_LSG).a

# toolchain
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
IMG = mkimage
QEMU = qemu-system-arm
GDB = arm-none-eabi-gdb
LINT = cppcheck
FORMAT = clang-format

# configuration
CFLAGS_ALL = -Wall -Wextra -ggdb -ffreestanding -mcpu=cortex-a7 $(CFLAGS)
CPPFLAGS = -Iinclude
LDFLAGS = -T$(LSCRIPT) -nostdlib
ifneq ($(BIN_LSG), )
	LDFLAGS += -L$(BIN_LSG_DIR) -l$(BIN_LSG)
endif
OBJCOPYFLAGS = -Obinary -S --set-section-flags .bss=contents,alloc,load,data
OBJDUMPFLAGS = -dfzh
IMGFLAGS = -A arm -T standalone -C none -a 0x8000
QEMUFLAGS = -M raspi2b -nographic
LINTFLAGS = $(KSRC_C) $(USRC_C) $(HEDAER_FILES) --inline-suppr -Iinclude
ifdef POOL_PC
	FORMATFLAGS = --style="file:$(POOL_TOOLS_PATH)/share/clang-format/.clang-format" -i
else
	FORMATFLAGS = --style="file:$(PREFIX)/share/clang-format/.clang-format" -i
endif

# Regeln
.PHONY: all

# default target
all: kernel dump

# include auto generated targets
-include $(DEP)

# single file targets
$(OBJ_S):$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS_ALL) -MMD -MP -o $@ -c $<

$(OBJ_C):$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS_ALL) -MMD -MP -o $@ -c $<

$(BUILD_DIR)/kernel.elf: $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(BUILD_DIR)/kernel.img: $(BUILD_DIR)/kernel.bin
	$(IMG) $(IMGFLAGS) -d $< $@

$(BUILD_DIR)/kernel_only.elf: $(KOBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/user_only.elf: $(UOBJ)
	$(LD) -o $@ $^

$(BUILD_DIR)/kernel_dump.s: $(BUILD_DIR)/kernel.elf
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

# aliases
.PHONY: dump kernel kernel.bin kernel.img kernel_only user_only
dump: $(BUILD_DIR)/kernel_dump.s
kernel: lint $(BUILD_DIR)/kernel.elf
kernel.bin: $(BUILD_DIR)/kernel.bin
kernel.img: $(BUILD_DIR)/kernel.img
kernel_only: lint $(BUILD_DIR)/kernel_only.elf
user_only: lint $(BUILD_DIR)/user_only.elf

# general targets
.PHONY: install home qemu qemu_debug clean submission
install: lint $(BUILD_DIR)/kernel.img
	arm-install-image $(BUILD_DIR)/kernel.img

home: lint $(BUILD_DIR)/kernel.img
	cp -v $(BUILD_DIR)/kernel.img $(TFTP_PATH)

qemu: lint $(BUILD_DIR)/kernel.elf
	$(QEMU) $(QEMUFLAGS) -kernel $(BUILD_DIR)/kernel.elf

qemu_debug: lint $(BUILD_DIR)/kernel.elf
	$(QEMU) $(QEMUFLAGS) -s -S -kernel $(BUILD_DIR)/kernel.elf

qemu_trace: $(BUILD_DIR)/kernel.elf
	$(QEMU) $(QEMUFLAGS) -d "trace:bcm283*,trace:pl011*" -kernel $<

gdb: $(BUILD_DIR)/kernel.elf
	$(GDB) --ex="target remote localhost:1234" $<

gdb_window: $(BUILD_DIR)/kernel.elf
ifeq (, $(shell which x-terminal-emulator 2>/dev/null))
	@which $(TERM) || { echo "TERM not set properly, please do so, e.g. with 'export TERM=xterm'"; exit 1; }
	$(TERM) -e bash -c 'make gdb && killall $(QEMU)' &
else
	x-terminal-emulator -e "make gdb; killall $(QEMU)" &
endif

debug: gdb_window qemu_debug

lint:
	$(LINT) $(LINTFLAGS)

format:
	@$(FORMAT) $(FORMATFLAGS) $$(find . -name '*.c' -o -name '*.h')
	@echo "Finished reformatting code."

clean:
	rm -rf $(BUILD_DIR)

CURR_BRANCH = $(shell git branch --show-current 2> /dev/null)
STATUS_BRANCH = "$(shell git status --porcelain 2> /dev/null)"

submission:
ifneq ($(CURR_BRANCH), $(SUBMISSION_BRANCH))
	@echo "Refusing to create submission!"
	@echo "Current branch does not match submission branch."
	@echo " > Currently on branch: '$(CURR_BRANCH)'"
	@echo " > But registered submission branch is: '$(SUBMISSION_BRANCH)'"
	@echo "Please change branch or fix submission branch."
	@exit 1
endif
ifneq ($(STATUS_BRANCH), "")
	@echo "Refusing to create submission!"
	@echo "Submission branch is dirty or contains untracked files."
	@echo ""
	@echo "Please commit changes and add non essential files to '.gitignore'."
	@echo "Use 'git status' for a detailed list."
	@echo ""
	@exit 1
endif
	git archive --format=tar.gz -o submission.tar.gz $(SUBMISSION_BRANCH)
	@echo Successfully created submission.tar.gz
	@echo We recommend to test the created archive before submitting it.
