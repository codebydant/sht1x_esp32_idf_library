#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp32/clk.h"
#include "esp32/rom/rtc.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_timer.h"
#include "esp_tls.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "nvs_flash.h"

#include "freertos/queue.h"

#include <sys/time.h>
#include <time.h>

#include "soc/apb_ctrl_reg.h"
#include "soc/dport_reg.h"
#include "soc/efuse_periph.h"
#include "soc/frc_timer_reg.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "xtensa/core-macros.h"

#include <inttypes.h>

#define HIGH 1
#define LOW 0

float readTempC(int, int);          // calc temp in celcius
float readTempF(int, int);          // calculate temp in farhenite
float readHumidity(int, int);       // calculate relative humidity
int readTemperatureRaw(int, int);   // get temperature raw data
int getHumidityfrmSensor(int, int); // get humidity raw data
void sendCommandSHT(int, int, int); // send command to start measurement
void waitForResult(int);            // wait till sensor has finished measuring data
void skipCRC(int, int);             // skip the CRC check
int getData16SHT(int data, int clk);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);
void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
esp_err_t sht1x_init(gpio_num_t data_pin, gpio_num_t clc_pin);

/*
shiftOut function: https://forum.arduino.cc/index.php?topic=45529.0 (lloyddean)
shiftIn function: https://forum.arduino.cc/index.php?topic=391376.0 (CrossRoads)
*/

/*
 * Harware configuration:
 *
 *    +-----------------+     +----------+
 *    | ESP8266 / ESP32 |     | SHT3x    |
 *    |                 |     |          |
 *    |   GPIO 14 (CLK) ------> SCK      |
 *    |   GPIO 13 (SDA) <-----> SDA      |
 *    +-----------------+     +----------+
 */
