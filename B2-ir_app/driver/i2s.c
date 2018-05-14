//#include "driver/spi.h"
//#include "driver/spi_overlap.h"


#include "osapi.h"
#include "os_type.h"
#include "gpio.h"

#include "driver/i2s_reg.h"
#include "driver/slc_register.h"
#include "driver/sdio_slv.h"
#include "driver/i2s.h"

#define i2c_bbpll                            0x67
#define i2c_bbpll_en_audio_clock_out            4
#define i2c_bbpll_en_audio_clock_out_msb        7
#define i2c_bbpll_en_audio_clock_out_lsb        7
#define i2c_bbpll_hostid                        4

#define i2c_writeReg_Mask(block, host_id, reg_add, Msb, Lsb, indata)  rom_i2c_writeReg_Mask(block, host_id, reg_add, Msb, Lsb, indata)
#define i2c_readReg_Mask(block, host_id, reg_add, Msb, Lsb)  rom_i2c_readReg_Mask(block, host_id, reg_add, Msb, Lsb)
#define i2c_writeReg_Mask_def(block, reg_add, indata) \
      i2c_writeReg_Mask(block, block##_hostid,  reg_add,  reg_add##_msb,  reg_add##_lsb,  indata)
#define i2c_readReg_Mask_def(block, reg_add) \
      i2c_readReg_Mask(block, block##_hostid,  reg_add,  reg_add##_msb,  reg_add##_lsb)

#define IIS_RX_BUF_LEN  32  //unit Byte
#define IIS_TX_BUF_LEN  32  //unit Byte
//#define n  2

uint32 i2s_rx_buff1[IIS_RX_BUF_LEN/4];
uint32 i2s_rx_buff2[IIS_RX_BUF_LEN/4];

struct sdio_queue i2s_rx_queue1, i2s_rx_queue2;

//create DMA buffer descriptors, unit of either size or length here is byte. 
//More details in I2S documents.
void ICACHE_FLASH_ATTR
creat_one_link(uint8 own, uint8 eof,uint8 sub_sof, uint16 size, uint16 length,
                       uint32* buf_ptr, struct sdio_queue* nxt_ptr, struct sdio_queue* i2s_queue)
{
    unsigned int link_data0;
    unsigned int link_data1;
    unsigned int link_data2;
    unsigned int start_addr;

    i2s_queue->owner=own;
    i2s_queue->eof= eof;
    i2s_queue->sub_sof=sub_sof;
    i2s_queue->datalen=length;
    i2s_queue->blocksize=size;
    i2s_queue->buf_ptr=(uint32)buf_ptr;
    i2s_queue->next_link_ptr=(uint32)nxt_ptr;
    i2s_queue->unused=0;
}

//Initialize the I2S module
//More Registor details in I2S documents.
void ICACHE_FLASH_ATTR
i2s_init()
{
    //CONFIG I2S TX PIN FUNC
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_I2SO_DATA);

    //Enable a 160MHz clock to i2s subsystem
    i2c_writeReg_Mask_def(i2c_bbpll, i2c_bbpll_en_audio_clock_out, 1);

    //reset I2S
    CLEAR_PERI_REG_MASK(I2SCONF,I2S_I2S_RESET_MASK);
    SET_PERI_REG_MASK(I2SCONF,I2S_I2S_RESET_MASK);
    CLEAR_PERI_REG_MASK(I2SCONF,I2S_I2S_RESET_MASK);

    //Enable FIFO in i2s module
    SET_PERI_REG_MASK(I2S_FIFO_CONF, I2S_I2S_DSCR_EN);

    //set I2S_FIFO
    //set rx,tx data size, both are "24-bit  half data discontinue" here
    SET_PERI_REG_MASK(I2S_FIFO_CONF, ((0x03&I2S_I2S_TX_FIFO_MOD)<<I2S_I2S_TX_FIFO_MOD_S));

    //set I2S_CHAN 
    //set rx,tx channel mode, both are "two channel" here
    SET_PERI_REG_MASK(I2SCONF_CHAN, ((0x1&I2S_TX_CHAN_MOD) << I2S_TX_CHAN_MOD_S));
    
    //trans master&rece slave mode,
    //MSB_shift,right_first,MSB_right,
    //use I2S clock divider to produce a 32KHz Sample Rate

    CLEAR_PERI_REG_MASK(I2SCONF, I2S_TRANS_SLAVE_MOD|
                        (I2S_BITS_MOD<<I2S_BITS_MOD_S)|
                        (I2S_BCK_DIV_NUM <<I2S_BCK_DIV_NUM_S)|
                                        (I2S_CLKM_DIV_NUM<<I2S_CLKM_DIV_NUM_S));
    //I2S_DATA_FREQ = 160M/I2S_BCK_DIV_PRAR/I2S_CLKM_DIV_PARA/24
    SET_PERI_REG_MASK(I2SCONF, I2S_RIGHT_FIRST|I2S_MSB_RIGHT|I2S_RECE_SLAVE_MOD|
                        I2S_RECE_MSB_SHIFT|I2S_TRANS_MSB_SHIFT|
                        ((I2S_BCK_DIV_PRAR&I2S_BCK_DIV_NUM )<<I2S_BCK_DIV_NUM_S)|
                        ((I2S_CLKM_DIV_PARA&I2S_CLKM_DIV_NUM)<<I2S_CLKM_DIV_NUM_S)|
                        (8<<I2S_BITS_MOD_S));

    //clear int
    SET_PERI_REG_MASK(I2SINT_CLR,   I2S_I2S_TX_REMPTY_INT_CLR|I2S_I2S_TX_WFULL_INT_CLR|
    I2S_I2S_PUT_DATA_INT_CLR|I2S_I2S_TAKE_DATA_INT_CLR);
    CLEAR_PERI_REG_MASK(I2SINT_CLR,   I2S_I2S_TX_REMPTY_INT_CLR|I2S_I2S_TX_WFULL_INT_CLR|I2S_I2S_RX_REMPTY_INT_CLR|
    I2S_I2S_RX_WFULL_INT_CLR|I2S_I2S_PUT_DATA_INT_CLR|I2S_I2S_TAKE_DATA_INT_CLR);


    //enable int
    SET_PERI_REG_MASK(I2SINT_ENA,   I2S_I2S_TX_REMPTY_INT_ENA|I2S_I2S_TX_WFULL_INT_ENA|
    I2S_I2S_TX_PUT_DATA_INT_ENA);
}

//Initialize the SLC module for DMA function
void ICACHE_FLASH_ATTR
slc_init()
{
    //Reset DMA
    SET_PERI_REG_MASK(SLC_CONF0, SLC_TXLINK_RST);
    CLEAR_PERI_REG_MASK(SLC_CONF0, SLC_TXLINK_RST);

    //Enable and configure DMA
    CLEAR_PERI_REG_MASK(SLC_CONF0, (SLC_MODE<<SLC_MODE_S));
    SET_PERI_REG_MASK(SLC_CONF0,(1<<SLC_MODE_S));
    CLEAR_PERI_REG_MASK(SLC_RX_DSCR_CONF,SLC_INFOR_NO_REPLACE|SLC_TOKEN_NO_REPLACE);//|0xfe
}


void ICACHE_FLASH_ATTR
i2s_dma_mode_init(void)
{
    uint32 i;
    slc_init();
    //To receive data from the I2S module, counter-intuitively we use the TXLINK part, not the RXLINK part.
    //Vice versa.
    //Note:At the transimitter side,the size of the DMAs can not be smaller than 128*4 bytes which are the
    //size of the I2S FIFO.
    //Note:At the receiver side,the number of the DMAs can not be smaller than 3 which is limited by the 
    //hardware. 
    creat_one_link(1,1,0,IIS_RX_BUF_LEN,IIS_RX_BUF_LEN,i2s_rx_buff1,&i2s_rx_queue2,&i2s_rx_queue1); 
    creat_one_link(1,1,0,IIS_RX_BUF_LEN,IIS_RX_BUF_LEN,i2s_rx_buff2,&i2s_rx_queue1,&i2s_rx_queue2); 
    //config rx&tx link to hardware
    CONF_RXLINK_ADDR(&i2s_rx_queue1);
    //config rx control, start  
    START_RXLINK();
    os_printf("i2s_init start\r\n");    
    i2s_init(); //start

}

void i2s_start(void)
{
    SET_PERI_REG_MASK(I2SCONF,I2S_I2S_TX_START);
    os_printf("i2s start\r\n");
}
void i2s_stop(void)
{
    CLEAR_PERI_REG_MASK(I2SCONF,I2S_I2S_TX_START);
    os_printf("i2s_init stop\r\n");
}

// change the iis data tx duty (range: 0-0xffffff)
//return: 0:right   -1:err
char ICACHE_FLASH_ATTR 
i2s_carrier_duty_set(uint32 duty_24bit)
{

    if (duty_24bit > 0xffffff) {
        os_printf("Duty must less than 0xffffff!\r\n");
        return -1;
    }
    
    uint16 i,j;
    uint32 val,val1;
    val = duty_24bit<<8;
    val1 = duty_24bit<<8;
    //generate data  
    for(j=0;j<IIS_RX_BUF_LEN/4;j++){
        i2s_rx_buff1[j]=val1;
        i2s_rx_buff2[j]=val1;
    }
    return 0;
}

