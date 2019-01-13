/* 
 * Demo code for driving digital RGB(W) LEDs using the ESP32's RMT peripheral
 *
 * Modifications Copyright (c) 2017 Martin F. Falatic
 *
 * Based on public domain code created 19 Nov 2016 by Chris Osborn <fozztexx@fozztexx.com>
 * http://insentricity.com
 *
 */
/* 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <esp_log.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include "esp32_digital_led_lib.h"

#ifndef __cplusplus
#define nullptr  NULL
#endif

#define HIGH 1
#define LOW 0
#define OUTPUT GPIO_MODE_OUTPUT
#define INPUT GPIO_MODE_INPUT

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

#define min(a, b)  ((a) < (b) ? (a) : (b))
#define max(a, b)  ((a) > (b) ? (a) : (b))
#define floor(a)   ((int)(a))
#define ceil(a)    ((int)((int)(a) < (a) ? (a+1) : (a)))

strand_t STRANDS[] = { // Avoid using any of the strapping pins on the ESP32
//  {.rmtChannel = 1, .gpioNum = 17, .ledType = LED_WS2812B_V3, .brightLimit = 32, .numPixels =  93,
//   .pixels = nullptr, ._stateVars = nullptr},
//  {.rmtChannel = 2, .gpioNum = 18, .ledType = LED_WS2812B_V3, .brightLimit = 32, .numPixels =  93,
//   .pixels = nullptr, ._stateVars = nullptr},
//  {.rmtChannel = 3, .gpioNum = 19, .ledType = LED_WS2812B_V3, .brightLimit = 32, .numPixels =  64,
//   .pixels = nullptr, ._stateVars = nullptr},
//{.rmtChannel = 0, .gpioNum = 16, .ledType = LED_WS2812B_V3, .brightLimit = 32, .numPixels = 256,
// .pixels = nullptr, ._stateVars = nullptr},
  {.rmtChannel = 0, .gpioNum = 19, .ledType = LED_SK6812W_V1, .brightLimit = 32, .numPixels = 8,
   .pixels = nullptr, ._stateVars = nullptr}, // Adafruit NeoPixel stick #2868
//  {.rmtChannel = 0, .gpioNum = 16, .ledType = LED_WS2813_V2, .brightLimit = 32, .numPixels = 3,
//   .pixels = nullptr, ._stateVars = nullptr},
};

int STRANDCNT = sizeof(STRANDS)/sizeof(STRANDS[0]);

void gpioSetup(int gpioNum, int gpioMode, int gpioVal) {
  #if defined(ARDUINO) && ARDUINO >= 100
    pinMode (gpioNum, gpioMode);
    digitalWrite (gpioNum, gpioVal);
  #elif defined(ESP_PLATFORM)
    gpio_num_t gpioNumNative = (gpio_num_t)(gpioNum);
    gpio_mode_t gpioModeNative = (gpio_mode_t)(gpioMode);
    gpio_pad_select_gpio(gpioNumNative);
    gpio_set_direction(gpioNumNative, gpioModeNative);
    gpio_set_level(gpioNumNative, gpioVal);
  #endif
}


void show_colours(strand_t * pStrand) {
	strand_t *s = pStrand;
  if (s->ledType != LED_SK6812W_V1) { // 3-colours
    for (uint16_t i = 0; i < s->numPixels; i++) {
        if (i%4 == 0) s->pixels[i] = pixelFromRGB(12, 0, 0);
        if (i%4 == 1) s->pixels[i] = pixelFromRGB(0, 12, 0);
        if (i%4 == 2) s->pixels[i] = pixelFromRGB(0, 0, 16);
        //if (i%4 == 3) s->pixels[i] = pixelFromRGBW(0, 0, 0, 8);
    }
  }
  else { // also has a white led
    for (uint16_t i = 0; i < s->numPixels; i++) {
        if (i%4 == 0) s->pixels[i] = pixelFromRGBW(12, 0, 0, 0);
        if (i%4 == 1) s->pixels[i] = pixelFromRGBW(0, 12, 0, 0);
        if (i%4 == 2) s->pixels[i] = pixelFromRGBW(0, 0, 16, 0);
        if (i%4 == 3) s->pixels[i] = pixelFromRGBW(0, 0, 0, 8);
    }
  }
	digitalLeds_updatePixels(s);
}

void app_main() {
  gpioSetup(16, OUTPUT, LOW);
  if (digitalLeds_initStrands(STRANDS, STRANDCNT)) {
    ets_printf("Init FAILURE: halting\n");
    while (true) {};
  }
  strand_t *pStrand = &STRANDS[0];
  digitalLeds_resetPixels(pStrand);
  show_colours(pStrand);
  while (true) {
    vTaskDelay(100);
  }
  vTaskDelete(NULL);
}
