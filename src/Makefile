F_CPU = 20000000
mmcu = atmega168

cfiles   := $(wildcard *.c)
hexfiles := $(patsubst %.c,%.hex,$(cfiles))

avr_gcc = avr-gcc
avr_objcopy = avr-objcopy
avr_size = avr-size
avrdude = avrdude

CFLAGS   = -mmcu=$(mmcu) -DF_CPU=$(F_CPU) -pipe -Wall -O2
MAPFLAGS = -Wl,-Map,

all : $(hexfiles)

%.elf : %.c
	$(avr_gcc)  $(CFLAGS) -o $@ $<
	$(avr_size) --common $@

%.hex : %.elf
	$(avr_objcopy) -O ihex $< $@

%.hex.upload : %.hex
	$(avrdude) -p $(mmcu) -P /dev/ttyACM0 -c stk500v2 \
	           -U flash:w:$<

clean :
	rm -f *.hex

.PHONY : clean
.DEFAULT_GOAL := all
