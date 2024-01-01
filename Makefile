# By default, recipe steps will be quieted.
# But a user can supply VERBOSE=1 as an environment variable or command line argument
# to re-enable build output.
VERBOSE ?= 0
ifeq ($(VERBOSE),1)
Q :=
export VERBOSE = 1
else
Q := @
export VERBOSE = 0
endif

TARGET ?= bin_monitor

# This build is only cross-compiled, so we enforce the toolchain files here
CC := arm-none-eabi-gcc
AR := arm-none-eabi-ar
AS := arm-none-eabi-as
OBJCOPY := arm-none-eabi-objcopy

# This function is used to check that a file fits within the allocated size
# Inputs:
#    $1: binary
#    $2: size limit
#  If max size is non-zero, use the specified size as a limit
ENFORCESIZE = @(FILESIZE=`stat -c '%s' $1` ; \
	if [ $2 -gt 0 ]; then \
		if [ $$FILESIZE -gt $2 ] ; then \
			echo "ERROR: File $1 exceeds size limit ($$FILESIZE > $2)" ; \
			exit 1 ; \
		fi ; \
	fi )

# This function can be used to work back from an object file to
# the appropriate .c file in the source tree
c_sourcefile = $(patsubst $(BUILDRESULTS)/%.o,%.c,$(1))
asm_sourcefile = $(patsubst $(BUILDRESULTS)/%.o,%.s,$(1))

#
# The following options can be controlled on the command line
# by supplying a defintion, e.g.: make BUILDRESULTS=output/ DEBUG=1
#
BUILDRESULTS ?= buildresults
DEBUG ?= 1
# TODO: Set the max size for you build, if desired
MAX_SIZE:= 131702

# Default C compilation flags. We use += to allow the user to specify others on the command line
CFLAGS += -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB
CFLAGS += -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage --specs=nano.specs -mfloat-abi=soft -mthumb

PRJ_DIR = projects

LINKER_FILE = $(PRJ_DIR)/STM32F103C8TX_FLASH.ld
# Default C linker flags. We use += to allow the user to specify others on the command line
LDFLAGS += -mcpu=cortex-m3 -T"$(LINKER_FILE)" -Wl,-Map="$(BUILDRESULTS)/$(TARGET).map" -Wl,--gc-sections -static -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group

DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

# By default, this Makefile produces release builds
ifeq ($(DEBUG),1)
CFLAGS += -Og -g3
else
CFLAGS += -O2
endif

.PHONY: all
all: $(BUILDRESULTS)/$(TARGET).hex $(BUILDRESULTS)/$(TARGET).bin

SRC_DIR = application
PRJ_DIR = projects
BUILD_RESULTS = build

PRJ_DRIVERS_DIR = $(PRJ_DIR)/Drivers
PRJ_SRC_DIR = $(PRJ_DIR)/Core

STARTUP_DIR = $(PRJ_SRC_DIR)/Startup
HAL_DIR = $(PRJ_DRIVERS_DIR)/STM32F1xx_HAL_Driver

APP_SOURCES = $(wildcard $(SRC_DIR)/*/*.c)
APP_SOURCES += $(STARTUP_DIR)/startup_stm32f103c8tx.s
APP_SOURCES += $(wildcard $(PRJ_SRC_DIR)/Src/*.c)
APP_SOURCES += $(wildcard $(HAL_DIR)/Src/*.c)


# Include directory arguments
# Normally, we would separate these for each library and executable target,
# but the STM32 SDK is designed so everything needs to include everything else.
INCLUDES := -I$(SRC_DIR)/at
INCLUDES += -I$(SRC_DIR)/gsm
INCLUDES += -I$(SRC_DIR)/ring_buffer
INCLUDES += -I$(PRJ_SRC_DIR)/Inc
INCLUDES += -I$(HAL_DIR)/Inc
INCLUDES += -I$(PRJ_DRIVERS_DIR)/CMSIS/Device/ST/STM32F1XX/Include
INCLUDES += -I$(PRJ_DRIVERS_DIR)/CMSIS/Include

# Translate the application sources into object file names that can be used as prerequisites
APP_OBJECTS := $(APP_SOURCES:%.c=$(BUILDRESULTS)/%.o)
# We also need to get that pesky .s file
APP_OBJECTS := $(APP_OBJECTS:%.s=$(BUILDRESULTS)/%.o)
# Translate the object files into dependency files
DEPFILES := $(DRIVER_LIB_OBJECTS:%.o=%.d) $(BSP_LIB_OBJECTS:%.o=%.d) $(APP_OBJECTS:%.o=%.d)

# Rules to turn C source files into object files
# Normally, we would want to separate the drivers, BSP, and App into different
# We use the c_sourcefile function to convert the object ot the appropriate source
%.o: %.c
.SECONDEXPANSION:
%.o: $$(call c_sourcefile,$$@) %.d | $$(@D)
	@echo "[CC] $@"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@

%.o: %.s
.SECONDEXPANSION:
%.o: $$(call asm_sourcefile,$$@) | $$(@D)
	@echo "[AS] $@"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


# Assemble the application out of object files and libraries
# This target is now purely a link step. Split into multiple lines due to length
# TODO: Replace with the appropriate program name and library name, add additional libraries
$(BUILDRESULTS)/$(TARGET):| $(BUILDRESULTS)
$(BUILDRESULTS)/$(TARGET): $(APP_OBJECTS)
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) $(APP_OBJECTS) -o $@
	$(Q)echo Built app $@

# Generates a .hex file from the application using objcopy
$(BUILDRESULTS)/$(TARGET).hex: $(BUILDRESULTS)/$(TARGET)
	$(Q)echo Generating hex file $@
	$(Q)$(OBJCOPY) -O ihex $^ $@

# Generates a .bin file from the application using objcopy
$(BUILDRESULTS)/$(TARGET).bin: $(BUILDRESULTS)/$(TARGET)
	$(Q)echo Generating binary $@
	$(Q)$(OBJCOPY) -O binary $^ $@
	$(call ENFORCESIZE,$@,$(MAX_SIZE))

# Removes all generated build output
clean:
	$(Q)$(RM) -r $(BUILDRESULTS)

# Rule to create a mirror of the source tree in the build output folder,
# where object files for APP_SOURCES will be kept
$(patsubst %/,%,$(addprefix $(BUILDRESULTS)/,$(sort $(dir $(APP_SOURCES))))):
	$(Q)mkdir -p $@

# Create the build output folder
$(BUILDRESULTS):
	$(Q)mkdir -p $@

# Create the library output folder
$(LIBDIR):
	$(Q)mkdir -p $@

$(DEPFILES):

flash:
	openocd -f open_ocd.cfg -c "program $(BUILDRESULTS)/$(TARGET) verify reset exit"

include $(wildcard $(DEPFILES))

### Makefile help target, where you can add useful commands.
.PHONY: help
help:
	@echo "\nTo compile the software, run `make`"
	@echo "Options:"
	@echo "	DEBUG=1 (default) generates a debug build, set to 0 to generate a release builds"
	@echo "	BUILDRESULTS=build (default) sets build folder, set to desired build folder name"
	@echo "	VERBOSE=0 (default) controls the display of commands, set to 1 to display the commands"
	@echo "Other targets:"
	@echo "	- clean: removes all generated build results"
	@echo "	- flash: uploads the binary to the target microcontroller"
