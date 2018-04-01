#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := airbox

include $(IDF_PATH)/make/project.mk

# The name of the compiler that we want to use
CXX=g++

# Compiler flags
CXXFLAGS=$(INCDIRS) -std=c++11

# Linker flags
LDFLAGS=$(LIBDIRS) $(LIBS)

# List of header files are included
HEADERS=userLogin.h, airbox.h

all:    GPIOTestApp WaitForPinApp

%.o: %.c $(HEADERS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# List of GPIOTestApp dependencies.
GPIOTESTAPP_DEPS=\
   airbox.o \
   userLogin.o \
   battery.o

AirBoxHomeApp: $(GPIOTESTAPP_DEPS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

# List of WaitForPinApp dependencies.
WAITFORPINAPP_DEPS=\
   WaitForPinApp.o \
   userLogin.o \

WaitForPinApp: $(WAITFORPINAPP_DEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o AirBoxHomeApp WaitForPinApp
