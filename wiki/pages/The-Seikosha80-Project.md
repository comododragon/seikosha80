This is a personal project that has been going for a long time. Finally some progress has been achieved, and I decided to use this place as a quest log.

# Long story short

This is an electronic device used to sound like a "Westminster Quarters" clock. It uses a Real-Time Clock (RTC) device to wake up a microcontroller every 15 minutes. Then, the microcontroller (an ATMEGA) reads an SD card, and plays a chime respective to the time pointed by the RTC. That's it.

# Long story long

Now, for the whole story. My family always loved clocks. Like really, clocks everywhere and making lots of noises. My grandma had some clocks that she was very fond of. One of them was a Seiko wall clock that included a "Westminster Quarters" feature. That is, every 15 minutes the clock rings the current time, similar to the so-popular "Big Ben" in London. More info about the chimes [here](https://en.wikipedia.org/wiki/Westminster_Quarters).

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/seikoclock.jpeg?raw=true" alt="drawing" width="400"/>

My father bought two of those clocks. One was in our home since I was born, and the other one was with my grandmother. So, this clock really sounds like childhood to me.

Unfortunately, the clock starts to act strange after a couple of decades. Culprit is that its inner gears start to disintegrate due to age. My father's one was the first, back in 2005-ish. At the time, my father was able to order a new "Seikosha" movement that fitted perfectly. The chimes changed, however. But, it has been so long already that both the new and old chimes are nostalgic to me, but for different times of my life.

My grandma clock worked for a little longer, but then in 2015-ish it started to show its age as well. At first, the clock started to miss some of the quarters, or just play the wrong chimes in the wrong time. Then, it started to stay completely mute for months, and then suddenly start working again for a couple of weeks.

From 2019-ish onwards, it became completely mute. I knew that as soon I tried to open it for fixing, it would probably disintegrate in my hands. Instead of buying a new movement, I decided to take a different approach because I wanted to maintain the most-nostalgic chimes, since I knew that my grandma also enjoyed this one for the memories.

This was around 2021, a time I was messing around with some Arduinos. So, I decided to take on a personal project to build some sort of "Westminster Quarters" device that would play the same chimes as the original, but using a generic movement with pendulum. At least these movements are cheap and easily replaceable, so the clock part wouldn't be a concern anymore.

My first idea was to buy a clock movement that included 15-minutes chimes as in the original Seikosha. Then, I would "hack" the circuit to add my own chime device, Arduino powered. However, I couldn't easily find any clock movement with such functionality that was affordable. I decided then for the simplest clock movements that included a pendulum, and the chime device would need its own internal clock. Then, I would manually synchronize the clock movement with the chime device, and that would work sufficiently well for some months.

## Project constraints

I had some constraints in mind:

1. **Device has to be battery-powered.** This is a wall clock using battery, and it doesn't make much sense - in my mind - to add an electrical wire to the clock. So, this device would have to operate by battery;
2. **Device has to operate for months using normal non-rechargeable batteries.** I wanted to make this device serviceable by my grandmother, which was a pro on changing batteries from her clocks. This means no rechargeable batteries, since I know that my grandmother wouldn't know how to handle lithium cells properly. Also, I wanted this device to operate for several months, similar to the original clock movement;
3. **Use easy and affordable components.** All electronic components should be available on Arduino-hobbyist websites, or at most AliExpress. Stores like DigiKey, Mouser, etcetc. are out of question for me. First because most of times it doesn't make much sense to buy there as a hobbyist (in low volumes), and second because their shipping is quite expensive for a humble Brazilian resident.

# Extracting the chimes

The first part was to get these chimes. I couldn't find them on the internet. So, I decided to remove the failing movement from my grandma's clock, and try to make it work. In no time, some gears started to disintegrate, but after some fiddling, I was actually able to make it play the chimes. The internal mechanism had a metallic contact that rotated with time, and its position would connected different pads on the PCB. Below is a picture of these pads:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/chimepads.jpeg?raw=true" alt="drawing" width="400"/>

These pads go directly to a mysterious chip, located below a black blob on the circuit board. After some fiddling, I was able to manually rotate the metallic contact gear and make all chimes play. I removed the speaker and connected the output directly to a 2.5mm audio jack, and hooked to an old PC. I was then able to record all chimes using Audacity:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/chimeextract.jpeg?raw=true" alt="drawing" width="600"/>

Now, comes the real work: creating the device to play these chimes.

# First - abandoned - attempt (2021)

Back in 2021, I had some knowledge on Arduinos, more specifically I programmed the ATMega328P directly using C. However, I hadn't any experience with creating my own ATMega circuits. So, my experience was limited to off-the-shelf boards such as the Arduino Nano.

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/arduinonano.jpeg?raw=true" alt="drawing" width="400"/>

For the clock functionality, I decided to go with the DS3231 RTC. These are readily available on small boards for Arduino projects. They are cheap, accurate, and can operate at really low power levels. So, it was a perfect choice:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/rtc.jpeg?raw=true" alt="drawing" width="400"/>

For the audio, I decided to go with the LM386 amplifier. Also available in small boards for hobbyists:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/audio.jpeg?raw=true" alt="drawing" width="400"/>

The restriction of using off-the-shelf Arduino boards was a real problem. Although you can easily power an Arduino Nano using batteries, they wouldn't last long due to the linear regulator used in these boards. For the months-long autonomy that I wanted, we need power consumption to be in the microamps level.

I had some ideas at the time, for example to remove the linear regulator and power the board directly using unregulated power from batteries. But, I had some real problem by finding the suitable unregulated voltage range for all components (well, this is why you use a regulator...). I tried to find some really low-power regulators, but they were all unfit for one reason or another (e.g. cost, availability, or my lack of electronics knowledge).

I also encountered another issue that I wasn't expecting: memory. Since I am not synthesising the chimes, I would store the WAV files. They would take at least a couple of MBs, even in low quality (Mono 8-bit 8kHz). This means I needed an external memory chip.

At the time, many challenges arised and I had other issues in my life to take care of (like a PhD). So, this project became quite abandoned for a couple of years, with the components taking dust on my table.

# Second attempt, finally a kickoff (2024-)

It is 2024, my grandma has passed away for almost two years already (rest in peace), and I decided to give another try to this project. Unfortunately, my grandma wouldn't be able to appreciate the result, but at least is one less abandoned project.

I decided to go full standalone and use discrete components, no off-the-shelf boards (I was a bit inspired by watching too many BigClive videos on Youtube...). I realised that the ATMega microprocessor is such a versatile component that can be powered with as little as batteries, some capacitors, and a resistor for the reset pullup. That's it. So, why not build my own board?

As soon as I removed this "only Arduino Nano" restriction, the problem became much easier and I actually found several ideas on the Arduino forums that could work as a base. For example, Nick Gammon developed a battery-powered temperature logger to be used in a museum down under in Australia. He used the ATMega328P, it was battery powered, used RTC, and had a power on/off mechanism using a P-MOSFET. The FET was used to power on/off the power-hungry devices, leaving only the ATMega fully powered by the battery. This way, the batteries can last for months. The project can be found [here](https://www.gammon.com.au/forum/?id=12106), and it is an interesting read.

This project was a perfect base for my application, given some changes that I would need for my own objectives. All components are easily available (apart from the PFETs, I had to order them from AliExpress, but they were quite cheap).

I decided to stick with 3 AA/AAA batteries. That would give me 1.65V x 3 = 4.95V when full, and I am expecting something around 1.1V x 3 = 3.3V when depleted. So, ideally, the device has to work in this range: 3.3-4.95V.

After much work and thoughts, I was able to put together a first prototype (Revision 1) for this project. The schematic is available [here](https://github.com/comododragon/seikosha80/blob/main/schematics/rev1/kicadproj/output.pdf).

Below is the picture of Revision 1:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/rev1.jpeg?raw=true" alt="drawing" width="600"/>

So far, it has been operating quite well. Power consumption is extremely low (after weeks of operation, each cell is still around 1.4V), which makes me believe this prototype will be able to reach the many-months of operation that I expect.

This device includes the following components:

- **Three AA/AAA** batteries;
- A **DS3231 RTC** that keeps track of time, and is configured to trigger an alarm every fifteen minutes;
- A speaker and an **LM386 amplifier** to play the chimes;
- A **microSD board**, that holds the microSD containing the chimes;
- **Two PFETs**, one for reverse polarity protection, and one that controls the audio/sdcard power rails;
- **Two pushbuttons**, one for advancing one hour, and one for advancing fifteen minutes;
- An **ATMega328P microcontroller**, responsible for configuring the RTC, reading the SD card and playing the audio.

Below, I detail each part of my quest towards the Revision 1.

## Real-Time Clock

The DS3231 board is very straightforward to use:

- Power `VCC`
- Ground `GND`
- Communicate with the RTC using the I2C wires `SDA` and `SCL`
- Use the `SQW` pin to gate the PFET. `SQW` is open drain, and when an alarm triggers, the DS3231 pulls it low. That can be used to power the PFET and feed the rest of the components (audio, sdcard...)

Since this board will be powered up at all times by the battery, I must ensure that everything unnecessary must be removed or disabled. I desoldered the following components:

- The LED and its resistor;
- The charging diode and its resistor;
    - Due to this charging diode, this board is actually suitable for LIR2032 rechargeable coin cells. Many stores say that one can simply use a CR3032 non-rechargeable coin cell. However, using this board as is **WILL** trickle-charge your CR2032, and that is a bad idea. I had a good online source for this, but unfortunately I lost it. Since I won't use any coin cell, I will just remove this nonsense anyway;
- This board comes with a handy memory chip. It is too small to my application though, so I just removed it, along with its resistor array used for pull-ups;
- Since I am not going to use a coin cell, I removed the coin cell holder as well.

However, I noticed that the DS3231 was still consuming something around 120 microamps even when going into standby. This is already considering that the square wave output `32KHZ` is disabled (it is enabled by default when the RTC is powered up, so we have to manually disable it via I2C). Although this is not a huge issue, I know that this RTC can do less, so why not go lower?

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/rtconvcc.jpeg?raw=true" alt="drawing" width="400"/>

Two things are needed to achieve ultra-low power with this RTC: the 32kHz square wave must be turned off and the RTC shall be powered by `VBAT` instead of `VCC`. The first one can be solved via software, but the second one requires a physical hack on the board. Luckily, there is a neat way of doing that:

- First, cut the `VCC` leg on the DS3231 chip. That effectively disconnects the `VCC` rail of the board from the `VCC` pin;
- Then, solder a wire between `VCC` and `VBAT`. Since I removed the battery holder, `VBAT` is directly accessible from the through-hole where the positive terminal for the battery holder was present. And for the VCC, this DS3231 board usually has a `VCC` through-hole on the other side that is usually reserved for the memory chip. However, this `VCC` pin is directly connected to the whole `VCC` rail. Since we cut the `VCC` pin from the DS3231 chip itself, shunting the `VCC` rail to the positive battery rail effectively connects the board's `VCC` pin to the chip's `VBAT`, which is exactly what we want.

With this solution, I can use the board's `VCC` pin to feed directly `VBAT`. Below is a figure exemplifying these two steps:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/rtcvccmod.jpeg?raw=true" alt="drawing" width="800"/>

And finally, with this mod the RTC drops to less than a single microamp when in standby. That is **VERY** low and more than sufficient for my application:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/rtconvbat.jpeg?raw=true" alt="drawing" width="400"/>

Below is an image showing all the modifications altogether (please ignore my terrible ~pad-destruction~ de-soldering techniques):

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/rtcallmods.jpeg?raw=true" alt="drawing" width="800"/>

And below is the schematic part related to the DS3231 board, with all modifications:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/ds3231boardsch.jpeg?raw=true" alt="drawing" width="800"/>

## Audio Amplifier and Speaker

For the audio part I am simply using an LM386 board. These are readily available on hobbyist stores. For the speaker, I am currently using 8 Ohms, 0.5 W, but I intend to put something a bit stronger soon.

This board - as almost any Arduino-hobbyist-ish boards - has a power LED. I intend to remove it in future revisions, but as of now I just left it. Since this board is only powered when a chime is being played, it does not affect much on the total power consumption.

For the audio input, ideally I would use a DAC. Perhaps I will use one in the future, but as of now I am using a PWM output of the microcontroller. This solution is very simple and works reasonably well.

I still don't know if the LM386 is going to be a perfect choice, because the LM386 datasheet says that the minimum operating voltage is 4 V. If I intend to go below 4 V, the LM386 may not work. I am still investigating. I have bought some alternative components that may come as plan B: LM4871 and NS8002.

The figure below shows the schematic part related to the audio output:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/lm386boardsch.jpeg?raw=true" alt="drawing" width="800"/>

## MicroSD Card Reader

I am not synthesising or mixing the chimes, which means that I have to store fifteen uncompressed audio samples:

- 12 full-hour samples
- 1 sample for one quarter
- 1 sample for half an hour
- 1 sample for three quarters

I need some few MBs to store all this. I could use some simple memory chips, but I was too lazy to create any type of supporting peripherals for programming these memories. SD/microSD cards are very easy to load from an PC, and there are hobbyist boards for them, so I decided to stick with SD/microSD.

I am using an microSD board that includes a 3.3V regulator for the card, and a level shifter for all interfaces between the microcontroller and the card. Hobbyist stores and AliExpress are flooded with SD card boards that only provides a 3.3V regulator but no level shifter. SD cards are intended to operate at around 3.3V, so connecting the ATMega328P pins directly to the card can damage the latter. My - proposed - operating range for my device is 3.3-4.95 V, so this is way above the acceptable level for an SD card.

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/microsdcardboard.jpeg?raw=true" alt="drawing" width="400"/>

The specific board that I am using here has the 3.3V regulator and level shifter (more specifically a 74VHCT125). I can safely connect this board to the SPI ports of my microcontroller, and everyone can be happy.

Here is the schematic part for this daughter board:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/microsdboardsch.jpeg?raw=true" alt="drawing" width="800"/>

## The PFETs

MOSFETs always confused me, but I think I got the idea with this project. I am using a very similar configuration to the temperature logger from Nick Gammon. There are two PFETs: one for reverse polarity protection, and one for the power gating.

### Reverse polarity protection

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/pfetrevpol.jpeg?raw=true" alt="drawing" width="400"/>

Because why not? The simplest way of protecting from reverse polarity is using a single diode, but they have a voltage drop that can really be a pain in low-voltage applications.

I am using the same configuration as in Gammon's device. There is a pretty neat video from [Afrotechmods](https://www.youtube.com/watch?v=IrB-FPcv1Dc) explaining why this is a good solution.

### Power gating

Since I want the batteries to last for months, all powered components must have a really low standby current. In Revision 1, only the RTC and the microcontroller are powered at all time. Their standby current is on the microamp level, so power consumption, so far, seems really low.

All other components are powered through a PFET instead. The gate is controller by the microcontroller and by the RTC alarm. It is an open drain configuration, so both microcontroller and the RTC can enable and hold the PFET active.

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/pfetgating.jpeg?raw=true" alt="drawing" width="400"/>

On the schematic, there are two power rails:

1. `+BATT`: the positive battery terminal. Connected to the PFET, RTC, and microcontroller;
2. `VCC`: the gated positive rail. Connected to the microSD card and audio amplifier boards.

### Choosing the right PFET

At first, I decided to use the same PFETs that Gammon used. That is, an FQP27P06 for the reverse polarity protection, and an FQP47P06 for the power gating. These are quite cheap on AliExpress.

They worked quite well for a couple of weeks, then the device started to become wonky. Sometimes, the PFET wouldn't turn fully off and the system remained on an unstable inoperable state. Then, watching Afrotech's video, i started to raise suspicion on the Vgs value for these PFETs. Vgs has to be below -4V in order to turn on the PFET. I believe that my batteries started to get a bit low on voltage, and there could be some voltage drops on which Vgs would go below -4V. Remember, I am not regulating anything.

I decided that perhaps would be better to go after some PFETs with smaller Vgs values. That - in my assumption - will allow the device to operate more stable when batteries are mid to low life.

There are several PFETs with lower Vgs: FDN302P, IRLML2244, SI2323. However, I couldn't find a single PFET on this level that weren't surface-mount components.

I decided to stick with the SI2323 for some reasons:

- They are cheap and readily available on AliExpress. I couldn't find any of these SMD PFETs on national stores;
- The ratings are within my needs:
    - Absolute Vds = 20V, way above maximum total battery voltage of 4.95V;
    - Absolute Vgs = 8V, also way above 4.95V;
    - Vgs = -1V, which is way below minimum expected operating voltage of 3.3V. This means that the PFET remains in a stable ON/OFF state even when the batteries are about to die;
    - Rds is a bit higher than the FQPxxP06 PFETs, but nothing to be concerned of.

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/si2323.jpeg?raw=true" alt="drawing" width="400"/>

I was also able to find some SMD adapters. With these, I am able to use an SOT-23 SMD PFET as an TO-220 pass-through component.

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/pfetadapters.jpeg?raw=true" alt="drawing" width="400"/>

I have then proceeded to soldering these tiny components on the adapters. I have also soldered some legs, and now these can easily replace the FQPxxP06 PFETs. Please, don't mind my fantastic soldering proficiency.

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/pfets.jpeg?raw=true" alt="drawing" width="600"/>

So far, things seems to be working well. No unstable states so far.

## Pushbuttons

Currently, Revision 1 has two pushbuttons for time adjustment:

- Hour adjustment: when pressed, the RTC is set to the next full hour (and the chime is played)
- Quarter adjustment: when pressed, the RTC is set to the next quarter (and the chime is played)

I plan to add two more buttons following the original Seikosha movement:

- Volume switch: to toggle between low and high
- Night mode: the device stops ringing on night times

## The Microcontroller

For the microcontroller, I chose the ATmega328P-PU. Because why not? Straightforward to use from an Arduino background.

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/uc.jpeg?raw=true" alt="drawing" width="300"/>

Also, this microcontroller can be driven by a potato. Just connected directly to unregulated battery voltage, with 4 capacitors to provide smoothing, filtering, and analog reference. Then, pull `/RESET` high with a resistor and you are good. That's it.

The microcontroller is one of the two components that are always powered, directly from battery. And when the right components are turned off, this chip is ideally low-power for this application.

Here is the schematic part for the heart:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/ucsch.jpeg?raw=true" alt="drawing" width="600"/>

Along the usual connections (`VCC`, `AVCC`, `AREF`, `GND`, and `/RESET`), the following pins are connected:

- `PB2:5`: SPI connection to the microSD card;
- `PC0:1`: will be used by the switches on next revision;
- `PC4:5`: I2C connection to the RTC;
- `PD2`: open drain connection used wake up the microcontroller;
    - Pull-up resistor is located on the RTC board;
    - Can be pulled down by the RTC when alarm is triggered;
    - Can be pulled down by pressing the **hour adjust** button;
- `PD3`: open drain connection used wake up the microcontroller;
    - Can be pulled down by pressing the **15-min adjust** button;
- `PD6`: PWM output used to generate the sound output.

At first, I was not going to keep the ATmega powered at all time. It was also going to be connected to the post-PFET `VCC`. The RTC would be responsible for waking everyone up. However, I discarded this idea when I noticed two issues:

- When waking up via button, it would have to be pressed long enough until the ATmega wakes up, and takes control of the PFET gate. If the button is released before that, the microcontroller would turn off. I don't know how much this would be, but I didn't like this idea at all;
- Also, all pins coming from the RTC board are pulled up. Since the RTC is always energised, this means that these pins would always drive the ATmega pins. It is not recommended to leave pins powered when there is no power on the ATmega, as these currents could actually power the microcontroller up via the protection diodes. This is a no go.

When I found out that the ATmega could go REALLY low power for my needs, then I decided to keep it energised at all times and just focus on the powerdown logic.

### Programming the heart

I am using the usual Arduino bootloader on this chip, because it really helps to flash it using a simple FTDI interface.

All my ATmega328P-PU chips came virgin, so I had to flash the bootloader before I could use the FTDI programming method. I have created a pretty dodgy TSA-unapproved programmer using an Arduino Nano as ISP Programmer:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/bootloaderpgmr.jpeg?raw=true" alt="drawing" width="600"/>

After bootloader is loaded (heh), I have another pretty dodgy TSA-unapproved programmer on which I can interface with an FTDI board and flash the code:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/ftdipgmr.jpeg?raw=true" alt="drawing" width="600"/>

My original idea with this board was to be able to use it for both pre- and post-bootloader programming, as it interfaces with the RX/TX and the ICSP pins of the ATmega. However, I couldn't make the ICSP part to work, probably because relying on the internal oscillator for bootloader flash is quite flimsy. But for the FTDI part, it works quite well. I eventually bought an Arduino Uno that I could use instead, but turns out that it is easier to insert and remove the ATmega on the breadboard from my own circuit. So I am still using it for all my post-bootloader programmings.

In order to avoid potential flashing errors, I have to make slight pressure with my thumb on the ATmega while it is being programmed. I call that TIF Socket (Thumb Insertion Force Socket).

## The Software

Now, the software to tie all of this together.

Current version is located at `source/0_bringup`. Most part of the code is self-explanatory with comments, so go have a look for deep details. The subsections below present the most important parts.

### Makefile

The project is built using `make`.

You will need the AVR-related tools in order to build this, like `avr-gcc`. If you have Arduino IDE installed, you probably have it already. For Ubuntu, required packages are `gcc-avr`, `binutils-avr`, `avr-libc`, and `avrdude`. Other Linux distributions might be quite similar.

Command `make help` brings the available targets (`make <target>`):
- `program`: build the program and try to upload it to the ATmega using an FTDI;
- `sdcard`: generate the binary image to be cloned on an microSD card;
- `clean`: cleanup generated files.

Make variables that can be changed:
- `F_CPU`: microcontroller frequency, default is `8000000UL`;
- `MCU`: target microcontroller, default is `atmega328p`;
- `PORT`: target serial port, default is `/dev/ttyUSB0`;
- `BAUD`: port baud rate, default is `57600`.

### Main file

The `src/main.c` file ties all together. Performs RTC, SD, audio calls, puts the device to sleep, detect button presses, etc.

The sound part is handled by interrupts. While these interrupts are happening, the code is on a loop reading from the microSD card and writing to a ring buffer, used as cache. Read and write rate must be balanced so that no buffer overrun occurs, nor starvation.

### Inputs

The files `include/inputs.h` and `src/inputs.c` implement the interface with the buttons and interrupts.

### Power

The files `include/power.h` and `src/power.c` have functions related to power management, mainly preparing for power down and for managing the post-PFET `VCC` rail.

### RTC

The files `include/rtc.h` and `src/rtc.c` implement the function for communicating with the DS3231 RTC. It uses I2C to read date, alarm status, and write next alarms.

The I2C calls are handled by a supporting library located at `include/twiutils.h` and `src/twiutils.c`.

### SD

The files `include/sd.h` and `src/sd.c` implements the logic to deal with the microSD card. This is not a perfect implementation, but has been working with absolutely no issues for weeks. I also ensured to test with both new and old cards. For example, currently I am using an 128MB TransFlash card from a very old phone (remember TransFlash?). Also tested with a 4GB microSD.

This implementation was heavily based on the teaching of this reference: http://elm-chan.org/docs/mmc/mmc_e.html. The microSD is accessed in SPI mode, which is the most basic and low speed mode.

With this current mode, the code has been optimised to maximise the reading bandwidth, including all potentially unnecessary overheads in the library. The ATmega is not able to hold a massively big array as cache, so current audio quality is Mono, 8-bit signed PCM, 10kHz sampling rate. Could be better, but for now is enough. The chimes are not that hi-fi anyway...

SPI calls are handled by a supporting library located at `include/spiutils.h` and `src/spiutils.c`.

### Sound

The analog sound signal is generated using the PWM capability of the ATmega. Works reasonably OK. Implementation is on `include/sound.h` and `src/sound.c`.

### Chime conversion and image file

On `utils`, there is a Python 3 script that can be used to create a microSD card image with the chimes. It has a pretty self-explanatory help message:

```
Usage: ./utils/prepare_chime_files.py CHIMESFOLDER SDCARDIMG INDEXH

    where CHIMESFOLDER points to a folder containing
    15 wav files, named as follows:
        quarter.wav
        half.wav
        threequarters.wav
        01hour.wav
        02hour.wav
        03hour.wav
        04hour.wav
        05hour.wav
        06hour.wav
        07hour.wav
        08hour.wav
        09hour.wav
        10hour.wav
        11hour.wav
        12hour.wav

The names are self-explanatory.
These files will be concatenated in this order to a binary image
1 channel, 8 bit, 10000kHz
Each wave will have 5 appended seconds of silence
Before each wave file, an unsigned long is placed with the size in bytes of the wave file
This binary image can be dd'ed to an sdcard
And an "index.h" file will be generated with the file table
```

It generates a microSD card image named `sdcard.img`. This image can be directly cloned to a card **(always good to remind that THIS WILL DESTRUCT THE CONTENTS IN YOUR CARD)**. For the Seikosha chimes, the image size is no larger than 10MB.

### Index

The ATmega will read the chimes from the microSD card, but it needs the base address for each chime. There is no filesystem or file table.

The microcontroller uses an address array with 16 elements named `sdCardIndex`. Each element points to the begin address of each chime, byte-addressed. The last element points to EOF.

This file is automatically generated by `prepare_chime_files.py`. It must be copied to `include/index.h` before building the software.

## Schematics and layouts

The schematic and PCB layout files are located in `schematics_layouts/`. Tool being used is `KiCad` because it is free.

Below is a print of current schematic:

<img src="https://github.com/comododragon/seikosha80/blob/main/wiki/sch.jpeg?raw=true" alt="drawing" width="800"/>

The PDF file for Revision 1 can be found [here](https://github.com/comododragon/seikosha80/blob/main/schematics_layouts/rev1/kicadproj/output.pdf).

**PCB layouts will also be included in these projects.**

## The old layout folder...

There is an `old/layouts/` folder. Don't look there. These are preliminary layout for the breadboard circuits. I had no Internet at the time I was doing this, so please ignore the file format I used. Really, just nod and move on.

## Next steps

Current status is: testing revision 1 on breadboard. I want to see for how long it will work without breaking anything. After some months, battery voltage is 4.15V total.

My next step is to create the PCB layout, then put the Revision 1 all together.
