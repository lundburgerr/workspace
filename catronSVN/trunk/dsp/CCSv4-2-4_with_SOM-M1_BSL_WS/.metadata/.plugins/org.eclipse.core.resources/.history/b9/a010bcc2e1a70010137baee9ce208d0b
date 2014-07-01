################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

EMPTY := 
SPACE := $(EMPTY) $(EMPTY)
CG_TOOL_ROOT := C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000

ORDERED_OBJS += \
"./src/spiflash_numonyx.obj" \
"./src/evmomapl138_vpif.obj" \
"./src/evmomapl138_videoenc.obj" \
"./src/evmomapl138_videodec.obj" \
"./src/evmomapl138_usb.obj" \
"./src/evmomapl138_uart.obj" \
"./src/evmomapl138_uPP.obj" \
"./src/evmomapl138_touch.obj" \
"./src/evmomapl138_timer.obj" \
"./src/evmomapl138_spiflash.obj" \
"./src/evmomapl138_spi.obj" \
"./src/evmomapl138_rtc.obj" \
"./src/evmomapl138_pmic.obj" \
"./src/evmomapl138_pb.obj" \
"./src/evmomapl138_nor.obj" \
"./src/evmomapl138_nand.obj" \
"./src/evmomapl138_mmcsd.obj" \
"./src/evmomapl138_mcasp.obj" \
"./src/evmomapl138_led.obj" \
"./src/evmomapl138_lcd_raster.obj" \
"./src/evmomapl138_lcd_lidd.obj" \
"./src/evmomapl138_i2c_gpio.obj" \
"./src/evmomapl138_i2c.obj" \
"./src/evmomapl138_gpio.obj" \
"./src/evmomapl138_emac.obj" \
"./src/evmomapl138_dip.obj" \
"./src/evmomapl138_cdce913.obj" \
"./src/evmomapl138_aic3106.obj" \
"./src/evmomapl138.obj" \
"../lib/evmomapl138_bsl.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/subdir_vars.mk
-include lib/subdir_vars.mk
-include subdir_rules.mk
-include src/subdir_rules.mk
-include lib/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: evmomapl138_bsl.lib

# Tool invocations
evmomapl138_bsl.lib: $(OBJS) $(LIB_SRCS)
	@echo 'Building target: $@'
	@echo 'Invoking: Archiver'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c6000/bin/ar6x" r "evmomapl138_bsl.lib" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(C6000_LIBRARY_OUTPUTS__QTD) "evmomapl138_bsl.lib"
	-$(RM) ".\src\evmomapl138.obj" ".\src\evmomapl138_aic3106.obj" ".\src\evmomapl138_cdce913.obj" ".\src\evmomapl138_dip.obj" ".\src\evmomapl138_emac.obj" ".\src\evmomapl138_gpio.obj" ".\src\evmomapl138_i2c.obj" ".\src\evmomapl138_i2c_gpio.obj" ".\src\evmomapl138_lcd_lidd.obj" ".\src\evmomapl138_lcd_raster.obj" ".\src\evmomapl138_led.obj" ".\src\evmomapl138_mcasp.obj" ".\src\evmomapl138_mmcsd.obj" ".\src\evmomapl138_nand.obj" ".\src\evmomapl138_nor.obj" ".\src\evmomapl138_pb.obj" ".\src\evmomapl138_pmic.obj" ".\src\evmomapl138_rtc.obj" ".\src\evmomapl138_spi.obj" ".\src\evmomapl138_spiflash.obj" ".\src\evmomapl138_timer.obj" ".\src\evmomapl138_touch.obj" ".\src\evmomapl138_uPP.obj" ".\src\evmomapl138_uart.obj" ".\src\evmomapl138_usb.obj" ".\src\evmomapl138_videodec.obj" ".\src\evmomapl138_videoenc.obj" ".\src\evmomapl138_vpif.obj" ".\src\spiflash_numonyx.obj" 
	-$(RM) ".\src\evmomapl138.pp" ".\src\evmomapl138_aic3106.pp" ".\src\evmomapl138_cdce913.pp" ".\src\evmomapl138_dip.pp" ".\src\evmomapl138_emac.pp" ".\src\evmomapl138_gpio.pp" ".\src\evmomapl138_i2c.pp" ".\src\evmomapl138_i2c_gpio.pp" ".\src\evmomapl138_lcd_lidd.pp" ".\src\evmomapl138_lcd_raster.pp" ".\src\evmomapl138_led.pp" ".\src\evmomapl138_mcasp.pp" ".\src\evmomapl138_mmcsd.pp" ".\src\evmomapl138_nand.pp" ".\src\evmomapl138_nor.pp" ".\src\evmomapl138_pb.pp" ".\src\evmomapl138_pmic.pp" ".\src\evmomapl138_rtc.pp" ".\src\evmomapl138_spi.pp" ".\src\evmomapl138_spiflash.pp" ".\src\evmomapl138_timer.pp" ".\src\evmomapl138_touch.pp" ".\src\evmomapl138_uPP.pp" ".\src\evmomapl138_uart.pp" ".\src\evmomapl138_usb.pp" ".\src\evmomapl138_videodec.pp" ".\src\evmomapl138_videoenc.pp" ".\src\evmomapl138_vpif.pp" ".\src\spiflash_numonyx.pp" 
	-@echo ' '

post-build:
	-copy "evmomapl138_bsl.lib" "..\lib${BuildArtifactFileName}" /y
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
