MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -I00_avr-libs-uart
SRC = main.c 00_avr-libs-uart/uart.c

TARGET = main
PORT = COM3
BAUD = 115200

main.elf: $(SRC)
	avr-gcc $(CFLAGS) -o $@ $^

all: $(TARGET).hex

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

%.elf: %.c
	$(CC) $(CFLAGS) $< -o $@

flash: $(TARGET).hex
	avrdude -v -p $(MCU) -c arduino -P $(PORT) -b $(BAUD) -U flash:w:$<:i

clean:
	del /Q *.hex *.elf 2>NUL