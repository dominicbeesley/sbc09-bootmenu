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


$(BUILD)/boot-menu.srec: $(B_OBJS)
	$(CROSS_LD) -o $@ -Map=$(patsubst %.srec,%.map,$@) --script=boot-menu.link --oformat=srec -L/usr/lib/gcc/m6809-unknown/4.6.4/ -lgcc $^
	cat ../basic/runme.txt $@ > $(patsubst %.srec,%.run.srec, $@)
	srec_cat $@ -offset -0xE00 -o $(basename $@).bin -binary

$(BUILD)/boot-menu-rom.srec: $(B_OBJS_R)
	$(CROSS_LD) -o $@ -Map=$(patsubst %.srec,%.map,$@) --script=boot-menu-rom.link --oformat=srec -L/usr/lib/gcc/m6809-unknown/4.6.4/ -lgcc $^
	srec_cat $@ -fill 0xFF 0xC000 0xFFFF -offset -0xC000 -o $(basename $@).bin -binary

clean:
	-rm $(BUILD)/boot-menu.run.srec
	-rm $(BUILD)/boot-menu.srec
	-rm $(B_OBJS)
	