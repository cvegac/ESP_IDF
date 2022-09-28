#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/timers.h"
#include "driver/adc.h"

#define led1 2
#define ADC_SAMPLES 50

uint8_t led_level = 0;
static const char *TAG = "MyModule";
TimerHandle_t xTimers;
int interval = 20;
int timerId = 1;

esp_err_t init_led(void);
esp_err_t blink_led(void);
esp_err_t set_timer(void);
esp_err_t set_adc(void);
int get_button(int adc_value);

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
  int accumulator_adc = 0;
  for (size_t i = 0; i < ADC_SAMPLES; i++)
  {
    accumulator_adc += adc1_get_raw(ADC_CHANNEL_7);
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
  int adc_val_7 = accumulator_adc / ADC_SAMPLES;
  int button = get_button(4095 - adc_val_7);
  ESP_LOGI(TAG, "   \tADC7: %d   \tButton: %d   \tV: %.3f", (4095 - adc_val_7), button, ((float)(4095 - adc_val_7) * 0.000805));
  blink_led();
}

esp_err_t set_adc(void)
{
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_11db);
  adc1_config_width(ADC_WIDTH_BIT_12);
  return ESP_OK;
}

int get_button(int adc_value)
{
  if (adc_value > 3890)
  {
    return 0;
  }
  if (adc_value > 3480)
  {
    return 1;
  }
  if (adc_value > 2860)
  {
    return 2;
  }
  if (adc_value > 2455)
  {
    return 3;
  }
  if (adc_value > 2045)
  {
    return 4;
  }
  if (adc_value > 1635)
  {
    return 5;
  }
  if (adc_value > 1225)
  {
    return 6;
  }
  if (adc_value > 815)
  {
    return 7;
  }
  if (adc_value > 405)
  {
    return 8;
  }
  if (adc_value > 100)
  {
    return 9;
  }
  return 10;
}