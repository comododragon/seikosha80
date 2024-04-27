F_CPU=8000000UL
MCU=atmega328p



.PHONY: help
help:
	# make [ help | program | sdcard | clean ]


.PHONY: program
program: out/main.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:out/main.hex


.PHONY: sdcard
sdcard: out/sdcard.img
	# sdcard image is generated
	# now you go out there and dd it yourself
	# I am not going to write this command that may ruin your operating system
	# go out there and run something like:
	#
	#     sudo dd if=out/sdcard.img of=/dev/mmcblk
	#
	# CONFIRM YOUR SDCARD PATH! It may not be /dev/mmcblk!


.PHONY: clean
clean:
	rm -rf obj/ out/ include/index.h


out/main.hex: out/main
	avr-objcopy -O ihex -R .eeprom out/main out/main.hex


out/main: obj/main.o obj/inputs.o obj/power.o obj/rtc.o obj/twiutils.o obj/sd.o obj/sound.o obj/spiutils.o
	mkdir -p out/
	avr-gcc -mmcu=$(MCU) $^ -o $@


obj/main.o: src/main.c include/index.h include/inputs.h include/power.h include/rtc.h include/twiutils.h include/sd.h include/sound.h include/spiutils.h
	mkdir -p obj/
	avr-gcc -Os -DF_CPU=$(F_CPU) -Iinclude -mmcu=$(MCU) -c src/main.c -o obj/main.o

obj/rtc.o: src/rtc.c include/rtc.h include/twiutils.h
	mkdir -p obj/
	avr-gcc -Os -DF_CPU=$(F_CPU) -Iinclude -mmcu=$(MCU) -c src/rtc.c -o obj/rtc.o

obj/sd.o: src/sd.c include/sd.h include/spiutils.h
	mkdir -p obj/
	avr-gcc -Os -DF_CPU=$(F_CPU) -Iinclude -mmcu=$(MCU) -c src/sd.c -o obj/sd.o

obj/%.o: src/%.c include/%.h
	mkdir -p obj/
	avr-gcc -Os -DF_CPU=$(F_CPU) -Iinclude -mmcu=$(MCU) -c $< -o $@


define prepare_chime_files
	mkdir -p out/
	python3 ./utils/prepare_chime_files.py utils/seikosha-chimes out/sdcard.img include/index.h
endef

include/index.h: utils/seikosha-chimes/*.wav
	$(call prepare_chime_files)

out/sdcard.img: utils/seikosha-chimes/*.wav
	$(call prepare_chime_files)
