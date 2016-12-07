/*
 * bsp.c
 *
 *  Created on: Dec 28, 2015
 *      Author: shapa
 */

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_dbgmcu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "bsp.h"
#include "systemTimer.h"
#include "tracer.h"
#include "dbg_base.h"

void EXTI15_10_IRQHandler(void);

static void initialize_RCC(void);
static void initialize_GPIO_LED(void);
static void initialize_GPIO_Buttons(void);
static void initialize_GPIO_Trace(void);
static void initialize_GPIO_Power(void);
static void initialize_GPIO_LCD(void);
static void initialize_GPIO_CAN(void);
static void initialize_GPIO_SPI(void);

static uint8_t configure_CAN(void);
static void configure_CAN_NVIC(void);
static void configure_GPIO_NVIC(void);

static void dumpSystemStartup(void);

static void setSTBState(FunctionalState);
static void setENState(FunctionalState);
static _Bool getERRState(void);

//static _Bool sendData(uint32_t id, uint8_t *data, uint8_t size);

static _Bool s_isInitialized = false;
static volatile EventQueue_p s_eventQueue = NULL;

_Bool BSP_init(void) {
	_Bool result = true;

	System_init(BSP_SetRedLedState);
	initialize_RCC();
	initialize_GPIO_LED();
	initialize_GPIO_Buttons();
	initialize_GPIO_Trace();
	initialize_GPIO_Power();
	initialize_GPIO_LCD();
	initialize_GPIO_CAN();
	initialize_GPIO_SPI();
	configure_CAN_NVIC();
	configure_GPIO_NVIC();
	Trace_InitUSART1();

	System_setStatus(INFORM_INIT);
	result |= configure_CAN();

	dumpSystemStartup();

	s_isInitialized = true;
	return result;
}

void BSP_SetRedLedState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, val);
}

void BSP_SetGreenLedState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, val);
}

void BSP_queuePush(Event_p pEvent) {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	s_eventQueue = Queue_pushEvent(s_eventQueue, pEvent);
	if (!primask) {
		__enable_irq();
	}
}

void BSP_pendEvent(Event_p pEvent) {
	while (!s_eventQueue);
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	s_eventQueue = Queue_getEvent(s_eventQueue, pEvent);
	if (!primask) {
		__enable_irq();
	}
}

void BSP_LcdBacklight(_Bool state) {
	BitAction val = state ? Bit_SET : Bit_RESET;
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, val);
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI_GetFlagStatus(EXTI_Line13)) {
		_Bool state = getERRState();
		(void)state;
		// can error
		EXTI_ClearFlag(EXTI_Line13);
	}
}

/* private */
static void initialize_RCC(void) {

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
}

static void initialize_GPIO_CAN(void) {

	GPIO_InitTypeDef iface = {
			GPIO_Pin_8 | GPIO_Pin_9,
			GPIO_Speed_2MHz,
			GPIO_Mode_AF_PP
	};
	GPIO_InitTypeDef ifaceErr = {
			GPIO_Pin_13,
			GPIO_Speed_2MHz,
			GPIO_Mode_IPU
	};
	GPIO_InitTypeDef ifaceControl = {
			GPIO_Pin_14 | GPIO_Pin_15,
			GPIO_Speed_2MHz,
			GPIO_Mode_Out_PP
	};

	GPIO_Init(GPIOB, &iface);
	GPIO_Init(GPIOC, &ifaceErr);
	GPIO_Init(GPIOC, &ifaceControl);

//	Remap to PB8/PB9. Rmap1 = PD0/PD1
	GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);

	/* turn off transmitter */
	setSTBState(ENABLE);
	setENState(DISABLE);
}

static void initialize_GPIO_SPI(void) {
	GPIO_InitTypeDef iface = {
			GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,  // SCK, MISO, MOSI
			GPIO_Speed_2MHz,
			GPIO_Mode_AF_PP
	};
	GPIO_Init(GPIOB, &iface);
	// cs
	iface.GPIO_Pin = GPIO_Pin_12;
	iface.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &iface);
}

static void initialize_GPIO_LED(void) {

	GPIO_InitTypeDef iface = {
			GPIO_Pin_10 | GPIO_Pin_11,
			GPIO_Speed_2MHz,
			GPIO_Mode_Out_PP
	};
	GPIO_Init(GPIOB, &iface);
}

static void initialize_GPIO_Buttons(void) {
	GPIO_InitTypeDef iface = {
			GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2,
			GPIO_Speed_2MHz,
			GPIO_Mode_IPU
	};
	GPIO_Init(GPIOB, &iface);
	iface.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &iface);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource2);
}

static void initialize_GPIO_Trace(void) {
	GPIO_InitTypeDef iface = {
			GPIO_Pin_9 | GPIO_Pin_10,
			GPIO_Speed_2MHz,
			GPIO_Mode_AF_PP
	};
	GPIO_Init(GPIOA, &iface);
}

static void initialize_GPIO_Power(void) {
	GPIO_InitTypeDef iface = {
			GPIO_Pin_6,
			GPIO_Speed_2MHz,
			GPIO_Mode_AF_PP
	};
	GPIO_Init(GPIOB, &iface);
	GPIO_SetBits(GPIOB, GPIO_Pin_6); // hold power On
}

static void initialize_GPIO_LCD(void) {
	GPIO_InitTypeDef iface = {
			GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
				GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_15, // PA15 - backlight
			GPIO_Speed_2MHz,
			GPIO_Mode_Out_PP
	};
	GPIO_Init(GPIOA, &iface);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	BSP_LcdBacklight(false);
}

static void setSTBState(FunctionalState state) {
	/* low means StandBy */
	BitAction val = (state == DISABLE) ? Bit_SET : Bit_RESET;
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, val);
}

static void setENState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOC, GPIO_Pin_14, val);
}

static _Bool getERRState(void) {
	/* low means Error, or WakeUp - handled by interrupt */
	return (_Bool)!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
}

static uint8_t configure_CAN(void) {
	uint8_t initResult = 0;
	CAN_InitTypeDef iface = {0};
	CAN_FilterInitTypeDef  ifaceFilter ={0};
	const uint32_t baudRate = 125000;
	RCC_ClocksTypeDef RCC_Clocks;

	setSTBState(DISABLE);
	setENState(ENABLE);
	RCC_GetClocksFreq(&RCC_Clocks);

	iface.CAN_TTCM = DISABLE;
	iface.CAN_ABOM = DISABLE;
//	iface.CAN_AWUM = ENABLE;
	iface.CAN_AWUM = DISABLE;
	iface.CAN_NART = ENABLE;
	iface.CAN_RFLM = DISABLE;
	iface.CAN_TXFP = DISABLE;
	iface.CAN_Mode = CAN_Mode_Silent;//CAN_Mode_Normal;
	iface.CAN_SJW = CAN_SJW_1tq;
	iface.CAN_BS1 = CAN_BS1_4tq;
	iface.CAN_BS2 = CAN_BS2_3tq;
	iface.CAN_Prescaler = (uint16_t)(RCC_Clocks.PCLK1_Frequency/(baudRate*(1+4+3))); //(CAN_SJW + CAN_BS1 + CAN_BS2)

	CAN_DeInit(CAN1);
	initResult = CAN_Init(CAN1, &iface);

	ifaceFilter.CAN_FilterNumber = 0;
	ifaceFilter.CAN_FilterMode = CAN_FilterMode_IdMask;
	ifaceFilter.CAN_FilterScale = CAN_FilterScale_32bit;
	ifaceFilter.CAN_FilterIdHigh = 0x0000;
	ifaceFilter.CAN_FilterIdLow = 0x0000;
	ifaceFilter.CAN_FilterMaskIdHigh = 0x0000;
	ifaceFilter.CAN_FilterMaskIdLow = 0x0000;
	ifaceFilter.CAN_FilterFIFOAssignment = CAN_FIFO0;
	ifaceFilter.CAN_FilterActivation = ENABLE;

	CAN_FilterInit(&ifaceFilter);

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);

	CAN_ITConfig(CAN1, CAN_IT_EWG, ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_EPV, ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_BOF, ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_LEC, ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_ERR, ENABLE);

	return initResult;
}

static void configure_CAN_NVIC(void){
	NVIC_InitTypeDef nvic = {
			USB_HP_CAN1_TX_IRQn,
			0,
			0,
			ENABLE
	};
	NVIC_Init(&nvic);

	nvic.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_Init(&nvic);
	nvic.NVIC_IRQChannel = CAN1_RX1_IRQn;
	NVIC_Init(&nvic);
	nvic.NVIC_IRQChannel = CAN1_SCE_IRQn;
	NVIC_Init(&nvic);
}

static void configure_GPIO_NVIC(void) {
	NVIC_InitTypeDef nvic = {
			EXTI0_IRQn,
			0,
			0,
			ENABLE
	};
	EXTI_InitTypeDef exti = {
			EXTI_Line0 | EXTI_Line1 | EXTI_Line2 | EXTI_Line7,
			EXTI_Mode_Interrupt,
			EXTI_Trigger_Rising_Falling,
			ENABLE
	};
	NVIC_Init(&nvic);
	nvic.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&nvic);
	nvic.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_Init(&nvic);
	nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&nvic);

	EXTI_Init(&exti);
}

static void dumpSystemStartup(void) {

	RCC_ClocksTypeDef clock;
	static const char *sourceName[] = {
			"HSI",
			"HSE",
			"PLL"
	};
	const uint8_t clkSource = RCC_GetSYSCLKSource()>>2 & 0x03;
	RCC_GetClocksFreq(&clock);
	uint16_t mHz = (uint16_t)(clock.SYSCLK_Frequency/1000/1000);
	uint16_t kHz = (uint16_t)(clock.SYSCLK_Frequency/1000%1000);
	uint16_t Hz = (uint16_t)(clock.SYSCLK_Frequency%1000);

	DBGMSG_INFO("\nSystem Starting");
	DBGMSG_INFO("\t Device %p Revision %p", DBGMCU_GetDEVID(), DBGMCU_GetREVID());
	DBGMSG_INFO("\t SysFreq %d.%03d.%03d Hz", mHz, kHz, Hz);
	DBGMSG_INFO("\t ClkSource %s", sourceName[clkSource]);

	if (!clkSource) {
		DBGMSG_INFO("WARN: HSE Startup failed");
	}
}

//static _Bool sendData(uint32_t id, uint8_t *data, uint8_t size) {
//	CanTxMsg txMess = {
//			id,
//			id,
//			CAN_Id_Standard,
//			CAN_RTR_Data,
//			size,
//			{0}
//	};
//	if (size > 8 || id > 0x1FFFFFFF)
//		return false;
//
//	txMess.RTR = (size || data) ? CAN_RTR_Data : CAN_RTR_Remote;
//	txMess.IDE = id > 0x7FF ? CAN_Id_Extended : CAN_Id_Standard;
//	memcpy(txMess.Data, data, size);
//
//	return CAN_Transmit(CAN1, &txMess) != CAN_TxStatus_NoMailBox;
//}
//void EXTI2_3_IRQHandler(void) {
//	if (EXTI_GetFlagStatus(EXTI_Line2)) {
//		/* wait at least 1 sec */
//		if (System_getUptime() > 1) {
//			onButtonTimeout(0,0); // force button release
//			Gate_onLedStateChange(BSP_GateLedState());
//		}
//		EXTI_ClearFlag(EXTI_Line2);
//	}
//}

//void CEC_CAN_IRQHandler(void) {
//
//	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0)) {
//		CanRxMsg rx = {0};
//		CAN_Receive(CAN1, CAN_FIFO0, &rx);
//		switch (rx.StdId) {
//			case CAN_SENSOR_ID:
//				Gate_SetState(rx.Data[0] > 12);
//			case CAN_CONTROL_UNIT_ID:
//				Timer_rearm(s_busWatchdogTimId);
//				break;
//		}
//	}
//	if (CAN_GetITStatus(CAN1, CAN_IT_FMP1)) {
//		trace_printf("CAN_IT_FMP1 \n");
//	}
//	if (CAN_GetITStatus(CAN1, CAN_IT_TME)) {
//		trace_printf("CAN_IT_TME \n");
//		trace_printf("\t MAIL 0 %d\n", CAN_TransmitStatus(CAN1, 0));
//		trace_printf("\t MAIL 1 %d\n", CAN_TransmitStatus(CAN1, 1));
//		trace_printf("\t MAIL 2 %d\n", CAN_TransmitStatus(CAN1, 2));
//		CAN_ClearITPendingBit(CAN1, CAN_IT_TME);
//	}
//
//	if (CAN_GetITStatus(CAN1, CAN_IT_EWG)) {
//		trace_printf("EWG \n");
//		CAN_ClearITPendingBit(CAN1, CAN_IT_EWG);
//	}
//	if (CAN_GetITStatus(CAN1, CAN_IT_EPV)) {
//		trace_printf("EPV \n");
//		CAN_ClearITPendingBit(CAN1, CAN_IT_EPV);
//	}
//	if (CAN_GetITStatus(CAN1, CAN_IT_BOF)) {
//		trace_printf("BOF \n");
//		CAN_ClearITPendingBit(CAN1, CAN_IT_BOF);
//	}
//	if (CAN_GetITStatus(CAN1, CAN_IT_LEC)) {
//		trace_printf("LEC \n");
//		CAN_ClearITPendingBit(CAN1, CAN_IT_LEC);
//	}
//	if (CAN_GetITStatus(CAN1, CAN_IT_ERR)) {
//		trace_printf("ERR \n");
//		CAN_ClearITPendingBit(CAN1, CAN_IT_ERR);
//	}
//}
