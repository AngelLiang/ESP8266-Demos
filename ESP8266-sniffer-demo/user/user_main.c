#include "driver/uart.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "gpio.h"
#include "mem.h"
#include "user_interface.h"
#include "sniffer.h"

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR
user_rf_pre_init(void)
{

}


void ICACHE_FLASH_ATTR
system_init_done(void)
{
	sniffer_init_in_system_init_done();
}

void ICACHE_FLASH_ATTR
user_init()
{
    uart_init(115200, 115200);
    os_printf("\r\n\r\nSDK version:%s\n", system_get_sdk_version());
    
    sniffer_init();

    // Continue to 'sniffer_system_init_done'
    system_init_done_cb(system_init_done);
}
