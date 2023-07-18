BUILD=../build

export CROSS_AS=m6809-unknown-as
export CROSS_LD=m6809-unknown-ld
export CROSS_CC=m6809-unknown-gcc
export CROSS_CCOPTS=-c -Wall -Os -msoft-reg-count=0 -mfar-stack-param -mcode-section=.text -I .
# Workaround for gcc6809 bug - register copy propagation issue
#CROSS_CCOPTS += -fno-cprop-registers


OBJS=crt0 commonmem main uart stdlib stdlib_s stdio string ctype crc16

B_OBJS=$(addprefix $(BUILD)/, $(addsuffix .o, $(OBJS)))

X:=$(shell mkdir -p $(BUILD))

$(BUILD)/%.o: %.c
	$(CROSS_CC) $(CROSS_CCOPTS) -o $@ $<

$(BUILD)/%.o: %.s
	$(CROSS_AS) $(CROSS_ASOPTS) -o $@ $<


$(BUILD)/boot-menu.srec: $(B_OBJS)
	$(CROSS_LD) -o $@ -Map=$(patsubst %.srec,%.map,$@) --script=boot-menu.link --oformat=srec -L/usr/lib/gcc/m6809-unknown/4.6.4/ -lgcc $(B_OBJS) 
	cat ../basic/runme.txt $@ > $(patsubst %.srec,%.run.srec, $@)

clean:
	-rm $(BUILD)/boot-menu.run.srec
	-rm $(BUILD)/boot-menu.srec
	-rm $(B_OBJS)
	