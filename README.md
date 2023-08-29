About
=====

USB Keyboard Controler for Teensy 4.0 with TFT ILI9341 color display (320x240).  
Called K.C.4 ("Kacey") for short.  
This is a port and continuation of my [KC repo](https://github.com/cryham/kc) for Teensy 3.2 and ST7735 display (160x128).

It allows editing everything on keyboard display, in real time. No other tools needed.

More description of project on [My Website](https://cryham.tuxfamily.org/portfolio/2020-k-c-4-controller/).

### Screenshots

![](https://raw.githubusercontent.com/cryham/kc4/master/screens.jpg)

## Features

#### Keyboard functions:
* **Mapping** - menu, for remapping USB key codes to physical keys.
* **Layers** - hold key, tap or hold for stick, inc/decrease default layer keys.
* **Sequences** - editor for macros, passwords, and more.
* In sequence **commands** - wait, set key delay, comment, hide rest, repeat itself, run other sequence, and all mouse commands.
* **Mouse** keys - with move acceleration, also for wheel.
* Anti-ghosting code - needed for foil keyboards without diodes.

#### Configuration related:
* **Visual keyboard** layout - drawn on display, with picked layer's mapping.
* Keys list - for picking, with custom group colors and filtering.
* Sequence preview in Mapping and Testing views. Sequence view also lists mapped keys (if any).
* **Help** text pages with all GUI keys used and commands listed.
* Colorful **GUI Menu** with 2 levels and icons.
* Testing pages - showing pressed keys info, matrix, ghosting, layer use, etc.
* Parameters - for Scan Setup (e.g. frequency), Mouse and Display (e.g. brightness), adjustable in menu.
* Setup in EEPROM - loading and saving full keyboard setup (parameters, mappings, sequences).

#### Utility stuff:
* **Clock** with date (needs 3V battery).
* **Temperature** reading from DS18B20 sensor (optional, 1pin).
* LED light (optional, 1pin).
* **Statistics**:
  * uptime, since on or stats reset.
  * *key presses per minute* (and average)
  * keyboard **Active** time and **Inactive** times (previous 2 and total sum).
  * Info page, with % of used: keys, layers, sequences etc.
* Key mapping visualizations, colored all keys with:
  * **Layer use** counts
  * **Heatmap** pressed counts.
* **Graphs**: show *key presses per minute* and temperature over time.
  * Auto ranged with adjustable history length.
  * 3 Sizes: short, normal 2h, and daily 12h, configurable.

#### For fun:
* Many **Demos** with presets. E.g.: Fractal, Plasma, Polyhedrons 3D, Wave, Fire, waving 2D CK logo.
* Falling blocks **Game** - with random generated blocks, many presets and custom options.

---

### Code used

It uses code from repositories, modified slightly:
* [PaulStoffregen cores](https://github.com/PaulStoffregen/cores/tree/master/teensy4) for Teensy 4 core. - Added mouse idle and acceleration.
* [KurtE ILI9341_t3n](https://github.com/KurtE/ILI9341_t3n) for fast ILI9341 display using DMA. - Removed fonts, added small utility methods.
* My previous [KC repo](https://github.com/cryham/kc) as base. Adapted to bigger display and continued with new features.

I am using double buffering (so 2 times screen buffer of 153kB for 320x240) which makes displaying at 45 Fps (almost always constant).  
Single buffering can work too, with slightly less Fps which then depends on drawing time.

This is while scanning keyboard matrix at 1kHz, with higher rate e.g. 2kHz Fps drops to 41 etc.

#### Licensing

My sources are licensed GPLv3. For other sources used, see their repositories.

---

### Hardware

The keyboard is made of:
* Teensy 4.0, at standard 600 MHz.
* TFT LCD display ILI9341, 320 x 240, 16bit color RGB565. At 60 MHz (more does jitter).  
Red PCB with SD card slot, no touch. Terribly low horizontal viewing angle.  
Display uses 5 SPI pins: 9 DC, 10 CS, no RST, rest default.
* Brightness is changed using PWM on pin 19 to LCD LED.

Optionally:
* External Serial EEPROM e.g. 25LC256 (64kB).
  * If not present internal will be used (1080 bytes, enough for basic use).
* DS18B20 1-wire temperature sensor with data pin through 4.7k resistor to 3.3V.
* LED lamp, brightness with PWM (optional 1 pin).
* Photoresistor just to show light intensity value
* External power supply +5V.
  * Allows daily graphs or changing PC without loosing statistics etc.

For more info refer to the [schematic.png](https://raw.githubusercontent.com/cryham/kc4/master/schematic.png) or subdir with Kicad files.


### Configuring

Information useful when starting to use K.C.4 with your keyboard matrix.

Setup is done in files:
* `def.h`
  * Define code features to use like: old demos, game. Also optional pins for LED and Temp'C.
  * Choose keyboard type by define (CKtest,CK1,CK8,CK9 or create your own).
* `matrix.h` has defined Teensy pins for keyboard Matrix_cols and Matrix_rows.
  * CKtest is easiest for testing (2 cols x 4 rows). CK9 is official and uses 18 cols and 8 rows.
  * Columns are as outputs (all High-Z with one set to low "0"), while rows are inputs (all high "1").
* `kbd_layout.cpp` has physical keys layout (drawn on display).

Other key constants are in `kc_params.h` like (max) counts of rows, cols, layers, sequences (also updated in `keys_usb.h`).

When defining a new keyboard see `#if 0` in `main.cpp`, can force starting on test pages to e.g. show matrix or scan codes.


### Building

On **Windows** it is done using [Cygwin](https://www.cygwin.com/) - not tested yet.  
Needs to have arm-gnu-none-eabi installed, just like for Linux or using Arduino compiler.  
See under COMPILER PATHS in Makefile for details.

On **Linux** required package are (at least): binutils-arm-none-eabi, gcc-arm-none-eabi, git.  
On Debian based, you can `sudo apt-get install` them or use package manager.


To get sources in console use:
```
git clone https://github.com/cryham/kc4.git
```
Edit `Makefile` and set your path in line with `ARDUINOPATH ?=`

Then `cd kc` and `make` (or `make -j8` for 8 threads/CPUs) to start building.

On successful build the last lines are e.g.
```
  CC kc/matrix.c
 Linking 
   SRAM: 70%  370656 / 524288 B
  Flash: 15%  311032 / 2031616 B
Upload
```
showing used percentages of memories.

It should auto upload new binary to Teensy.  
If not, open created binary `bin/main.hex` with [Teensy Loader App](https://www.pjrc.com/teensy/loader.html) and press reset on board to flash (upload) and use.
