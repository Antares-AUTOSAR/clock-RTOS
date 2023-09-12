# master makefile to build project, several targets have been defined with the aim to ease an
# easy handling of the most common task in a project like
# - make 		- To build the project
# - maek clean	- Remove Build folder with objects and binaries
# - make flash 	- Flash the board
# - make open	- Open a SW connection with the target
# - make debug	- OpenGDB ansd start a debug session
# - make lint	- Run cppcheck with MISRA validation
# - make docs	- Run doxygen to extract documentation from code
# - make test	- Run unit tests with code coverage using ceedling  

# Project name
TARGET = temp
# Files to compile
SRCS  = main.c ints.c msps.c startup_stm32g0b1xx.s system_stm32g0xx.c 
SRCS += list.c heap_1.c port.c queue.c tasks.c timers.c
SRCS += SEGGER_SYSVIEW.c SEGGER_RTT.c SEGGER_SYSVIEW_Conf.c SEGGER_SYSVIEW_FreeRTOS.c SEGGER_RTT_printf.c
SRCS += stm32g0xx_hal.c stm32g0xx_hal_cortex.c stm32g0xx_hal_rcc.c stm32g0xx_hal_flash.c
SRCS += stm32g0xx_hal_gpio.c stm32g0xx_hal_pwr_ex.c 
SRCS += stm32g0xx_hal_fdcan.c
SRCS += stm32g0xx_hal_spi.c
SRCS += stm32g0xx_hal_spi_ex.c
SRCS += stm32g0xx_hal_rtc.c
SRCS += stm32g0xx_hal_rtc_ex.c
SRCS += stm32g0xx_hal_rcc_ex.c
SRCS += stm32g0xx_hal_pwr.c
SRCS += stm32g0xx_hal_adc.c
SRCS += stm32g0xx_hal_adc_ex.c
# linker file
LINKER = linker.ld
# Global symbols (#defines)
SYMBOLS = -DSTM32G0B1xx -DUSE_HAL_DRIVER
# directories with source files to compiler (.c y .s)
SRC_PATHS  = app
SRC_PATHS += cmsisg0/startups
SRC_PATHS += halg0/Src
SRC_PATHS += rtos
SRC_PATHS += rtt/Config
SRC_PATHS += rtt/FreeRTOS
SRC_PATHS += rtt/SEGGER
SRC_PATHS += rtt/SEGGER/Syscalls
# directories with header files
INC_PATHS  = app
INC_PATHS += app/config
INC_PATHS += cmsisg0/core
INC_PATHS += cmsisg0/registers
INC_PATHS += halg0/Inc
INC_PATHS += rtos/include
INC_PATHS += rtt/Config
INC_PATHS += rtt/FreeRTOS
INC_PATHS += rtt/SEGGER

# -------------------------------------------------------------------------------------------------
# NOTE: From this point do not edit anything at least you know what your are doing
# ------------------------------------------------------------------------------------------------- 
TOOLCHAIN = arm-none-eabi
CPU = -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft

# compiler flags
CFLAGS  = $(CPU)
CFLAGS += -O0                        # Compile with Size Optimizations (O0, O1, O2, O3, Os)
CFLAGS += -g3                        # Debugging information level (g1, g2, g3)
CFLAGS += -ffunction-sections        # Create a separate function section
CFLAGS += -fdata-sections            # Create a separate data section
CFLAGS += -fno-builtin               # Don't recognize built-in functions that do not begin with ‘__builtin_’ as prefix
CFLAGS += -std=c11                   # Comply with C11
CFLAGS += -Wall                      # Be anal Enable All Warnings
CFLAGS += -pedantic                  # Be extra anal More ANSI Checks
CFLAGS += -Wstrict-prototypes        # Warn if a function is declared or defined without specifying the argument types
CFLAGS += -fsigned-char              # char is treated as signed
CFLAGS += -fdiagnostics-color=always # color the output
CFLAGS += -fomit-frame-pointer       # Don't keep the frame pointer in a register for functions that don't need one
CFLAGS += -fverbose-asm              # Put extra commentary information in the generated assembly code
CFLAGS += -MMD -MP

# assembler flags
AFLAGS = $(CPU)

# linker flags
LFLAGS  = $(CPU) 
LFLAGS += -Wl,--gc-sections
LFLAGS += --specs=rdimon.specs 			# link with semihosting 
LFLAGS += --specs=nano.specs 			# nano version of stdlib
LFLAGS += -Wl,-Map=Build/$(TARGET).map	# Generate map file 

# Linter ccpcheck flags
LNFLAGS  = --inline-suppr       # comments to suppress lint warnings
LNFLAGS += --quiet              # spit only useful information
LNFLAGS += --enable=warning,style # enable only warnings
LNFLAGS += --error-exitcode=1	# return error if any warnings
LNFLAGS += --std=c11            # check against C11
LNFLAGS += --template=gcc       # display warning gcc style
LNFLAGS += --force              # evaluate all the #if sentences
LNFLAGS += --platform=unix32    # lint againt a unix32 platform, but we are using arm32
LNFLAGS += --cppcheck-build-dir=Build/lint

# prefix substitution 
OBJS = $(SRCS:%.c=Build/obj/%.o)
OBJS := $(OBJS:%.s=Build/obj/%.o)
DEPS = $(OBJS:%.o=%.d)

# set source and header directories variables
VPATH = $(SRC_PATHS)
INCLS = $(addprefix -I ,$(INC_PATHS))

#---Build project----------------------------------------------------------------------------------
all : build format lint $(TARGET)

$(TARGET) : $(addprefix Build/, $(TARGET).elf)
	$(TOOLCHAIN)-objcopy -Oihex $< Build/$(TARGET).hex
	$(TOOLCHAIN)-objdump -S $< > Build/$(TARGET).lst
	$(TOOLCHAIN)-size --format=berkeley $<

Build/$(TARGET).elf : $(OBJS)
	$(TOOLCHAIN)-gcc $(LFLAGS) -T $(LINKER) -o $@ $^

Build/obj/%.o : %.c
	$(TOOLCHAIN)-gcc $(CFLAGS) $(INCLS) $(SYMBOLS) -o $@ -c $<

Build/obj/%.o : %.s
	$(TOOLCHAIN)-as $(AFLAGS) -o $@ -c $<

-include $(DEPS)

.PHONY : build clean flash flash open debug docs lint test terminal format

#---Make directory to place all the generated bynaries for build, docs, lint and test--------------
build :
	mkdir -p Build/obj

#---Erase fodler with program binaries-------------------------------------------------------------
clean :
	rm -rf Build

#---flash the image into the mcu-------------------------------------------------------------------
flash :
	openocd -f board/st_nucleo_g0.cfg -c "program Build/$(TARGET).hex verify reset" -c shutdown

#---open a debug server conection------------------------------------------------------------------
open :
	JLinkGDBServer -if SWD -device stm32g0b1re -nogui -port 3333
#	openocd -f board/st_nucleo_g0.cfg

#---launch a debug session, NOTE: is mandatory to previously open a debug server session-----------
debug :
	arm-none-eabi-gdb Build/$(TARGET).elf -iex "set auto-load safe-path /"

#---Genrete project documentation with doxygen-----------------------------------------------------
docs : build format
	mkdir -p Build/doxygen
	doxygen .doxyfile
	firefox Build/doxygen/html/index.html

#---Run Static analysis plus MISRA-----------------------------------------------------------------
lint : build format
	mkdir -p Build/lint
	cppcheck --addon=misra.json --suppressions-list=.msupress $(LNFLAGS) app

#---Run unit testing with code coverage using ceedling---------------------------------------------
test : build
	ceedling clobber
	ceedling gcov:all utils:gcov

#---RTT viewver to viuasualize print functions output---------------------------------------------- 
terminal :
	JLinkRTTClient

#---format code using clang format-----------------------------------------------------------------
format :
	clang-format -style=file -i $(shell find test -iname *.h -o -iname *.c)
	clang-format -style=file -i $(shell find app -iname *.h -o -iname *.c)
	