# sbc09-bootmenu
A boot menu and monitor for the sbc09 board

# Introduction

The boot loader contains a simple set of commands for manipulating memory and
loading Motorola SREC files.

Data can be loaded to a 64K RAM buffer (which is located at the top of 
physical RAM) and may be written to anywhere in the "physical" memory map.

It is also possible to read data from the physical memory map into the buffer
and inspect it.

# Building 

Run the top-level makefile 

	make all

This produces several outputs:


- build/boot-menu-rom.bin - this is suitable for blowing to an EEPROM at offset 
  4000 with eeprom software that doesn't understand SREC files (i.e. XGpro)
- build/boot-menu-rom.srec - an srec version of the above that can be loaded
  using the monitor and blow to rom as described below
- build/boot-menu.run.srec - this is a self-bootstrapping version of the loader
  that can be run from BASIC. This bootstraps itself with a small BASIC program
  and runs the monitor from memory.

# Line endings

The monitor program is designed to accept BBC-BASIC style input and line-endings
i.e. each line should end with \<CR\>. However, it will also accept and silently
discard \<LF\>

The monitor program terminates output lines with a BBC-Micro like \n\r line 
ending.

# Starting the monitor

If you are running a version of BASIC you may run the monitor as follows

The boot-menu.run.srec script should be copied to the terminal window when
at the BASIC prompt. This will bootstrap a small machine code srec-loader
which in turn will load the monitor and run it. 

When running in this manner the monitor will be load at logical 0E00-3FFF
and will then self-relocate to the topmost block of RAM. The top most block
of RAM will then be mapped to the C000-FFFF slot and the monitor will run
from there. 

The monitor uses the top 80K of RAM for code, workspace and buffer memory.

# Commands

## ? : Query buffer

	?

This operation will show the range in the buffer that contains active data
(if any) and will perform a CRC16 check some over that range.

## . : Catalogue operator

	.

The Flash ROM and RAM excluding the bottom 32K and top 80K will be scanned in 
16K chunks for "ROM"s that look like:

- BBC Micro Sideways ROM - byte offset 7 points at "(C)" copyright string
- SBC09MOS - offset 3802..380B contains string "SBC09MOS"

TODO: Show examples

## ! : Boot image

	! <phys addr> [W|!]+

This command will call an SBC09MOS image via its boot entry point. Images that
can be booted in this way will have a line starting "MOS" in the catalogue i.e.:

	MOS  : 00C000 : !F800   :        Midi-mos test

The entry point for this rom is indicated in the third field typing !F800 will 
boot this image.

The W flag can be added for a "warm" boot i.e. the MOS will not clear all memory
if this flag is supplied and OLD will work. This should only be used if the image
(or a similar one) has already been booted since the system was powered up.

The ! flag can be added to automatically warm boot this image on subsequent resets
this will remain in force until the next power cycle.

The "boot" command does the following:

- calculate the 16k block number of the phys addr (B)
- calculate the offset of phys addr within 16K (X)
- maps the lowest block of RAM in at 8000-BFFF
- copies the rest of the process to this block at offset $100
- maps the lowest two blocks of RAM in 0000-7FFF
- jumps to 0100 to execute the rest of code just copied
- maps the block containing the phys address in to C000-FFFF
- maps the block preceding that to 8000-BFFF
- sets B to the block number mapped in to C000
- sets A to 0 for cold boot, 1 for warm start
- jump indirect to X

In this way the ! command can also be used to boot via the normal reset vector i.e.

	!377FE

would boot via the normal reset vector

## D : Dump buffer

	D [<buffer address>]

Data in the buffer can be inspected by supplying a start address, if no
start address is specified then the bytes following the previous dump command
will be shown

Up to 256 bytes are displayed at a time, if a start address that is not a 
multiple of 16 is supplied the number of bytes shown is rounded down.

## E : Erase Chip / Sector

	E !|<phys address>[+<len>|<end address>]

The EEPROM will be erased in its entirety (!) or the sectors which are in the
range specified will be erased. 

## S : SREC load

Any line beginning with "S" is treated as a Motorola SREC record and will be
parsed as such. S1 lines will additionally be written to the buffer memory
at the address contained in the SREC.

## R : Read physical memory to buffer

	R <phys address> <buffer address>[+<len>|<buffer end address>]

The bytes at phys address are read to the buffer. 

Lengths > 64K are truncated
If the buffer would overflow then the buffer is wrapped

## W : Write buffer to physical memory

	W <phys address> <buffer address>[+<len>|<buffer end address>]

The specified bytes in the buffer are written to the memory, if the phys 
address is in the range 00 0000 - 0F FFFF then the change will be written
permanently to the EEPROM. If the EEPROM has not been previously erased
(either completely or the relevant sectors) then the write may fail. 

Lengths > 64K are truncated

If the buffer would overflow then the buffer is wrapped

NOTE: The write command does NOT erase the EEPROM automatically
all bytes to be written must contain FF before writing or the
resulting data will be undefined. Use the E command before writing. This is
a feature not a bug to cope with the case where not a full sector will be
written at a time.

NOTE: The current version of the W command does not verify writes.

# Examples

## Install boot menu as the default at reset

!!! You should check that everything appears to have worked before resetting 
the machine as errors in this process will be unrecoverable and require the
Flash Chip to be removed and reprogrammed!!!

First follow the instructions in the [Starting the Monitor](#startingthemonitor)
section to load the monitor then

	E 4000+4000

This will clear the flash ROM at 00 4000.

Note: if your sector Flash's sector size is greater than 4000 this may have
unintended consequences. You may need to follow the instructions in 
[Preserving adjacent Flash](#preservingadjacentflash)

Clear the buffer

	C

Paste the boot-menu-rom.srec file into the terminal this will load in the
buffer region C000-FFFF

Write the buffer to the hard-reset flash sector at 00 4000

	W 4000 C000+4000


## To erase and re-load the BASIC ROMs at 00 0000 - 00 7FFF.

### Assuming a 39xxxx Flash with 4K sectors

1) Clear the buffer

	C

2) Erase the sectors

	E 0+8000

3) Read the SREC data to buffer (this assumes the ROM is based at 8000)

	S1208000xxxxxx ...
	S120 ......
	S120FFE0 ...

4) Check the CRC
	
	?

5) Write to the EEPROM

	W 0 8000+4000

### Assuming a 29xxxx Flash with 64K sectors

1) Clear the buffer

	C

2) Read the existing data at 00 8000 - 00 FFFF

	R 008000 0+8000

Note: this places the data at the start of the buffer, later we will use the
wrap feature to write the data back in the correct order

3) Erase the 64K sector

	E 0+10000

Note: this will also erase any ROM's loaded at 

4) Read the SREC data to buffer (this assumes the ROM is based at 8000)

	S1208000xxxxxx ...
	S120 ......
	S120FFE0 ...

5) Check the CRC
	
	?

6) Write to the EEPROM

	W 0 8000+10000

Note: this will again wrap around but this time the data will be written
back with the loaded ROMS at 00 0000 and the retained data from the EEPROM
at 00 8000

