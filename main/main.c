/* SHT1X Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

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

#include "include/sht1x.h"

#define _DATA_PIN 27
#define _SCK_PIN 2

void app_main(void) {

  gpio_num_t data_pin = _DATA_PIN;
  gpio_num_t clc_pin = _SCK_PIN;

  sht1x_init(data_pin, clc_pin);
}
