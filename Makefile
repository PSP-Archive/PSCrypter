TARGET = Pscrypter
OBJS = main.o crypto.o kirk_engine.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lpsppower

LIBDIR =
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSCRYPTER

BUILD_PRX = 1
#ENCRYPT = 1

#PSP_LARGE_MEMORY = 1 #OFW doesnt have this!

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
