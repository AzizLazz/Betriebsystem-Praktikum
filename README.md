# Betriebsystem-Praktikum


Got it! Here's the text:

---

**How-To Compile**

It is recommended to read this README in its entirety before starting to build/install the toolchain.

**Dependencies**

In general, the usual software components for package building are required. If not already present, all of these can be installed via the package manager of the respective distribution:

- For Debian based systems:
  - `build-essential`
- For Arch based systems:
  - `base-devel`
- For Mac OS:
  - `wget`
  - `coreutils`
  - `pkg-config`
  - `glib`
  - `pixman`

Additionally, for QEMU, `ninja` is required:

- For Debian based systems:
  - `ninja-build`
- For Arch based systems:
  - `ninja`
- For Mac OS:
  - `ninja`

For `clang-format`, the CMAKE build system is needed:

- For Debian based systems:
  - `cmake`
- For Arch based systems:
  - `cmake`
- For Mac OS:
  - `cmake`

**Toolchain**

The Makefile can build/install the following:

- Command Tool:
  - `make toolchain`: arm-none-eabi-gcc + binutils + linters
  - `make gdb`: arm-none-eabi-gdb
  - `make qemu`: qemu-system-arm
  - `make mkimage`: mkimage
  - `make uboot`: uboot.img
  - `make clang-format`: clang-format
  - `make cppcheck`: cppcheck

`mkimage` and `uboot.img` are only required if you want to work on the Pi at home.
