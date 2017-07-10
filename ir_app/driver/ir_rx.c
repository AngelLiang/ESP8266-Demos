
#include "eagle_soc.h"
#include "c_types.h"
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"

#include"driver/ringbuf.h"
#include "driver/ir_tx_rx.h"

/*
NOTICE:(only for NON-OS SDK)
You will find that all the functions which would be called in INTERRUPT HANDLER are defined in iram.
So that we don't add ICACHE_FLASH_ATTR attribute.
In other word,never call icache function in throughout the interrupt handler(include os_printf).
The rule is : 
Interrupt would pause all the functions including some cases in which irom cache is disabled(eg. flash operations)
If cache functions are called then, it would cause a conflict situation.
*/

/*When ir receiving a cmd data, push it into this RX RINGBUF*/
RINGBUF IR_RX_BUFF;
uint8 ir_rx_buf[RX_RCV_LEN];


//IR CODE ======================================================
static u32 t0 = 0;
static int ir_state  =  IR_NEC_STATE_IDLE;
static uint8 ir_rdy  =  0;
static int ir_repeat =  0;
static u32 ir_cmd    =  0;
static int cnt       =  0;

/******************************************************************************
 * FunctionName : nec_code_check
 * Description  : check whether the ir cmd and addr obey the protocol
 * Parameters   : u32 nec_code : nec ir code that received
 * Returns      :   true : cmd check ok  ;  false : cmd check fail
*******************************************************************************/
//check if is a legal nec code 
bool nec_code_check(u32 nec_code)
{   
    u8 addr1,addr2,cmd1,cmd2;
    cmd2=(nec_code>>24) &0xff;     
    cmd1=(nec_code>>16) &0xff;   
    addr2=(nec_code>>8) &0xff;   
    addr1=(nec_code>>0) &0xff;   
    if( (cmd1==((~cmd2)&0xff)) ){
        //os_printf("check ok \n\n");
        return true;
    }else{
        //os_printf("wrong code \n\n");
        return false;
    }
}
/******************************************************************************
 * FunctionName : ir_intr_handler
 * Description     : ir rx state machine via gpio intr
 * Parameters    : NONE
 * Returns         :  NONE
*******************************************************************************/
static void ir_intr_handler()
{
    uint32 gpio_status,t_h,d,t_st;
    static u8 rep_flg;
    d = GPIO_INPUT_GET(GPIO_ID_PIN(IR_GPIO_IN_NUM)) ;
    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    t_h = system_get_time() - t0;
    t0 = system_get_time();
    //clear interrupt status 
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
    
    switch(ir_state){
        case IR_NEC_STATE_IDLE:  
            if(t_h<IR_NEC_TM_PRE_US+IR_NEC_MAGIN_US&&t_h>IR_NEC_TM_PRE_US-IR_NEC_MAGIN_US){
                ir_state=IR_NEC_STATE_CMD;
            }else{
            }
            break;
        
        case IR_NEC_STATE_CMD:
            if(t_h<IR_NEC_D1_TM_US+IR_NEC_MAGIN_US && t_h>IR_NEC_D1_TM_US-IR_NEC_MAGIN_US){
                ir_cmd =  (ir_cmd>>1)|( 0x1<<(IR_NEC_BIT_NUM*4-1) );
                //os_printf("1");
                cnt++;
            }else if(t_h<IR_NEC_D0_TM_US+IR_NEC_MAGIN_US && t_h>IR_NEC_D0_TM_US-IR_NEC_MAGIN_US){
                ir_cmd =  (ir_cmd>>1)|( 0x0<<(IR_NEC_BIT_NUM*4-1) );
                //os_printf("0");
                cnt++;
            }else{
                goto RESET_STATUS;
            }
            if(cnt==IR_NEC_BIT_NUM*4){
                if(nec_code_check(ir_cmd)){
                    //os_printf("ir rcvd:%02xh %02xh %02xh %02xh \r\n",ir_cmd&0xff,(ir_cmd>>8)&0xff,(ir_cmd>>16)&0xff,(ir_cmd>>24)&0xff);
            //push rcv data to ringbuf
              RINGBUF_Put(&IR_RX_BUFF,(ir_cmd>>16)&0xff);
                    ir_state=IR_NEC_STATE_REPEAT;
                    rep_flg=0;
                }else{
                    goto RESET_STATUS;    
                }
            }
            break;
        case IR_NEC_STATE_REPEAT:
            if(rep_flg==0){
                if(t_h>IR_NEC_REP_TM1_US  &&  t_h<IR_NEC_REP_TM1_US*8){
                    rep_flg=1;
                }else{
                    goto RESET_STATUS;
                }
            }else if(rep_flg==1){
                if(t_h<IR_NEC_REP_TM1_US+IR_NEC_MAGIN_US && IR_NEC_REP_TM2_US-IR_NEC_MAGIN_US){
                //os_printf("REPEAT: %d\n",ir_repeat);
                //push rcv data to ringbuf
                RINGBUF_Put(&IR_RX_BUFF,(ir_cmd>>16)&0xff);
                ir_repeat++;
                rep_flg=0;
                }else{
                    goto RESET_STATUS;
                }
            }else{
            }
            break;
        default: break;
        
RESET_STATUS:
        ir_state=IR_NEC_STATE_IDLE;
        cnt=0;
        ir_cmd=0;
        ir_repeat=0;
        rep_flg=0;
        
    }
}



/******************************************************************************
 * FunctionName : interrupt_serv
 * Description  : sample that check gpio intr source and run relative task
 * Parameters   : NONE
 * Returns      :   NONE
*******************************************************************************/
//gpio interrupt  function select
static void interrupt_serv(void)
{
    s32 gpio_status;
    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    if( (gpio_status>>IR_GPIO_IN_NUM)& BIT0 ){
        ir_intr_handler();
    }  
    //add else if for other gpio intr task
    else{
        os_printf("gpio num mismached   \n");
        GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
    }
}


void ICACHE_FLASH_ATTR ir_gpio_init(void)
{
    //Select pin as gpio
    PIN_FUNC_SELECT(IR_GPIO_IN_MUX, IR_GPIO_IN_FUNC);
    //set GPIO as input
    GPIO_DIS_OUTPUT(IR_GPIO_IN_NUM);
    PIN_PULLUP_DIS(IR_GPIO_IN_MUX);
    //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(IR_GPIO_IN_NUM));
    //set negitive edge interrupt
    gpio_pin_intr_state_set(IR_GPIO_IN_NUM,GPIO_PIN_INTR_NEGEDGE);
    //register interrupt handler
    ETS_GPIO_INTR_ATTACH(interrupt_serv,NULL);
    //enable gpio interrupt
    ETS_GPIO_INTR_ENABLE();
}



/******************************************************************************
 * FunctionName : ir_rx_init
 * Description  : init gpio function, attach interrupt serv function
 * Parameters   : NONE
 * Returns      :   NONE
*******************************************************************************/

void ICACHE_FLASH_ATTR
 ir_rx_init()
{
    //a ring buffer to keep ir rx data
    RINGBUF_Init(&IR_RX_BUFF,ir_rx_buf,sizeof(ir_rx_buf));
    //gpio configure for IR rx pin
    ir_gpio_init();
}

void ICACHE_FLASH_ATTR
 ir_rx_disable()
{
    gpio_pin_intr_state_set(GPIO_ID_PIN(IR_GPIO_IN_NUM),GPIO_PIN_INTR_DISABLE);
}

void ICACHE_FLASH_ATTR
 ir_rx_enable()
{
    gpio_pin_intr_state_set(GPIO_ID_PIN(IR_GPIO_IN_NUM),GPIO_PIN_INTR_NEGEDGE);
}

