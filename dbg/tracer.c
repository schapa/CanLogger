/*
 * tracer.c
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#include "tracer.h"
#include "string.h"
#include "bsp.h"
#include "memman.h"
#include "stm32f10x_usart.h"

typedef struct traceNode {
	struct traceNode *next;
	char *string;
	size_t size;
} traceNode_t, *traceNode_p;

static traceNode_p s_traceHead = NULL;
static traceNode_p s_traceTail = NULL;

void USART1_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);

static void initNVIC(void);
static void sendNextItem(const char *ptr, size_t size);
static void onTxComplete(void);

void Trace_InitUSART1(void) {

	USART_InitTypeDef iface = {
			115200,
			USART_WordLength_8b,
			USART_StopBits_1,
			USART_Parity_No,
			USART_Mode_Rx | USART_Mode_Tx,
			USART_HardwareFlowControl_None
	};
	USART_DeInit(USART1);
	USART_Init(USART1, &iface);
//	USART_ITConfig(USART1, USART_IT_PE, ENABLE);
//	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
//	USART_ITConfig(USART1, USART_IT_LBD, ENABLE);
//	USART_ITConfig(USART1, USART_IT_CTS, ENABLE);
//	USART_ITConfig(USART1, USART_IT_ERR, ENABLE);
	USART_Cmd(USART1, ENABLE);

	initNVIC();
}

void Trace_dataAsync(char *buff, size_t size) {
	_Bool send = false;

	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	traceNode_p elt = (traceNode_p)MEMMAN_malloc(sizeof(traceNode_t));
	if (elt) {
		elt->next = NULL;
		elt->string = buff;
		elt->size = size;
		if (!s_traceHead) {
			s_traceHead = s_traceTail = elt;
			send = true;
		} else {
			s_traceTail->next = elt;
			s_traceTail = elt;
		}
	}
	if (!primask) {
		__enable_irq();
	}

	if (send) {
		sendNextItem(buff, size);
	}
}

void Trace_dataAsyncFlush(void) {
	if (s_traceHead) {
//		HAL_USART_DMAStop(s_tracerHandle);
//		while (s_traceHead) {
//			HAL_USART_Transmit(s_tracerHandle, (uint8_t*)s_traceHead->string, s_traceHead->size, 0xFF);
//			traceNode_p cur = s_traceHead;
//			s_traceHead = cur->next;
//			MEMMAN_free(cur->string);
//			MEMMAN_free(cur);
//		}
	}
}

void Trace_dataSync(const char *buff, size_t size) {
	if (!buff || !size)
		return;
	Trace_dataAsyncFlush();
//	HAL_USART_Transmit(s_tracerHandle, (uint8_t*)buff, size, 0xFF);
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_PE)) {

	}
	if (USART_GetITStatus(USART1, USART_IT_TXE)) {

	}
	if (USART_GetITStatus(USART1, USART_IT_TC)) {
		USART_ClearITPendingBit(USART1, USART_IT_TC);
	}
	if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	if (USART_GetITStatus(USART1, USART_IT_IDLE)) {

	}
	if (USART_GetITStatus(USART1, USART_IT_LBD)) {
		USART_ClearITPendingBit(USART1, USART_IT_LBD);
	}
	if (USART_GetITStatus(USART1, USART_IT_CTS)) {
		USART_ClearITPendingBit(USART1, USART_IT_CTS);
	}
	if (USART_GetITStatus(USART1, USART_IT_ORE)) {

	}
	if (USART_GetITStatus(USART1, USART_IT_NE)) {

	}
	if (USART_GetITStatus(USART1, USART_IT_FE)) {

	}
}

//void HAL_USART_TxCpltCallback(/*USART_HandleTypeDef *husart*/void) {
//	if (!Trace_onTxComplete(husart)) {
//		Event_t event = { EVENT_USART, { ES_UxART_TX },
//				.data.uxart.hUsart = husart
//		};
//		BSP_queuePush(&event);
//	}
//}

void DMA1_Channel4_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_IT_TC4)) { // transfer complete
		onTxComplete();
		DMA_ClearITPendingBit(DMA1_IT_TC4);
	}
	if (DMA_GetITStatus(DMA1_IT_HT4)) { // Half transfer complete
		DMA_ClearITPendingBit(DMA1_IT_HT4);
	}
	if (DMA_GetITStatus(DMA1_IT_TE4)) { // Error occurred
		DMA_ClearITPendingBit(DMA1_IT_TE4);
	}
	if (DMA_GetITStatus(DMA1_IT_GL4)) { // Global interrupt
		DMA_ClearITPendingBit(DMA1_IT_GL4);
	}
}

static void initNVIC(void) {
	NVIC_InitTypeDef nvic = {
			USART1_IRQn,
			0,
			0,
			ENABLE
	};
	NVIC_Init(&nvic);
	nvic.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_Init(&nvic);
}

static void sendNextItem(const char *ptr, size_t size) {

	DMA_InitTypeDef iface = {
			(uint32_t)&(USART1->DR),
			(uint32_t)ptr,
			DMA_DIR_PeripheralDST,
			size,
			DMA_PeripheralInc_Disable,
			DMA_MemoryInc_Enable,
			DMA_PeripheralDataSize_Byte,
			DMA_MemoryDataSize_Byte,
			DMA_Mode_Normal,
			DMA_Priority_Low,
			DMA_M2M_Disable
	};
	DMA_DeInit(DMA1_Channel4);
	DMA_Init(DMA1_Channel4, &iface);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TE, ENABLE);

	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Channel4, ENABLE);
}

static void onTxComplete(void) {
	if (s_traceHead) {
		uint32_t primask = __get_PRIMASK();
		__disable_irq();
		traceNode_p cur = s_traceHead;
		s_traceHead = cur->next;
		MEMMAN_free(cur->string);
		MEMMAN_free(cur);
		if (!s_traceHead) {
			s_traceTail = NULL;
		}
		if (!primask) {
			__enable_irq();
		}
		if (s_traceHead) {
			sendNextItem(s_traceHead->string, s_traceHead->size);
		}
	}
}
