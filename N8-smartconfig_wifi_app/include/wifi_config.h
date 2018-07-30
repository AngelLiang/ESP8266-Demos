/*
 * wifi_config.h
 */

#ifndef _WIFI_CONFIG_H_
#define _WIFI_CONFIG_H_

//***********************************************************************/
// 使用下面的wifi配置进行连接
// 与下面的 SMARTCONFIG_ENABLE 二选一
//#define WIFI_SSID_ENABLE

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "WIFI_PASS"

//***********************************************************************/
// 上电时使用smartconfig进行wifi配置，然后过一段时间自动连接之前配置好的wifi
// 与上面的 WIFI_SSID_ENABLE 二选一
#define SMARTCONFIG_ENABLE

//***********************************************************************/

/* wifi led 引脚配置 */
#define WIFI_STATUS_LED_PIN         0

//***********************************************************************/
// 下面代码不需要变动
// define
#define GPIO_HIGH(x)	GPIO_OUTPUT_SET(x, 1)
#define GPIO_LOW(x)		GPIO_OUTPUT_SET(x, 0)
// GPIO reverse
#define GPIO_REV(x)		GPIO_OUTPUT_SET(x, (1-GPIO_INPUT_GET(x)))

#define IS_RUNNING		1
#define IS_STOP			0
//***********************************************************************/

#endif /* N8_SMARTCONFIG_WIFI_APP_INCLUDE_WIFI_CONFIG_H_ */
