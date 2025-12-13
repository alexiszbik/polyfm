# Project Name
TARGET = PolyFM

# Sources
CPP_SOURCES = PolyFM.cpp \
DaisyYMNK/Display/DisplayManager.cpp \
DaisyYMNK/Mux/Mux16.cpp

# Library Locations
LIBDAISY_DIR = libDaisy
DAISYSP_DIR = DaisySP
# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

