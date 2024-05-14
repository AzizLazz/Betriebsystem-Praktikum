ENTRY(_start)
SECTIONS 
{

	. = 0x100000;
	.init : { *(.init) }
	.kernelcode : {
		*libe*.a:*(.text)
		build/arch/*.o(.text)
		build/kernel/*.o(.text)
	}

	. = 0x200000;
	.kerneldatareadonly : {
		*libe*.a:*(.rodata .rodata.*)
		build/arch/*.o(.rodata .rodata.*)
		build/kernel/*.o(.rodata .rodata.*)
	}

	. = 0x300000;
	.kerneldata : {
		*libe*.a:*(.data)
		build/arch/*.o(.data)
		build/kernel/*.o(.data)
	}
	.kernelbss : {
		*libe*.a:*(.bss)
		build/arch/*.o(.bss)
		build/kernel/*.o(.bss)
	}

	. = 0x500000;
	.usercode : {
		build/user/*.o(.text)
	}

	. = 0x600000;
	.userdatareadonly : {
		build/user/*.o(.rodata .rodata.*)
	}

	. = 0x700000;
	.userdata : {
		build/user/*.o(.data)
	}
	.userbss : {
		build/user/*.o(.bss)
	}

	. = 0x5F00000;
	.threadstacks : { *(.threadstacks) }

	. = 0x7F00000;
	.exeptionstacks : { *(.exeptionstacks) }

}	
