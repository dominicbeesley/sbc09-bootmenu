# sbc09-bootmenu
A boot menu and monitor for the sbc09 board

# Introduction

The boot loader contains a simple set of commands for manipulating memory and
loading Motorola SREC files.

Data can be loaded to a 64K RAM buffer (which is located at the top of 
physical RAM) and may be written to anywhere in the "physical" memory map.

It is also possible to read data from the physical memory map into the buffer
and inspect it.

# Starting the monitor

Currently the only way to run the monitor is to load it from BASIC. The
boot-menu.run.srec script should be copied to the terminal window when running
the mini-mos BASIC. This will bootstrap a small machine code srec-loader
which in turn will load the monitor and run it. W

When started in this manner the monitor runs in the the first 32K of logical
memory which is assumed to be mapped as the first 32K of physical memory.

The monitor also uses the top 80K of physical memory for workspace and buffer
memory.

# Commands

## ? : Query buffer

	?

This operation will show the range in the buffer that contains active data
(if any) and will perform a CRC16 check some over that range.

## D : Dump buffer

	D [<buffer address>]

Data in the buffer can be inspected by supplying a start address, if no
start address is specified then the bytes following the previous dump command
will be shown

Up to 256 bytes are displayed at a time, if a start address that is not a 
multiple of 16 is supplied the number of bytes shown is rounded down.

## E : Erase Chip / Sector

	E !|<phys address>[+<len>|<end address>]

The EEPROM will be erased in its entirety (!) or the sectors which in the
range specified will be erased. If only a single address is specified then
the sector containing that address will be erased.


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

# Examples


# To erase and re-load the boot ROMs at 00 0000 - 00 7FFF.

## Assuming a 39xxxx Flash with 4K sectors

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

## Assuming a 29xxxx Flash with 64K sectors

1) Clear the buffer

	C

2) Read the existing data at 00 8000 - 00 FFFF

	R 008000 0+4000

Note: this places the data at the start of the buffer, later we will use the
wrap feature to write the data back in the correct order

3) Erase the 64K sector

	E 0

4) Read the SREC data to buffer (this assumes the ROM is based at 8000)

	S1208000xxxxxx ...
	S120 ......
	S120FFE0 ...

5) Check the CRC
	
	?

6) Write to the EEPROM

	W 0 8000+8000

Note: this will again wrap around but this time the data will be written
back with the loaded ROMS at 00 0000 and the retained data from the EEPROM
at 00 8000



