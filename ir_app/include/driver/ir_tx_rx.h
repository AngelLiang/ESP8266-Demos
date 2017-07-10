#ifndef _IR_TEST_H_
#define _IR_TEST_H_

#include "c_types.h"
#include "driver/ringbuf.h"

//===============================================
//    IR TX MODE CONFIG

//-------------------------------------------
//There are 2 ways to Control status machine
//Mode 1:os_timer
   //soft time ,low precision
//Mode2:hw_timer
   //hardware time ,high precision
#define IR_TX_STATUS_MACHINE_HW_TIMER 1
/*There are 2 ways to generate 38k clk signal*/
/*CLK SOURCE MODE*/
/*MODE 1. IR carrier generated from IIS clk. 
       In this mode :MTMS/GPIO2/MTCK/MTDO can be choosed.
       MTMS and GPIO2 are recommended to get more accurate 38k clk*/
/*MODE 2. IR carrier generated from IIS DATA so as to adjust the carrier duty*/
/*MODE 3. IR carrier generated from GPIO sigma-delta module*/

/*IF  GEN_IR_CLK_FROM_IIS:  MODE 1*/
/*ELIF GEN_IR_CLK_FROM_DMA_IIS:  MODE 2*/
/*ELIF GEN_IR_CLK_FROM_GPIO_SIGMA_DELAT:  MODE 3*/
/*ENDIF*/
#define GEN_IR_CLK_FROM_IIS 			1
#define GEN_IR_CLK_FROM_DMA_IIS  		0
#define GEN_IR_CLK_FROM_GPIO_SIGMA_DELAT	0

//================================================
//    IR TX GPIO CONFIG
//-------------------------------------------
#if GEN_IR_CLK_FROM_IIS
#define IR_GPIO_OUT_MUX 	PERIPHS_IO_MUX_MTMS_U //MTCK PIN ACT AS I2S CLK OUT
#define IR_GPIO_OUT_NUM 	14
#define IR_GPIO_OUT_FUNC  	FUNC_GPIO14

#elif GEN_IR_CLK_FROM_DMA_IIS
#define IR_GPIO_OUT_MUX 	PERIPHS_IO_MUX_U0RXD_U  //U0RXD PIN ACT AS I2S DATA OUT
#define IR_GPIO_OUT_NUM 	3
#define IR_GPIO_OUT_FUNC  	FUNC_GPIO3

#elif GEN_IR_CLK_FROM_GPIO_SIGMA_DELAT
#define IR_GPIO_OUT_MUX PERIPHS_IO_MUX_MTMS_U  //EACH PIN CAN OUTPUT SIGMA-DELTA SIGNAL
#define IR_GPIO_OUT_NUM 14
#define IR_GPIO_OUT_FUNC  FUNC_GPIO14

#endif
//-------------------------------------------
//================================================



//===============================================
//    IR RX GPIO CONFIG
//-------------------------------------------
extern RINGBUF IR_RX_BUFF;
#define RX_RCV_LEN 128

#define IR_GPIO_IN_MUX PERIPHS_IO_MUX_GPIO5_U
#define IR_GPIO_IN_NUM 5
#define IR_GPIO_IN_FUNC  FUNC_GPIO5
//-------------------------------------------
//================================================

#define IR_POTOCOL_NEC 0
#define IR_POTOCOL_RC5 1  //not support yet 
#define IR_NEC_STATE_IDLE 0
#define IR_NEC_STATE_PRE 1
#define IR_NEC_STATE_CMD 2
#define IR_NEC_STATE_REPEAT 3

typedef enum{
	IR_TX_IDLE,
	IR_TX_HEADER,
	IR_TX_DATA,
	IR_TX_REP,
}IR_TX_STATE;

#define IR_NEC_BIT_NUM 		    8
#define IR_NEC_MAGIN_US 		200
#define IR_NEC_TM_PRE_US 		13500
#define IR_NEC_D1_TM_US 		2250
#define IR_NEC_D0_TM_US 		1120
#define IR_NEC_REP_TM1_US 	    20000
#define IR_NEC_REP_TM2_US 		11250
#define IR_NEC_REP_LOW_US 		2250
#define IR_NEC_REP_CYCLE 		110000
#define IR_NEC_HEADER_HIGH_US 	9000
#define IR_NEC_HEADER_LOW_US 	4500
#define IR_NEC_DATA_HIGH_US 	560
#define IR_NEC_DATA_LOW_1_US 	1690
#define IR_NEC_DATA_LOW_0_US 	560
  
extern void ir_rx_init(void);
extern void ir_rx_disable();
extern void ir_rx_enable();
extern void ir_tx_init();
extern uint8 get_ir_tx_status();
extern void ir_tx_handler();
#if GEN_IR_CLK_FROM_DMA_IIS
extern char i2s_carrier_duty_set(uint32 duty_24bit);// change the iis data tx duty (range: 0-0xffffff)
#endif

#endif
