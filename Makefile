# Teensy 4.0
MCU = IMXRT1062
LOWER_MCU = imxrt1062
CPUARCH = cortex-m7

# project name for hex file
TARGET = main


# configurable options  -DUSB_SERIAL
OPTIONS = -DF_CPU=600000000 -DUSB_HID -DLAYOUT_US_ENGLISH -DUSING_MAKEFILE -DMOUSE_INTERFACE

# options needed by many Arduino libraries to configure for Teensy 4.0
OPTIONS += -D__$(MCU)__ -DARDUINO=10810 -DTEENSYDUINO=149 -DARDUINO_TEENSY40


#  COMPILER PATHS
#************************************************************************
UNAME := $(shell uname)

ARDUINOPATH ?= /home/ch/_el/arduino-1.8.12

ifdef ARDUINOPATH

# path location for Teensy Loader, teensy_post_compile and teensy_reboot (on Linux)
TOOLSPATH = $(abspath $(ARDUINOPATH)/hardware/tools)

# path location for Arduino libraries (currently not used)
LIBRARYPATH = $(abspath $(ARDUINOPATH)/libraries)

# path location for the arm-none-eabi compiler
COMPILERPATH = $(abspath $(ARDUINOPATH)/hardware/tools/arm/bin)

else
	ifeq ($(UNAME), Linux)
		COMPILERPATH ?= /usr/bin
		E =  
	else
		COMPILERPATH ?= /usr/local/bin
		E = -e
	endif
endif


#  PATHS
#************************************************************************

# source subdirs 3
SRCDIR = t4
SRCLIB = lib
SRCKC = kc

# output dirs
OBJDIR = obj
BINDIR = bin
PROJECT = main

INC = -I$(SRCDIR) -I$(SRCLIB) -I$(SRCKC)
MCU_LD = $(SRCDIR)/$(LOWER_MCU).ld


#  FLAGS
#************************************************************************

WARN = -Wall -g -Wcomment -Wparentheses
OPT = -O3 -ffunction-sections -fdata-sections -fno-exceptions

# Cortex M7 with single & double precision FPU
ARCH = -mcpu=$(CPUARCH) -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb

# compiler options for C only
CFLAGS = $(WARN) $(OPT) $(ARCH) $(OPTIONS)

# compiler options for C++ only  -Wno-error=narrowing
CXXFLAGS = $(WARN) $(OPT) $(ARCH) -MMD -std=gnu++14 -felide-constructors -fno-rtti $(OPTIONS)

# linker options  --defsym=__rtc_localtime=0 --specs=nano.specs
LDFLAGS = -Os -Wl,--gc-sections,--relax --specs=nano.specs $(ARCH) -T$(MCU_LD)
LIBS = -larm_cortexM7lfsp_math -lm -lstdc++


#  compiler programs
CC_ = $(COMPILERPATH)/arm-none-eabi-gcc
CXX_ = $(COMPILERPATH)/arm-none-eabi-g++
CC = @$(CC_)
CXX = @$(CC_)
OBJCOPY = @$(COMPILERPATH)/arm-none-eabi-objcopy
SIZE = $(COMPILERPATH)/arm-none-eabi-size

#  auto create lists of sources and objects
C_FILES := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCLIB)/*.c) $(wildcard $(SRCKC)/*.c)
CPP_FILES := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCLIB)/*.cpp) $(wildcard $(SRCKC)/*.cpp)
OBJ_FILES := $(addprefix $(OBJDIR)/,$(notdir $(CPP_FILES:.cpp=.o))) $(addprefix $(OBJDIR)/,$(notdir $(C_FILES:.c=.o)))


#  util, color errors and warnings
#************************************************************************
#  "\e[38;5;27m   21 57v  63bv  27 33 39 45 bl  212 pink
#		yl 226 220  or 166  196 red  40 grn  51 cy   90 154 ylgrn
#	@mkdir -p $(dir $@) ?

CC_CLR  = \e[38;5;33m
CXX_CLR = \e[38;5;39m

ERR_CLR  = \e[38;5;202m
WARN_CLR = \e[33m

OBJ_CLR  = \e[38;5;248m
NORM_CLR = \e[38;5;249m
NO_CLR   = \033[m
ST_CLR = \e[38;5;51m

ifeq ($(UNAME), Linux)
COLOR_OUTPUT = 
else
COLOR_OUTPUT = 2>&1 |                                   \
	while IFS='' read -r line; do                       \
		if [[ $$line == *:[\ ]error:* ]]; then          \
			echo $(E) "$(ERR_CLR)$${line}$(NO_CLR)";    \
		elif [[ $$line == *:[\ ]warning:* ]]; then      \
			echo $(E) "$(WARN_CLR)$${line}$(NO_CLR)";   \
		else                                            \
			echo $(E) "$(NORM_CLR)$${line}$(NO_CLR)";   \
		fi;                                             \
	done; exit $${PIPESTATUS[0]};
endif


#  BUILD
#************************************************************************
#  test
#phony:
#	echo $(COMPILERPATH)

kc: $(BINDIR)/$(PROJECT).hex

#  C compilation
$(OBJDIR)/%.o : $(SRCKC)/%.c
	@echo $(E) "$(CC_CLR)  CC\e[m" $<
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ $(COLOR_OUTPUT)
$(OBJDIR)/%.o : $(SRCLIB)/%.c
	@echo $(E) "$(CC_CLR)  CC\e[m" $<
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ $(COLOR_OUTPUT)
$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo $(E) "$(CC_CLR)  CC\e[m" $<
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ $(COLOR_OUTPUT)

#  C++ compilation
$(OBJDIR)/%.o : $(SRCKC)/%.cpp
	@echo $(E) "$(CXX_CLR) CXX\e[m" $<
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@ $(COLOR_OUTPUT)
$(OBJDIR)/%.o : $(SRCLIB)/%.cpp
	@echo $(E) "$(CXX_CLR) CXX\e[m" $<
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@ $(COLOR_OUTPUT)
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@echo $(E) "$(CXX_CLR) CXX\e[m" $<
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@ $(COLOR_OUTPUT)


#  Linker invocation
$(BINDIR)/$(PROJECT).elf: $(OBJ_FILES) $(MCU_LD)
	@echo $(E) "\e[38;5;154m Linking \e[m"
	$(CC) $(LDFLAGS) -o $@ $(OBJ_FILES) $(LIBS)


#  Hex, last
$(BINDIR)/$(PROJECT).hex : $(BINDIR)/$(PROJECT).elf
#	$(SIZE) $<
	@./sizeCalc $(SIZE) ram $< 524288 " SRAM"
	@./sizeCalc $(SIZE) flash $< 2031616 Flash
	$(OBJCOPY) -O ihex -R .eeprom $< $@

#  Upload
	@echo $(E) "$(ST_CLR)Upload$(NO_CLR)"
ifneq (,$(wildcard $(TOOLSPATH)))
	@$(TOOLSPATH)/teensy_post_compile -file=$(basename $@) -path=$(shell pwd) -tools=$(TOOLSPATH)
	@$(TOOLSPATH)/teensy_reboot
endif
	@echo $(E) "$(ST_CLR) $(NO_CLR)"


#  SHORTCUTS
#************************************************************************

#clean:
c:
	@echo $(E) "$(ST_CLR)Clean$(NO_CLR)"
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d $(BINDIR)/$(PROJECT).elf $(BINDIR)/$(PROJECT).hex
#rebuild:
r:
	@make c --no-print-directory
	@echo $(E) "$(ST_CLR)Rebuild$(NO_CLR)"
	@make m --no-print-directory
#make -j
m:
	@make -j6 --no-print-directory
