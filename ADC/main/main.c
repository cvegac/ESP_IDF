#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/timers.h"
#include "driver/adc.h"

#define led1 2
#define ledR 33
#define ledG 25
#define ledB 26

uint8_t led_level = 0;
static const char *TAG = "MyModule";
TimerHandle_t xTimers;
int interval = 20;
int timerId = 1;
int adc_val = 0;

esp_err_t init_led(void);
esp_err_t blink_led(void);
esp_err_t set_timer(void);
esp_err_t set_adc(void);

void vTimerCallback(TimerHandle_t pxTimer);

void app_main(void)
{
  init_led();
  set_timer();
}

esp_err_t init_led(void)
{
  gpio_reset_pin(led1);
  gpio_set_direction(led1, GPIO_MODE_OUTPUT);
  gpio_reset_pin(ledR);
  gpio_set_direction(ledR, GPIO_MODE_OUTPUT);
  gpio_reset_pin(ledG);
  gpio_set_direction(ledG, GPIO_MODE_OUTPUT);
  gpio_reset_pin(ledB);
  gpio_set_direction(ledB, GPIO_MODE_OUTPUT);
  return ESP_OK;
}

esp_err_t blink_led(void)
{
  led_level = !led_level;
  gpio_set_level(led1, led_level);
  return ESP_OK;
}

esp_err_t set_timer(void)
{
  ESP_LOGI(TAG, "Timer init configuration.");
  xTimers = xTimerCreate("Timer",                 // Just a text name, not used by the kernel.
                         pdMS_TO_TICKS(interval), // The timer period in ticks.
                         pdTRUE,                  // The timers will auto-reload themselves when they expire.
                         (void *)timerId,         // Assign each timer a unique id equal to its array index.
                         vTimerCallback           // Each timer calls the same callback when it expires.
  );

  if (xTimers == NULL)
  {
    ESP_LOGI(TAG, "The timer was not created.");
  }
  else
  {
    if (xTimerStart(xTimers, 0) != pdPASS)
    {
      ESP_LOGI(TAG, "The timer could not be set into the Active state.");
    }
  }
  return ESP_OK;
}

void vTimerCallback(TimerHandle_t pxTimer)
{
  adc_val = adc1_get_raw(ADC_CHANNEL_6);
  int adc_case = adc_val / 512; // val max 512
  switch (adc_case)
  {
  case 0:
    gpio_set_level(ledR, 0);
    gpio_set_level(ledG, 0);
    gpio_set_level(ledB, 0);
    break;
  case 1:
    gpio_set_level(ledR, 1);
    gpio_set_level(ledG, 0);
    gpio_set_level(ledB, 0);
    break;
  case 2:
    gpio_set_level(ledR, 1);
    gpio_set_level(ledG, 1);
    gpio_set_level(ledB, 0);
    break;
  case 3:
    gpio_set_level(ledR, 1);
    gpio_set_level(ledG, 1);
    gpio_set_level(ledB, 1);
    break;
  case 4:
    gpio_set_level(ledR, 0);
    gpio_set_level(ledG, 1);
    gpio_set_level(ledB, 1);
    break;
  default:
    break;
  }
  ESP_LOGI(TAG, "ADC VAL: %.3f", ((float)adc_val) * 0.000805);
  blink_led();
}

esp_err_t set_adc(void)
{
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_11db);
  adc1_config_width(ADC_WIDTH_BIT_12);
  return ESP_OK;
}
