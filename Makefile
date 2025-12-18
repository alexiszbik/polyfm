# Project Name
TARGET = PolyFM

# Sources
CPP_SOURCES = \
PolyFM.cpp \
Source/PolyFMCore.cpp \
Source/PolyFMDSP.cpp \
Source/PolySynth.cpp \
Source/SynthVoice.cpp \
Source/Lfo.cpp \
DaisyYMNK/Base/DaisyBase.cpp \
DaisyYMNK/Base/HID.cpp \
DaisyYMNK/Display/DisplayManager.cpp \
DaisyYMNK/DSP/SmoothValue.cpp \
DaisyYMNK/DSP/ModuleCore.cpp \
DaisyYMNK/DSP/Parameter.cpp \
DaisyYMNK/DSP/DSPKernel.cpp \
DaisyYMNK/Mux/Mux16.cpp

# Library Locations
LIBDAISY_DIR = libDaisy
DAISYSP_DIR = DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

C_INCLUDES += \
-I. \
-ISource \
-IDaisyYMNK \
-IDaisyYMNK/Base \
-IDaisyYMNK/Display \
-IDaisyYMNK/DSP \
-IDaisyYMNK/Mux

include $(SYSTEM_FILES_DIR)/Makefile

