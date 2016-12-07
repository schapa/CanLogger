# toolchain defs
include $(ROOT_DIR)/toolchain.mk

OUT_SFX := app
BUILD_DIR :=$(BUILD_DIR)/$(OUT_SFX)

PROGNAME := logger-$(OUT_SFX)
FIRMWARE := logger-$(OUT_SFX).hex
MAP_FILE := $(BUILD_ROOT)/$(PROGNAME).map

# firmware specific sources
export ASM_SRC := 

SRC += \
	./src/bsp.c \
	./src/buttons.c \
	./dbg/tracer.c \
	./WH1602/HD44780.c \
	./firmware/system.c \
	
SRC += \
	./system/src/cmsis/system_stm32f10x.c \
	./system/src/cmsis/vectors_stm32f10x.c \
	./system/src/cortexm/_initialize_hardware.c \
	./system/src/cortexm/_reset_hardware.c \
	./system/src/cortexm/exception_handlers.c \
	
SRC += \
	./system/src/newlib/_cxx.cpp \
	./system/src/newlib/_exit.c \
	./system/src/newlib/_sbrk.c \
	./system/src/newlib/_startup.c \
	./system/src/newlib/_syscalls.c \
	./system/src/newlib/assert.c \
	
SRC += \
	./system/src/stm32f1-stdperiph/misc.c \
	./system/src/stm32f1-stdperiph/stm32f10x_can.c \
	./system/src/stm32f1-stdperiph/stm32f10x_dbgmcu.c \
	./system/src/stm32f1-stdperiph/stm32f10x_dma.c \
	./system/src/stm32f1-stdperiph/stm32f10x_exti.c \
	./system/src/stm32f1-stdperiph/stm32f10x_gpio.c \
	./system/src/stm32f1-stdperiph/stm32f10x_rcc.c \
	./system/src/stm32f1-stdperiph/stm32f10x_spi.c \
	./system/src/stm32f1-stdperiph/stm32f10x_usart.c \

CFLAGS += \
	-I./system/inc/arm \
	-I./system/inc/cmsis \
	-I./system/inc/cortexm \

CFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb -mabi=aapcs \
	-mfloat-abi=soft \

LDFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-fmessage-length=0 \
	-fsigned-char \
	-ffunction-sections \
	-fdata-sections \
	-ffreestanding \
	-fno-move-loop-invariants \
	-Wall \
	-Wextra \
	-T mem.ld -T libs.ld -T sections.ld \
	-nostartfiles \
	-Xlinker --gc-sections \
	-L"./ldscripts" \
	-Wl,-Map,"$(MAP_FILE)" \
	--specs=nano.specs \

.PHONY: all clean info

all: info $(FIRMWARE)

info:
	@echo "[BUILD] firmware"

$(FIRMWARE): $(PROGNAME)
	@echo " [FW] $< -> $@"
	$(OBJCOPY) -O ihex "$(BUILD_ROOT)/$(PROGNAME)" "$(BUILD_ROOT)/$(PROGNAME).hex"
	@echo " [Size]"
	$(SIZE) --format=berkeley "$(BUILD_ROOT)/$(PROGNAME)"

clean::
	@echo "[CLEAN] $(PROGNAME)"
	rm -f $(BUILD_ROOT)/$(PROGNAME) $(BUILD_ROOT)/$(FIRMWARE) $(BUILD_ROOT)/$(MAP_FILE)

include $(ROOT_DIR)/build.mk
