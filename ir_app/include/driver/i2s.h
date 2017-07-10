#include "driver/i2s_reg.h"
#include "osapi.h"
#include "os_type.h"
#include "driver/sdio_slv.h"

#define FUNC_I2SO_DATA 1


//I2S_DATA_FREQ = 160M/I2S_BCK_DIV_PRAR/I2S_CLKM_DIV_PARA/24 = 38K
#define I2S_BCK_DIV_PRAR  25
#define I2S_CLKM_DIV_PARA 7

#define CONF_RXLINK_ADDR(addr)      CLEAR_PERI_REG_MASK(SLC_RX_LINK,SLC_RXLINK_DESCADDR_MASK);\
    SET_PERI_REG_MASK(SLC_RX_LINK, ((uint32)(addr)) & SLC_RXLINK_DESCADDR_MASK)
#define CONF_TXLINK_ADDR(addr)      CLEAR_PERI_REG_MASK(SLC_TX_LINK,SLC_TXLINK_DESCADDR_MASK);\
    SET_PERI_REG_MASK(SLC_TX_LINK, ((uint32)(addr)) & SLC_TXLINK_DESCADDR_MASK)
    
#define START_RXLINK()  SET_PERI_REG_MASK(SLC_RX_LINK, SLC_RXLINK_START)
#define START_TXLINK()  SET_PERI_REG_MASK(SLC_TX_LINK, SLC_TXLINK_START)

void i2s_dma_mode_init(void);

//Initialize the I2S module
void i2s_init();

//create DMA buffer descriptors
void creat_one_link(uint8 own, uint8 eof,uint8 sub_sof, uint16 size, uint16 length,
                       uint32* buf_ptr, struct sdio_queue* nxt_ptr, struct sdio_queue* i2s_queue);

//Initialize the SLC module for DMA function
void slc_init();

void i2s_stop(void);
void i2s_start(void);
char i2s_carrier_duty_set(uint32 duty_24bit);// change the iis data tx duty (range: 0-0xffffff)

