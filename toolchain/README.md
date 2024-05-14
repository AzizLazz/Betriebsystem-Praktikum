# How-To Compile

Achtung: __Es empfiehlt sich dieses README erst ein mal vollständig zu lesen und dann mit dem bauen/installieren der Toolchain zu beginnen ;)__

## Abhängigkeiten
Allgemein werden die üblichen Softwarekomponenten zum Paketbau benötigt. Falls nicht eh schon vorhanden können diese alle über den Paketmanager der jeweiligen Distribution installiert werden:

- Debian based systems: `sudo apt install build-essential`
- Arch based systems: `sudo pacman -S base-devel`
- Mac OS: `brew install wget coreutils pkg-config glib pixman`

Für QEMU wird zusätzlich ninja benötigt:

- Debian based systems: `sudo apt install ninja-build`
- Arch bases systems: `sudo pacman -S ninja`
- Mac OS: `brew install ninja`

Für clang-format wird das Buildsystem CMAKE benötigt:

- Debian based systems: `sudo apt install cmake`
- Arch bases systems: `sudo pacman -S cmake`
- Mac OS: `brew install cmake`

## Toolchain

Die Makefile kann folgendes bauen/installieren:

| Command            | Tool                                   |
|--------------------|----------------------------------------|
| make toolchain     | arm-none-eabi-gcc + binutils + linters |
| make gdb           | arm-none-eabi-gdb                      |
| make qemu          | qemu-system-arm                        |
| make mkimage       | mkimage                                |
| make uboot         | uboot.img                              |
| make clang-format  | clang-format                           |
| make cppcheck      | cppcheck                               |


mkimage und uboot.img werden nur benötigt, falls ihr zu Hause auf dem Pi arbeiten wollt. Es können alle Tools mit `make all` installiert werden. Dies ist jedoch meist nicht notwendig und kann (wirklich) sehr lange dauern.

Falls bei dem Bauvorgang weitere Fehler auftreten, dass ein Tool oder eine Bibliothek fehlt, lohnt es sich in der Regel erst ein mal nach diesem Tool oder Bibliothek zu googlen (oder auch gerne jede andere Suchmaschine eurer Wahl). In den meisten fällen ist das Problem behoben, nachdem dieses mit eurem Paketmanager nachinstalliert wurde.

Standardmäßig werden alle Tools in eurem Home Verzeichnis in einem Ordner Namens "arm" (`$HOME/arm`) installiert. Dies könnt ihr anpassen, indem ihr die Environment Variable "PREFIX" auf den gewünschten Pfad setzt. Beispiel: `PREFIX=$HOME/path/to/dir make toolchain`.

Damit die frisch installierte Software auch gefunden wird muss die "PATH" Environment Variable angepasst werden. Dies kann zum Beispiel erreicht werden, indem folgende Zeile in eure ".profile" bzw. ".bash_profile" bzw. ".zshenv" (je nachdem was benutzt wird) im Home Verzeichnis eingefügt wird:

```sh
export PATH=$HOME/arm/bin:$PATH
```

Bei abweichendem Installationspfad dementsprechend anpassen.

### gcc + binutils

```sh
make toolchain
```

Hiermit werden die essentiellsten Tools gebaut, welche zum kompilieren, linken und analysieren benötigt werden. Es wird stark empfohlen diese über die hier mitgelieferte Makefile zu bauen, da diese auch als Referenz bei der Korrektur verwendet wird. Dadurch können Probleme bezüglich Versionsunterschiede bei der Korrektur vermieden werden.

Probleme wie "Bei mir kompiliert es aber" werden nicht berücksichtigt, falls nicht die hier mitgelieferte Referenz benutzt wird!

### gdb

```sh
make gdb
```

gdb muss nicht über die Makefile gebaut werden, da es nur zum Debuggen benutzt wird und weder den Bauprozess eures Images noch das Emulieren durch QEMU beeinflusst (außer man greift natürlich aktiv in den Ablauf ein durch gdb).

Die meisten Distributionen liefern über ihren Paketmanager entweder `arm-none-eabi-gdb` oder `gdb-multiarch`.

### QEMU

```sh
make qemu
```

Es ist absolut essentiell, dass QEMU über die Makefile gebaut wird! Wir mussten kleinere Patches an QEMU vornehmen um das Verhalten der Boards im Pool genauer zu simulieren. Die Patches werden automatisch vor dem Bauprozess angewendet. Ohne diesen Patches, können nicht alle Aufgaben vollständig bearbeitet werden.

### mkimage

```sh
make mkimage
```

Wird nur benötigt, falls von zu Hause an einem Raspi 2b gearbeitet wird. Konvertiert ein binäres Abbild des selbst gebauten Kernels in ein von uboot lesbares Image.

### uboot

```sh
make uboot
```

Wird nur benötigt, falls von zu Hause an einem Raspi 2b gearbeitet wird. Baut die Datei: "uboot.img". Ein einfacher bootloader für den Raspi 2b, der sich beim starten das aktuellste Image über tftp holt. gcc + binutils für arm werden benötigt (siehe oben) um "uboot.img" zu bauen.

## Cleanup

Nachdem alle notwendigen Tools installiert wurden kann mit Hilfe des Commands: `make distclean` alle temporären Dateien, welche nur zum bauen der Software benötigt werden, wieder gelöscht werden um Speicherplatz frei zu geben.
