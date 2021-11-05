F_CPU = 2500000
mmcu = atmega168

cfiles   := $(wildcard *.c)
hexfiles := $(patsubst %.c,%.hex,$(cfiles))

avr_gcc = avr-gcc
avr_objcopy = avr-objcopy
avr_size = avr-size
avrdude = avrdude

AVRDUDE_PROGRAMMER = avrisp2

CFLAGS   = -mmcu=$(mmcu) -DF_CPU=$(F_CPU) -pipe -Wall -O2 -std=c99
MAPFLAGS = -Wl,-Map,

all : $(hexfiles)

%.elf : %.c
	$(avr_gcc)  $(CFLAGS) -o $@ $<
	$(avr_size) --common $@

%.hex : %.elf
	$(avr_objcopy) -O ihex $< $@

flash : $(hexfiles)
	$(avrdude) -c $(AVRDUDE_PROGRAMMER) -U flash:w:$(hexfiles)

clean :
	rm -f *.hex

.PHONY : clean
.DEFAULT_GOAL := all
