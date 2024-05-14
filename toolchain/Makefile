# Makefile zum Toolchain bauen
# ============================
#
# Alles bauen (Toolchain (binutils, GCC, clang-format, cppcheck) + QEMU + GDB + UBOOT (image, mkimage)):
#	make all
#
# Einzelne Dinge bauen:
# 	make toolchain
# 	make qemu
# 	make gdb
# 	make uboot
# 	make mkimage
#	make cppcheck
#	make clang-format
#
# Aufräumen:
# 	make clean		-- Build-Verzeichnisse löschen
# 	make mrproper		-- wie clean, zusätzlich entpackte Quellen löschen
# 	make distclean		-- wie mrproper, zusätzlich die heruntergeladenen Archive löschen
#
# Um die Toolchain woanders zu installieren (anstatt $HOME/arm), muss die
# Umgebungsvariable PREFIX entsprechend gesetzt sein. Beispielsweise:
# 	PREFIX=/arm make all
#
# Um sich die ganzen Kommandos anzuschauen (quasi das HOWTO in Kompaktform),
# funktioniert z. B. sowas wie:
# 	 make -n -B all | grep -v "^touch"
#

# Standardinstallationspunkt der Toolchain
PREFIX ?= $(HOME)/arm

# Falls das Kompilieren fehlschlägt, weil capstone.h nicht gefunden wird
# kann dieser Fix aktiviert werden indem die flag auf "true" gesetzt wird
# Stellt sicher, dass die capstone library vorhanden ist
QEMU_CAPSTONE_FIX ?= false

# Default-Target
all:

# Pool: Prüfe, ob alle Variablen für Pool vorhanden
ifdef POOL_PC
ifndef POOL_TOOLS_PATH
$(error POOL_TOOLS_PATH is not set, but must be set, if POOL_PC is defined)
endif
ifndef POOL_INSTALL_PATH
$(error POOL_INSTALL_PATH is not set, but must be set, if POOL_PC is defined)
endif
endif

#
# Bestandteile der Toolchain
#

FETCH_FROM = $(X-$*)_URL

BINUTILS = binutils-2.35
BINUTILS_SHA = 7d24660f87093670738e58bcc7b7b06f121c0fcb0ca8fc44368d675a5ef9cff7
BINUTILS_ARCHIVE = $(BINUTILS).tar.bz2
BINUTILS_URL = http://ftp.gnu.org/gnu/binutils
BINUTILS_CONFIGURE = ../$(BINUTILS)/configure --target=arm-none-eabi --prefix=$(PREFIX) --enable-interwork --disable-nls

GCC_VERSION = 13.2.0
GCC = gcc-$(GCC_VERSION)
GCC_SHA = 8cb4be3796651976f94b9356fa08d833524f62420d6292c5033a9a26af315078
GCC_ARCHIVE = $(GCC).tar.gz
GCC_URL = ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/$(GCC)
GCC_CONFIGURE = ../$(GCC)/configure --target=arm-none-eabi --prefix=$(PREFIX) --enable-interwork --enable-languages="c" --with-newlib --without-headers --disable-nls

NEWLIB = newlib-3.3.0
NEWLIB_SHA = 58dd9e3eaedf519360d92d84205c3deef0b3fc286685d1c562e245914ef72c66
NEWLIB_ARCHIVE = $(NEWLIB).tar.gz
NEWLIB_URL = ftp://sourceware.org/pub/newlib
NEWLIB_CONFIGURE = ../$(NEWLIB)/configure --target=arm-none-eabi --prefix=$(PREFIX) --enable-interwork --disable-nls

UBOOT = u-boot-2020.10
UBOOT_SHA = 0d481bbdc05c0ee74908ec2f56a6daa53166cc6a78a0e4fac2ac5d025770a622
UBOOT_ARCHIVE = $(UBOOT).tar.bz2
UBOOT_URL = ftp://ftp.denx.de/pub/u-boot

QEMU = qemu-8.2.0
QEMU_SHA = bf00d2fa12010df8b0ade93371def58e632cb32a6bfdc5f5a0ff8e6a1fb1bf32
QEMU_ARCHIVE = $(QEMU).tar.xz
QEMU_URL = http://download.qemu.org/
QEMU_CONFIGURE = ../$(QEMU)/configure --target-list="arm-softmmu" --prefix=$(PREFIX)
qemu-pool: QEMU_CONFIGURE += --disable-sdl --disable-bluez --disable-vnc --disable-gtk

GDB = gdb-11.2
GDB_SHA = b558b66084835e43b6361f60d60d314c487447419cdf53adf83a87020c367290
GDB_ARCHIVE = $(GDB).tar.gz
GDB_URL = http://ftp.gnu.org/gnu/gdb
GDB_CONFIGURE = ../$(GDB)/configure --target=arm-none-eabi --prefix=$(PREFIX) --enable-interwork --disable-nls --with-expat

CLANGTOOLS_VERSION = 16.0.6
CLANGTOOLS = llvm-project-$(CLANGTOOLS_VERSION).src
CLANGTOOLS_SHA = ce5e71081d17ce9e86d7cbcfa28c4b04b9300f8fb7e78422b1feb6bc52c3028e
CLANGTOOLS_ARCHIVE = $(CLANGTOOLS).tar.xz
CLANGTOOLS_URL = https://github.com/llvm/llvm-project/releases/download/llvmorg-$(CLANGTOOLS_VERSION)
CLANGTOOLS_CMAKE_FLAGS = -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=MinSizeRel -G "Unix Makefiles" ../$(CLANGTOOLS)/llvm

CPPCHECK_VERSION = 2.12.0
CPPCHECK = cppcheck-$(CPPCHECK_VERSION)
CPPCHECK_SHA = 7d67776118aee9a4f0214f993a4baa4a168b2dbb10c14b6ec5baf2ca147565b8
CPPCHECK_ARCHIVE = $(CPPCHECK).tar.gz
CPPCHECK_URL = https://github.com/danmar/cppcheck/archive/$(CPPCHECK_VERSION).tar.gz
CPPCHECK_CONFIG_FILE = https://raw.githubusercontent.com/danmar/cppcheck/f56677a99d365a68c0bc069f38365ed7d8c471f5/cfg/std.cfg
CPPCHECK_COMPILER_FLAGS = MATCHCOMPILER=yes HAVE_RULES=no CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function"
# CPPCHECK does not search in dynamic directories for configs. They must be defined at compile-time
ifdef POOL_PC
	CPPCHECK_COMPILER_FLAGS += FILESDIR=$(POOL_TOOLS_PATH)/share/cppcheck
else
	CPPCHECK_COMPILER_FLAGS += FILESDIR=$(PREFIX)/share/cppcheck
endif


# Einzelteile zusammensammeln
PACKAGES = $(PACKAGES_BUILD) $(PACKAGES_EXTRA)
PACKAGES_BUILD = BINUTILS GCC NEWLIB UBOOT QEMU GDB CLANGTOOLS CPPCHECK
PACKAGES_EXTRA =

ARCHIVES = $(foreach target,$(PACKAGES),$($(target)_ARCHIVE))

# ermöglicht Reverse-Lookup (so dass z. B. $(X-gdb-7.6) nach "GDB" umgesetzt wird)
$(foreach target,$(PACKAGES),$(eval X-$($(target)) = $(target)))


#
# Globale Einstellungen
#
OS := $(shell uname)
ifeq ($(OS),Darwin)
	THREADS := $(word 2, $(shell sysctl hw.ncpu))
else
	THREADS := $(shell grep "^processor" /proc/cpuinfo | wc -l)
endif
export PATH := $(PREFIX)/bin:$(PATH)


#
# Allgemeine Regeln
#

.PHONY: all all-pool toolchain tools gdb mkimage qemu qemu-pool
all: toolchain tools gdb qemu uboot mkimage
all-pool: toolchain tools gdb mkimage qemu-pool
toolchain: $(GCC)-build/.installed clang-format cppcheck
gdb: $(GDB)-build/.installed
uboot: $(UBOOT)-build/.installed
mkimage: $(UBOOT)-build/.mki_installed
qemu: $(QEMU)-build/.installed
qemu-pool: qemu
clang-format: $(CLANGTOOLS)-build/.installed
cppcheck: $(CPPCHECK)/.installed

.PHONY: clean mrproper distclean
clean:
	rm -Rf $(foreach target,$(PACKAGES_BUILD),$($(target))-build) u-boot.bin
mrproper: clean
	rm -Rf $(foreach target,$(PACKAGES),$($(target)))
distclean: mrproper
	rm -Rf $(ARCHIVES)

define dep_check
	$(if $(shell which $(1)), @echo "$(1): yes", @$(error "No $(1) in PATH, please install!"))
endef

#
# Generelle Regeln
#
.SECONDARY:
%-build/.installed: %-build/.built
	make -C $*-build install
	touch $@

%-build/.built: %-build/.configured
	make -C $*-build -j $(THREADS)
	touch $@

%-build/.configured: %/.unpacked
	mkdir -p $*-build
	cd $*-build && $($(X-$*)_CONFIGURE)
	touch $@

%/.unpacked: %.tar.gz
	$(call dep_check,tar)
	tar xzf $<
	touch $@

%/.unpacked: %.tar.bz2
	$(call dep_check,tar)
	tar xjf $<
	touch $@

%/.unpacked: %.tar.xz
	$(call dep_check,tar)
	tar xJf $<
	touch $@

.DELETE_ON_ERROR: $(filter %.tar.bz2,$(ARCHIVES))
$(filter %.tar.bz2,$(ARCHIVES)): %.tar.bz2:
	$(call dep_check,wget)
	$(call dep_check,sha256sum)
	wget -c --no-check-certificate $($(FETCH_FROM))/$($(X-$*)_ARCHIVE) -O $@
	GET_SHA=$$(sha256sum $@ | cut -d ' ' -f1); \
	if [ $$GET_SHA = "$($(X-$*)_SHA)" ]; \
		then echo "Checksum match."; \
		else echo "Checksum mismatch, got $${GET_SHA}"; exit 1; \
	fi

.DELETE_ON_ERROR: $(filter %.tar.gz,$(ARCHIVES))
#cppcheck needs to be renamed, handled below
$(filter-out $(CPPCHECK_ARCHIVE), $(filter %.tar.gz,$(ARCHIVES))): %.tar.gz:
	$(call dep_check,wget)
	$(call dep_check,sha256sum)
	wget -c --no-check-certificate $($(FETCH_FROM))/$($(X-$*)_ARCHIVE) -O $@
	GET_SHA=$$(sha256sum $@ | cut -d ' ' -f1); \
	if [ $$GET_SHA = "$($(X-$*)_SHA)" ]; \
		then echo "Checksum match."; \
		else echo "Checksum mismatch, got $${GET_SHA}"; exit 1; \
	fi


.DELETE_ON_ERROR: $(filter %.tar.xz,$(ARCHIVES))
$(filter %.tar.xz,$(ARCHIVES)): %.tar.xz:
	$(call dep_check,wget)
	$(call dep_check,sha256sum)
	wget -c --no-check-certificate $($(FETCH_FROM))/$($(X-$*)_ARCHIVE) -O $@
	GET_SHA=$$(sha256sum $@ | cut -d ' ' -f1); \
	if [ $$GET_SHA = "$($(X-$*)_SHA)" ]; \
		then echo "Checksum match."; \
		else echo "Checksum mismatch, got $${GET_SHA}"; exit 1; \
	fi

#
# Spezielle Regeln
#

# GCC braucht binutils
$(GCC)-build/.configured: $(BINUTILS)-build/.installed

# minimale GCC (wird benötigt, um Newlib bauen zu können, um wiederum GCC fertig zu bauen)
$(GCC)-build/.built-gcc: $(GCC)-build/.configured
	make -C $(GCC)-build -j $(THREADS) all-gcc
	touch $@

$(GCC)-build/.installed-gcc: $(GCC)-build/.built-gcc
	make -C $(GCC)-build install-gcc
	touch $@

# Newlib braucht minimale GCC
$(NEWLIB)-build/.configured: $(GCC)-build/.installed-gcc

# vollständige GCC braucht Newlib
$(GCC)-build/.built: $(NEWLIB)-build/.installed

# uboot/mkimage hat kein "normales" Makefile und teilen sich das archive
$(UBOOT)-build/.mki_installed: $(UBOOT)-build/.mki_built
	cp $(UBOOT)-build/tools/mkimage $(PREFIX)/bin
	touch $@

$(UBOOT)-build/.installed: $(UBOOT)-build/.built
	cp $(UBOOT)-build/u-boot.bin ./
	touch $@

$(UBOOT)-build/.configured: $(UBOOT)/.unpacked
	cp -r $(UBOOT) $(UBOOT)-build
	make -C $(UBOOT)-build rpi_2_defconfig
	touch $@

$(UBOOT)-build/.mki_built: $(UBOOT)-build/.configured
	make -C $(UBOOT)-build -j $(THREADS) tools-only
	touch $@

$(UBOOT)-build/.built: $(UBOOT)-build/.configured
	CROSS_COMPILE=$(PREFIX)/bin/arm-none-eabi- make -C $(UBOOT)-build -j $(THREADS) u-boot.bin
	touch $@

# Spezielles target für qemu patches
$(QEMU)/.patched: $(QEMU)/.unpacked
ifeq ($(QEMU_CAPSTONE_FIX),true)
	$(call dep_check,find)
	echo "Applying capstone fix"
	find ./$(QEMU) -type f -exec sed -i 's/#include <capstone.h>/#include <capstone\/capstone.h>/g' {} +
endif
	$(call dep_check,patch)
	OUT=$$(patch -d ${QEMU} -p0 -N -r - < qemu-led.patch || echo ERROR); (echo $$OUT | grep -vq ERROR) || ((echo $$OUT | grep -q "Reversed (or previously applied) patch detected!") || (echo $$OUT && false))
	OUT=$$(patch -d ${QEMU} -p0 -N -r - < qemu-unaligned.patch || echo ERROR); (echo $$OUT | grep -vq ERROR) || ((echo $$OUT | grep -q "Reversed (or previously applied) patch detected!") || (echo $$OUT && false))
	OUT=$$(patch -d ${QEMU} -p1 -N -r - < qemu-fix-keymap.patch || echo ERROR); (echo $$OUT | grep -vq ERROR) || ((echo $$OUT | grep -q "Reversed (or previously applied) patch detected!") || (echo $$OUT && false))
	touch $@

# Config von qemu erst nach patches
$(QEMU)-build/.configured: $(QEMU)/.patched

# Spezielle Targets für Formatting und Linting. Überschreiben generelle Regeln von oben.
$(CLANGTOOLS)-build/.installed: $(CLANGTOOLS)-build/.built
	@mkdir -p $(PREFIX)/bin  # falls es noch kein bin-dir gibt
	cp $(CLANGTOOLS)-build/bin/clang-format $(PREFIX)/bin
	touch $@

$(CLANGTOOLS)-build/.built: $(CLANGTOOLS)-build/.configured
	cd $(CLANGTOOLS)-build && make -j $(THREADS) clang-format
	touch $@

$(CLANGTOOLS)-build/.configured: $(CLANGTOOLS)/.unpacked
	$(call dep_check,cmake)
	mkdir -p $(CLANGTOOLS)-build
	cd $(CLANGTOOLS)-build && cmake $(CLANGTOOLS_CMAKE_FLAGS)
	touch $@

# Spezielle Regeln für cppcheck
$(CPPCHECK)/.installed: $(CPPCHECK)/.built
ifdef POOL_PC
	@mkdir -p $(POOL_INSTALL_PATH)/bin
	cp $(CPPCHECK)/cppcheck $(POOL_INSTALL_PATH)/bin
	# Standard-config für cppcheck laden und installieren
	mkdir -p $(POOL_INSTALL_PATH)/share/cppcheck/cfg
	cd $(POOL_INSTALL_PATH)/share/cppcheck/cfg/ && wget -c --no-check-certificate $(CPPCHECK_CONFIG_FILE)
else
	@mkdir -p $(PREFIX)/bin
	cp $(CPPCHECK)/cppcheck $(PREFIX)/bin
	mkdir -p $(PREFIX)/share/cppcheck/cfg
	cd $(PREFIX)/share/cppcheck/cfg/ && wget -c --no-check-certificate $(CPPCHECK_CONFIG_FILE)
endif
	touch $@

$(CPPCHECK)/.built: $(CPPCHECK)/.unpacked
	mkdir -p $(CPPCHECK)
	cd $(CPPCHECK) && make -j $(THREADS) $(CPPCHECK_COMPILER_FLAGS)
	touch $@

$(CPPCHECK)/.unpacked: $(CPPCHECK_ARCHIVE)
	tar xf $(CPPCHECK).tar.gz
	touch $@

# Downloadregel überschreiben, um Archiv umzubenennen
$(CPPCHECK_ARCHIVE):
	$(call dep_check,wget)
	$(call dep_check,sha256sum)
	wget -c --no-check-certificate $(CPPCHECK_URL) -O $(CPPCHECK_ARCHIVE)
	GET_SHA=$$(sha256sum $(CPPCHECK).tar.gz | cut -d ' ' -f1); \
	if [ $$GET_SHA = "$(CPPCHECK_SHA)" ]; \
		then echo "Checksum match."; \
		else echo "Checksum mismatch, got $${GET_SHA}"; exit 1; \
	fi

$(GCC)/.unpacked: $(GCC_ARCHIVE)
	$(call dep_check,tar)
	tar xzf $<
	cd $(GCC); ./contrib/download_prerequisites
ifeq ($(OS),Darwin)
	$(call dep_check,patch)
	OUT=$$(patch -d $(GCC) -p2 -N -r - < gcc-macos.patch || echo ERROR); (echo $$OUT | grep -vq ERROR) || ((echo $$OUT | grep -q "Reversed (or previously applied) patch detected!") || (echo $$OUT && false))
endif
	touch $@
