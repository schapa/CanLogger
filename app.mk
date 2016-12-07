
CFLAGS += \
	-I./Queue \
	-I./WH1602 \
	-I./dbg \
	-I./inc \
	-I./menu \
	-I./system/inc/ \
	-I./system/inc/cmsis/ \
	-I./system/inc/diag \
	-I./system/inc/stm32f1-stdperiph \
	
CFLAGS += \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER \
	-DHSE_VALUE=8000000 \
	
export SRC := \
	./src/system.c \
	./src/_write.c \
	./src/main.cpp \
	./src/memman.c \
	./src/systemTimer.c \
	./src/timers.c \
	\
	./menu/mainMenu.c \
	./menu/menu.c \
	./dbg/dbg_trace.c \
	./Queue/Queue.c \

