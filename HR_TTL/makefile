#target
TARGET = stm32_project

#debug build?
DEBUG = 1
#optimization
OPT = -Og


#build path
BUILD_DIR = build

#C sources
C_SOURCES = \
	    $(shell find ./ -name '*.c')

#ASM sources
ASM_SOURCES = \
startup/startup_stm32f10x_md.s

#------------------------------------------------H
#binaries
#------------------------------------------------H
PREFIX = arm-none-eabi-
ifdef GCC-PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif 
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#------------------------------------------------H
#CFLAGS(compiler flags)
#------------------------------------------------H
#cpu
CPU = -mcpu=cortex-m3
#FPU
#f103 has no fpu

#flaot-abi

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT_ABI)

# macros for gcc
# AS defines
AS_DEFS = 

#C defines
C_DEFS = \
-DUSE_STDPERIPH_DRIVER \
-DHSE_VALUE=8000000 \
-DHSE_STARTUP_TIMEOUT=5000 \
-DLSI_VALUE=32768 \
-DLSE_VALUE=40000 \
-DVDD_VALUE=3300 \
-DSTM32F10X_MD 

# Defined in stm32f10x.h
# -DHSE_STARTUP_TIMEOUT=100 \
# -DHSI_VALUE=8000000 \

#no asm files
#AS_INCLUDES = 

#C includes
C_INCLUDES = \
	     -Icmsis \
	     -IDrivers/inc \
	     -ILibraries/inc \
	     -Isource \
         -I/usr/arm-none-eabi/include

#compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
	CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = stm32f103.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLOAT= -lc -lrdimon -u _printf_float
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections $(LDFLOAT) 

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
#
# compile C files and Generate .lst 
$(BUILD_DIR)/%.o: %.c makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
# compile asm files 
$(BUILD_DIR)/%.o: %.s makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

# Link all object files to create the ELF file
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	echo "C sources "$(C_SOURCES)
	mkdir $@	

#######################################
# clean up
#######################################
clean:
	-rm -rf $(BUILD_DIR)

#######################################
#flash
#######################################
flash: $(BUILD_DIR)/$(TARGET).hex
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c init -c "reset halt; wait_halt; flash write_image erase $(BUILD_DIR)/$(TARGET).hex" -c reset -c shutdown

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

