
BUILD=../build

all::$(BUILD)/srec-2.bas


$(BUILD)/srec-2.bas: srec2bas.pl $(BUILD)/srec-2.srec
	./srec2bas.pl <$(BUILD)/srec-2.srec >$(BUILD)/srec-2.bas

$(BUILD)/%.srec:%.asm
	asm6809 -S -o $@ -l $(patsubst %.srec,%.lst,$@) $<

clean:
	-rm $(BUILD)/srec-2.bas
	-rm $(BUILD)/srec-2.srec
	-rm $(BUILD)/srec-2.lst
