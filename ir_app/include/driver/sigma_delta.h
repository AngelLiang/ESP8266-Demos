#ifndef _SIGMA_DELTA_H
#define _SIGMA_DELTA_H
#include "os_type.h"
#include"c_types.h"



void sigma_delta_setup(uint32 GPIO_MUX,uint32 GPIO_NUM,uint32 GPIO_FUNC);
void sigma_delta_gen_38k(uint32 GPIO_MUX,uint32 GPIO_NUM,uint32 GPIO_FUNC);
void     sigma_delta_close(uint32 GPIO_NUM);
void     set_sigma_target(uint8 target);
void     set_sigma_prescale(uint8 prescale);
void     set_sigma_duty_312KHz(uint8 duty);

#endif










