
#include "eagle_soc.h"
#include "c_types.h"
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"

#include "driver/i2s.h"
#include "driver/sigma_delta.h"
#include "driver/ir_tx_rx.h"
#include "driver/hw_timer.h"
/*Generally speaking , this is a ir tx demo of IR tx function( NEC CODE ,32 BIT LENGTH)*/

#define IR_NEC_TX_IO_MUX               IR_GPIO_OUT_MUX
#define IR_NEC_TX_GPIO_NUM      IR_GPIO_OUT_NUM

static os_timer_t  ir_tx_timer;
void ir_tx_handler();


typedef enum{
    IR_TX_ADDR_P,
    IR_TX_ADDR_N,
    IR_TX_DATA_P,
    IR_TX_DATA_N,
}IR_TX_SUB_STATE;

typedef enum{
     TX_BIT_CARRIER,
     TX_BIT_LOW,
}TX_BIT_STA;



/*
The standard NEC ir code is:
addr + ~addr + cmd + ~cmd
You can also set the addr and cmd independently if you set ir_tx_standard=0
*/
static u8 ir_tx_standard=1;  //standard_if
static u8 ir_tx_addr=0xee;   //addr code
static u8 ir_tx_addr2=0xee;
static u8 ir_tx_cmd=0x5a;   //cmd code
static u8 ir_tx_cmd2=0x5a;
static u8 ir_tx_rep=5;         //repeat number

#define IR_TX_OUTPUT_LOW(ir_out_gpio_num)  \
    gpio_output_set(0, 1<<ir_out_gpio_num, 1<<ir_out_gpio_num, 0)
#define IR_TX_OUTPUT_HIGH(ir_out_gpio_num) \
    gpio_output_set(1<<ir_out_gpio_num, 0, 1<<ir_out_gpio_num, 0)

#define IR_TX_SET_INACTIVE(io_num)   IR_TX_OUTPUT_HIGH(io_num)

#if GEN_IR_CLK_FROM_IIS
    /* MODE 1 : GENERATE IR CLK FROM IIS CLOCK */

    //#if GEN_IR_CLK_FROM_IIS
    #define TIMER_INTERVAL_US  63
    #define I2C_BASE                   0x60000D00
    #define I2S_BCK_DIV_NUM         0x0000003F
    #define I2S_BCK_DIV_NUM_S       22
    #define I2S_CLKM_DIV_NUM        0x0000003F
    #define I2S_CLKM_DIV_NUM_S      16
    #define I2S_BITS_MOD            0x0000000F
    #define I2S_BITS_MOD_S          12
    #define I2SCONF                 (DR_REG_I2S_BASE + 0x0008)
    #define DR_REG_I2S_BASE         (0x60000e00)
    
    #define U32 uint32
    #define i2c_bbpll                               0x67
    #define i2c_bbpll_en_audio_clock_out            4
    #define i2c_bbpll_en_audio_clock_out_msb        7
    #define i2c_bbpll_en_audio_clock_out_lsb        7
    #define i2c_bbpll_hostid                        4
    
    /******************************************************************************
     * FunctionName : ir_tx_carrier_clr
     * Description  : stop 38khz carrier clk and output low
     * Parameters   : NONE
     * Returns      :   NONE
    *******************************************************************************/
    void  ir_tx_carrier_clr()
    {
        PIN_FUNC_SELECT(IR_GPIO_OUT_MUX, IR_GPIO_OUT_FUNC); 
        IR_TX_SET_INACTIVE(IR_GPIO_OUT_NUM);
        WRITE_PERI_REG(0x60000e08, READ_PERI_REG(0x60000e08) & 0xfffffdff | (0x0<<8) ) ; //i2s clk stop
    }
    
    /******************************************************************************
     * FunctionName : gen_carrier_clk
     * Description  : gen 38khz carrier clk
     * Parameters   : NONE
     * Returns      :   NONE
    *******************************************************************************/
    void gen_carrier_clk()
    {
        
        //CONFIG AS I2S 
    #if (IR_NEC_TX_IO_MUX==PERIPHS_IO_MUX_GPIO2_U)
        //set i2s clk freq 
        WRITE_PERI_REG(I2SCONF,  READ_PERI_REG(I2SCONF) & 0xf0000fff|
                            ( (( 62&I2S_BCK_DIV_NUM )<<I2S_BCK_DIV_NUM_S)|
                            ((2&I2S_CLKM_DIV_NUM)<<I2S_CLKM_DIV_NUM_S)|
                            ((1&I2S_BITS_MOD  )   <<  I2S_BITS_MOD_S )  )  );
    
        WRITE_PERI_REG(IR_NEC_TX_IO_MUX, (READ_PERI_REG(IR_NEC_TX_IO_MUX)&0xfffffe0f)| (0x1<<4) );
        WRITE_PERI_REG(0x60000e08, READ_PERI_REG(0x60000e08) & 0xfffffdff | (0x1<<8) ); //i2s tx  start
    #endif
    
    #if (IR_NEC_TX_IO_MUX==PERIPHS_IO_MUX_MTMS_U)
        //set i2s clk freq 
        WRITE_PERI_REG(I2SCONF,  READ_PERI_REG(I2SCONF) & 0xf0000fff|
                            ( (( 62&I2S_BCK_DIV_NUM )<<I2S_BCK_DIV_NUM_S)|  
                            ((2&I2S_CLKM_DIV_NUM)<<I2S_CLKM_DIV_NUM_S)|   
                            ((1&I2S_BITS_MOD  )   <<  I2S_BITS_MOD_S )  )  );
    
        WRITE_PERI_REG(IR_NEC_TX_IO_MUX, (READ_PERI_REG(IR_NEC_TX_IO_MUX)&0xfffffe0f)| (0x1<<4) );
        WRITE_PERI_REG(0x60000e08, READ_PERI_REG(0x60000e08) & 0xfffffdff | (0x2<<8) ) ;//i2s rx  start
    #endif
    #if (IR_NEC_TX_IO_MUX==PERIPHS_IO_MUX_MTCK_U)
        //set i2s clk freq 
        WRITE_PERI_REG(I2SCONF,  READ_PERI_REG(I2SCONF) & 0xf0000fff|
                            ( (( 63&I2S_BCK_DIV_NUM )<<I2S_BCK_DIV_NUM_S)|
                            ((63&I2S_CLKM_DIV_NUM)<<I2S_CLKM_DIV_NUM_S)| 
                            ((1&I2S_BITS_MOD  )   <<  I2S_BITS_MOD_S )  )  );
    
        WRITE_PERI_REG(IR_NEC_TX_IO_MUX, (READ_PERI_REG(IR_NEC_TX_IO_MUX)&0xfffffe0f)| (0x1<<4) );
        WRITE_PERI_REG(0x60000e08, READ_PERI_REG(0x60000e08) & 0xfffffdff | (0x2<<8) ) ;//i2s rx  start
    #endif
    #if (IR_NEC_TX_IO_MUX==PERIPHS_IO_MUX_MTDO_U)
        //set i2s clk freq 
        WRITE_PERI_REG(I2SCONF,  READ_PERI_REG(I2SCONF) & 0xf0000fff|
                            ( (( 63&I2S_BCK_DIV_NUM )<<I2S_BCK_DIV_NUM_S)|
                            ((63&I2S_CLKM_DIV_NUM)<<I2S_CLKM_DIV_NUM_S)|
                            ((1&I2S_BITS_MOD  )   <<  I2S_BITS_MOD_S )));
    
        WRITE_PERI_REG(IR_NEC_TX_IO_MUX, (READ_PERI_REG(IR_NEC_TX_IO_MUX)&0xfffffe0f)| (0x1<<4) );
        WRITE_PERI_REG(0x60000e08, READ_PERI_REG(0x60000e08) & 0xfffffdff | (0x1<<8) ); //i2s tx  start
    #endif
    }
    


#elif GEN_IR_CLK_FROM_DMA_IIS
    /* MODE 2 : GENERATE IR CLK FROM I2S data line */

    /******************************************************************************
     * FunctionName : gen_carrier_clk
     * Description  : gen 38khz carrier clk
     * Parameters   : NONE
     * Returns      :   NONE
    *******************************************************************************/
    void gen_carrier_clk()
    {
        
        #if (IR_NEC_TX_IO_MUX == PERIPHS_IO_MUX_U0RXD_U)
        //CONFIG I2S TX PIN FUNC
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_I2SO_DATA);
        
        i2s_start();  //START
        #else
        os_printf("Please select right GPIO for IIS TX data\r\n");
        #endif
    }
    
    /******************************************************************************
     * FunctionName : ir_tx_carrier_clr
     * Description  : stop 38khz carrier clk and output low
     * Parameters   : NONE
     * Returns      :   NONE
    *******************************************************************************/
    void ir_tx_carrier_clr(void)
    {
        PIN_FUNC_SELECT(IR_GPIO_OUT_MUX, IR_GPIO_OUT_FUNC); 
        IR_TX_SET_INACTIVE(IR_GPIO_OUT_NUM);
        
        i2s_stop(); //stop
    }
    
#elif GEN_IR_CLK_FROM_GPIO_SIGMA_DELAT
    /* MODE 3 : GENERATE IR CLK FROM SIGMA-DELTA MODULE */
    extern void sigma_delta_close(uint32 GPIO_NUM);
    extern void sigma_delta_gen_38k(uint32 GPIO_MUX,uint32 GPIO_NUM,uint32 GPIO_FUNC);
    
    void  ir_tx_carrier_clr()
    {
        sigma_delta_close(IR_NEC_TX_GPIO_NUM);
        IR_TX_SET_INACTIVE(IR_GPIO_OUT_NUM);
    }
    
    void gen_carrier_clk()
    {
        sigma_delta_gen_38k(IR_NEC_TX_IO_MUX,IR_NEC_TX_GPIO_NUM,IR_GPIO_OUT_FUNC);
    }
    
#endif


/******************************************************************************
 * FunctionName : set_tx_data
 * Description  : set IR_NEC addr and cmd
 * Parameters   : u8 tx_addr : nec addr code
                         u8 tx_cmd  : nec cmd code
                         u8 tx_rep   : repeat times
 * Returns      :   NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
set_tx_data(u8 tx_addr,u8 tx_cmd,u8 tx_rep)
{
    ir_tx_standard=1;
    ir_tx_addr=tx_addr;
    ir_tx_cmd=tx_cmd;
    ir_tx_rep=tx_rep;
}

void ICACHE_FLASH_ATTR
set_tx_data_custom(u8 tx_addr,u8 tx_addr2,u8 tx_cmd,u8 tx_cmd2,u8 tx_rep)
{
    ir_tx_standard=0;
    ir_tx_addr=tx_addr;
    ir_tx_addr2=tx_addr2;
    ir_tx_cmd=tx_cmd;
    ir_tx_cmd2=tx_cmd2;
    ir_tx_rep=tx_rep;
}

/******************************************************************************
 * FunctionName : ir_tx_handler
 * Description  :  IR_NEC tx ,state machine
 * Parameters   : NONE
 * Returns      :   NONEe
*******************************************************************************/

static IR_TX_STATE ir_tx_state = IR_TX_IDLE;

void ir_tx_handler()//u8 ir_tx_addr, u8 ir_tx_data,u16 ir_tx_rep)
{
	u32 t_expire=0;
	static u32 rep_expire_us=110000;//for nec 32bit mode
	static u16 data_tmp=0;
	static u8 ir_tx_bit_num=0;
	static  u8 ir_bit_state=TX_BIT_CARRIER;
	#if IR_TX_STATUS_MACHINE_HW_TIMER
		hw_timer_stop();
	#else
		os_timer_disarm(&ir_tx_timer);
	#endif
    switch(ir_tx_state)
    {
        case IR_TX_IDLE: 
        {
            gen_carrier_clk();
            ir_tx_state = IR_TX_HEADER;
            #if IR_TX_STATUS_MACHINE_HW_TIMER
              hw_timer_arm(IR_NEC_HEADER_HIGH_US);
              hw_timer_start();
            #else
            os_timer_arm_us(&ir_tx_timer, IR_NEC_HEADER_HIGH_US, 0);//143  9ms
            #endif
            break;
        }
        case IR_TX_HEADER:
        {
            ir_tx_carrier_clr();
            #if IR_TX_STATUS_MACHINE_HW_TIMER
                hw_timer_arm(IR_NEC_HEADER_LOW_US);
                hw_timer_start();
            #else
                os_timer_arm_us(&ir_tx_timer, IR_NEC_HEADER_LOW_US, 0);//71 ,4.5ms
            #endif
            ir_tx_state=IR_TX_DATA;
            ir_bit_state=TX_BIT_CARRIER;
            data_tmp=ir_tx_addr;
            rep_expire_us-=13500;
            break;
        }
        case IR_TX_DATA:
        {
            if(ir_bit_state==TX_BIT_CARRIER){
                t_expire=IR_NEC_DATA_HIGH_US;
                ir_bit_state=TX_BIT_LOW;
                gen_carrier_clk();
            }else if(ir_bit_state==TX_BIT_LOW){
                    ir_tx_carrier_clr();
                    if( (data_tmp>>(ir_tx_bit_num% IR_NEC_BIT_NUM ))&0x1){
                        t_expire=IR_NEC_DATA_LOW_1_US;
                    } else{
                        t_expire=IR_NEC_DATA_LOW_0_US; 
                    }
                    ir_tx_bit_num++;
                    if(ir_tx_bit_num==IR_NEC_BIT_NUM ){
                        if(ir_tx_standard) data_tmp=(~ir_tx_addr);
                        else data_tmp = ir_tx_addr2;
                    }else if(ir_tx_bit_num==IR_NEC_BIT_NUM*2 ){
                        if(ir_tx_standard) data_tmp=ir_tx_cmd;
                        else data_tmp=ir_tx_cmd;
                    }else if(ir_tx_bit_num==IR_NEC_BIT_NUM*3){
                        if(ir_tx_standard) data_tmp=(~ir_tx_cmd);
                        else data_tmp=ir_tx_cmd2;
                    }else if((ir_tx_bit_num==(IR_NEC_BIT_NUM*4+1))){
                        //clean up state for next or for repeat
                        ir_tx_state=IR_TX_IDLE;
                        ir_tx_bit_num=0;
                        ir_bit_state=TX_BIT_CARRIER;
                            #if IR_TX_STATUS_MACHINE_HW_TIMER
                                hw_timer_stop();
                            #else
                                os_timer_disarm(&ir_tx_timer);
                            #endif
                        if(ir_tx_rep>0){   
                           t_expire= (rep_expire_us -5); 
                           #if IR_TX_STATUS_MACHINE_HW_TIMER
                               hw_timer_arm(t_expire);
                               hw_timer_start();
                           #else
                                os_timer_arm_us(&ir_tx_timer, t_expire, 0);//71
                            #endif
                            rep_expire_us=IR_NEC_REP_CYCLE;
                             ir_tx_state=IR_TX_REP;
                        }else{
                            rep_expire_us=IR_NEC_REP_CYCLE;
                            ir_tx_state=IR_TX_IDLE;
                        }
                        break;
                        
                    }
                    ir_bit_state=TX_BIT_CARRIER;
                }else{
            }
            rep_expire_us-=t_expire;
            #if IR_TX_STATUS_MACHINE_HW_TIMER
                hw_timer_arm(t_expire);
                hw_timer_start();
            #else
                os_timer_arm_us(&ir_tx_timer, t_expire, 0);
            #endif
            break;
        }
        case IR_TX_REP:
        {
            if(ir_tx_rep>0){   
                if(ir_tx_bit_num==0){
                    gen_carrier_clk();
                    t_expire=IR_NEC_HEADER_HIGH_US ; 
                }else if(ir_tx_bit_num==1){
                    ir_tx_carrier_clr();
                     t_expire=IR_NEC_D1_TM_US ; 
                }else if(ir_tx_bit_num==2){
                     gen_carrier_clk();
                      t_expire=750 ; 
                }else if(ir_tx_bit_num==3) {
                    ir_tx_carrier_clr();
                    ir_tx_rep--;
                    if(ir_tx_rep>0){
                        t_expire=rep_expire_us ; 
                        rep_expire_us=IR_NEC_REP_CYCLE;
                    }else{
                        //os_printf("rep = 0 \n");
                        ir_tx_bit_num=0;
                        rep_expire_us=IR_NEC_REP_CYCLE;
                        ir_tx_state=IR_TX_IDLE;
                        ir_bit_state=TX_BIT_CARRIER;
                        #if IR_TX_STATUS_MACHINE_HW_TIMER
                           hw_timer_stop();
                        #else
                           os_timer_disarm(&ir_tx_timer);
                        #endif
                        os_printf("rep = 0  end \n");
                        break;
                    }
                }
                else{
                }
                ir_tx_bit_num++;//bit num reuse for repeat wave form
                if(ir_tx_bit_num==4) {
                    ir_tx_bit_num=0;
                    rep_expire_us=IR_NEC_REP_CYCLE;
                }else{
                    rep_expire_us-=  t_expire;
                }
                #if IR_TX_STATUS_MACHINE_HW_TIMER
                    hw_timer_arm(t_expire);
                    hw_timer_start();
                #else
                    os_timer_arm_us(&ir_tx_timer, t_expire, 0);//143
                #endif
            }
            break;
        }
        default: break;
    }
}

uint8 ICACHE_FLASH_ATTR
get_ir_tx_status()
{
    return ir_tx_state;

}
extern void test_ir_nec_tx();

void ICACHE_FLASH_ATTR
ir_tx_init()
{
    /*NOTE: NEC IR code tx function need a us-accurate timer to generate the tx logic waveform*/
    /* So we call system_timer_reinit() to reset the os_timer API's clock*/
    /* Also , Remember to #define USE_US_TIMER to enable  os_timer_arm_us function*/
  
    
    rom_i2c_writeReg_Mask(i2c_bbpll, i2c_bbpll_hostid, i2c_bbpll_en_audio_clock_out, i2c_bbpll_en_audio_clock_out_msb, i2c_bbpll_en_audio_clock_out_lsb, 1);

#if IR_TX_STATUS_MACHINE_HW_TIMER
     hw_timer_init(0,0);//0:FRC1 INTR LEVEL 0:NOT AUTOLOAD
     hw_timer_set_func(ir_tx_handler);
     hw_timer_stop();
     os_printf("Ir tx status machine Clk is hw_timer\n");
#else
    system_timer_reinit();
    os_timer_disarm(&ir_tx_timer);
    os_timer_setfn(&ir_tx_timer, (os_timer_func_t *)ir_tx_handler, NULL);
    os_printf("Ir tx status machine Clk is soft_timer\n");
#endif  
    //init code for mode 2;
    #if GEN_IR_CLK_FROM_DMA_IIS
    i2s_carrier_duty_set(0xfff);//0xfff:50% : THE DATA WIDTH IS SET 24BIT, SO IF WE SET 0XFFF,THAT IS A HALF-DUTY.
    i2s_dma_mode_init();
    #endif
}

