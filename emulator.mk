
export CPP := g++
export LD := g++

OUT_SFX := emulator
BUILD_DIR :=$(BUILD_DIR)/$(OUT_SFX)

PROGNAME := logger-$(OUT_SFX)

CFLAGS += \
	-I./emulator/ \
	
SRC += \
	./emulator/lcd.c \
	./emulator/system.c \
	./emulator/emulator.c \

#
# emulator specific CFLAGS
CFLAGS += \
	-DEMULATOR \
	-I./emulator/ \
	-Wno-deprecated-declarations \
	-Wall -Wformat=0 \

# emulator specific LIBS
LDLIBS += \
	-lc \
	-lpthread \
	-lrt

.PHONY: all clean info

all: info $(PROGNAME)

info:
	@echo "[BUILD] $(PROGNAME)"

clean::
	@echo "[CLEAN] $(PROGNAME)"
	rm -rf $(BUILD_ROOT)/$(PROGNAME)

include $(ROOT_DIR)/build.mk
