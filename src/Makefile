BUILD=../build

export CROSS_AS=m6809-unknown-as
export CROSS_LD=m6809-unknown-ld
export CROSS_CC=m6809-unknown-gcc
export CROSS_CCOPTS=-std=c99 -c -Wall -Os -msoft-reg-count=0 -mfar-stack-param -mcode-section=.text -I .
# Workaround for gcc6809 bug - register copy propagation issue
#CROSS_CCOPTS += -fno-cprop-registers


OBJS=crt0 main uart stdlib stdlib_s stdio string ctype crc16 membuf memw srec flash boot rdline
B_OBJS=$(addprefix $(BUILD)/, $(addsuffix .o, $(OBJS)))

OBJS_R=crt0_rom main uart stdlib stdlib_s stdio string ctype crc16 membuf memw srec flash boot rdline
B_OBJS_R=$(addprefix $(BUILD)/, $(addsuffix .o, $(OBJS_R)))

X:=$(shell mkdir -p $(BUILD))

all: $(BUILD)/boot-menu.srec $(BUILD)/boot-menu-rom.srec

$(BUILD)/%.o: %.c
	$(CROSS_CC) $(CROSS_CCOPTS) -o $@ $<

$(BUILD)/%.o: %.s
	$(CROSS_AS) $(CROSS_ASOPTS) -o $@ $<

$(BUILD)/srec-2.bas:
	$(MAKE) -C ../basic all

$(BUILD)/boot-menu.srec: $(B_OBJS) $(BUILD)/srec-2.bas
	$(CROSS_LD) -o $@ -Map=$(patsubst %.srec,%.map,$@) --script=boot-menu.link --oformat=srec -L/usr/lib/gcc/m6809-unknown/4.6.4/ -lgcc $(filter %.o, $^)
	srec_cat $@ -offset -0xE00 -o $(basename $@).bin -binary
	dos2unix $@
	unix2mac $@
	cat $(BUILD)/srec-2.bas $@ > $(patsubst %.srec,%.run.srec, $@)

$(BUILD)/boot-menu-rom.srec: $(B_OBJS_R)
	$(CROSS_LD) -o $@ -Map=$(patsubst %.srec,%.map,$@) --script=boot-menu-rom.link --oformat=srec -L/usr/lib/gcc/m6809-unknown/4.6.4/ -lgcc $(filter %.o, $^)
	srec_cat $@ -fill 0xFF 0xC000 0x10000 -offset -0xC000 -o $(basename $@).bin -binary
	dos2unix $@
	unix2mac $@

clean:
	-rm $(BUILD)/boot-menu.run.srec
	-rm $(BUILD)/boot-menu.srec
	-rm $(BUILD)/boot-menu.bin
	-rm $(BUILD)/boot-menu.map
	-rm $(BUILD)/boot-menu-rom.srec
	-rm $(BUILD)/boot-menu-rom.bin
	-rm $(BUILD)/boot-menu-rom.map
	-rm $(B_OBJS) $(B_OBJS_R)
	