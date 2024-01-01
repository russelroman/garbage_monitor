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
CC := g++


# This function can be used to work back from an object file to
# the appropriate .c file in the source tree
c_sourcefile = $(patsubst $(BUILDRESULTS)/%.o,%.cpp,$(1))
asm_sourcefile = $(patsubst $(BUILDRESULTS)/%.o,%.s,$(1))

#
# The following options can be controlled on the command line
# by supplying a defintion, e.g.: make BUILDRESULTS=output/ DEBUG=1
#
BUILDRESULTS ?= buildresults
DEBUG ?= 1

# Default C compilation flags. We use += to allow the user to specify others on the command line
CFLAGS =
# The -MD $*.d argument below tells the assemble to create the proper dep file
# so that our dependency rules work correctly
# Default C linker flags. We use += to allow the user to specify others on the command line
LDFLAGS = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt


STATIC_LIB_FLAGS := rcs
DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

# By default, this Makefile produces release builds
ifeq ($(DEBUG),1)
CFLAGS += -Og -g3
else
CFLAGS += -O2
endif

.PHONY: all
all: $(BUILDRESULTS)/$(TARGET)

SRC_DIR = application
PRJ_DIR = projects
BUILD_RESULTS = build
CPPUTEST_HOME = utilities/cpputest-3.8

APP_SOURCES = $(wildcard tests/*.cpp)

DRIVER_LIB_SOURCES = 

BSP_LIB_SOURCES = 

INCLUDES = -I$(CPPUTEST_HOME)/include


# The output location where libraries should be placed
LIBDIR:=$(BUILDRESULTS)/lib

# Translate the library sources into object file namesthat can be used as prerequisites
DRIVER_LIB_OBJECTS := $(DRIVER_LIB_SOURCES:%.c=$(BUILDRESULTS)/%.o)
BSP_LIB_OBJECTS := $(BSP_LIB_SOURCES:%.c=$(BUILDRESULTS)/%.o)
# Translate the application sources into object file names that can be used as prerequisites
APP_OBJECTS := $(APP_SOURCES:%.cpp=$(BUILDRESULTS)/%.o)
# We also need to get that pesky .s file
APP_OBJECTS := $(APP_OBJECTS:%.s=$(BUILDRESULTS)/%.o)
# Translate the object files into dependency files
DEPFILES := $(DRIVER_LIB_OBJECTS:%.o=%.d) $(BSP_LIB_OBJECTS:%.o=%.d) $(APP_OBJECTS:%.o=%.d)

# Rules to turn C source files into object files
# Normally, we would want to separate the drivers, BSP, and App into different
# We use the c_sourcefile function to convert the object ot the appropriate source
%.o: %.cpp
.SECONDEXPANSION:
%.o: $$(call c_sourcefile,$$@) %.d | $$(@D)
	@echo "[CC] $@"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@


# Assemble the application out of object files and libraries
# This target is now purely a link step. Split into multiple lines due to length
# TODO: Replace with the appropriate program name and library name, add additional libraries
$(BUILDRESULTS)/$(TARGET):| $(BUILDRESULTS)
$(BUILDRESULTS)/$(TARGET): $(APP_OBJECTS)
	$(Q)$(CC) $(CFLAGS) $(APP_OBJECTS) $(LDFLAGS)  -o $@
	$(Q)echo Built app $@
	./$(BUILDRESULTS)/$(TARGET)

# Removes all generated build output
clean:
	$(Q)$(RM) -r $(BUILDRESULTS)

# Rule to create a mirror of the source tree in the build output folder,
# where object files for APP_SOURCES will be kept
$(patsubst %/,%,$(addprefix $(BUILDRESULTS)/,$(sort $(dir $(APP_SOURCES) $(DRIVER_LIB_SOURCES) $(BSP_LIB_SOURCES))))):
	$(Q)mkdir -p $@

# Create the build output folder
$(BUILDRESULTS):
	$(Q)mkdir -p $@

# Create the library output folder
$(LIBDIR):
	$(Q)mkdir -p $@

$(DEPFILES):

include $(wildcard $(DEPFILES))

### Makefile help target, where you can add useful commands.
.PHONY: help
help:
	@echo "\nTo compile the software, run `make`"
	@echo "Options:"
	@echo "	DEBUG=1 (default) generates a debug build, set to 0 to generate a release builds"
	@echo "Other targets:"
	@echo "	- clean: removes all generated build results"
