#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define ledR 33
#define ledG 25
#define ledB 26
#define STACK_SIZE 1024 * 2
#define R_delay 400
#define G_delay 2000

QueueHandle_t GlobalQueue = 0;

const char *tag = "Main";

esp_err_t init_led(void);
esp_err_t create_tasks(void);

void vTaskR(void *pvParameters);
void vTaskG(void *pvParameters);

void app_main(void)
{
  GlobalQueue = xQueueCreate(20, sizeof(uint32_t));
  init_led();
  create_tasks();
  while (1)
  {
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

esp_err_t init_led()
{
  gpio_reset_pin(ledR);
  gpio_set_direction(ledR, GPIO_MODE_OUTPUT);
  gpio_reset_pin(ledG);
  gpio_set_direction(ledR, GPIO_MODE_OUTPUT);
  gpio_reset_pin(ledB);
  gpio_set_direction(ledR, GPIO_MODE_OUTPUT);
  return ESP_OK;
}

esp_err_t create_tasks()
{
  static uint8_t ucParameterToPass;
  TaskHandle_t xHandle = NULL;

  xTaskCreatePinnedToCore(vTaskR,
                          "vTaskR",
                          STACK_SIZE,
                          &ucParameterToPass,
                          1,
                          &xHandle,
                          0);

  xTaskCreatePinnedToCore(vTaskG,
                          "vTaskG",
                          STACK_SIZE,
                          &ucParameterToPass,
                          1,
                          &xHandle,
                          1);

  /*xTaskCreatePinnedToCore(vTaskB,
                          "vTaskB",
                          STACK_SIZE,
                          &ucParameterToPass,
                          1,
                          &xHandle,
                          tskNO_AFFINITY);*/

  return ESP_OK;
}

void vTaskR(void *pvParameters)
{
  while (1)
  {
    for (size_t i = 0; i < 8; i++)
    {
      gpio_set_level(ledR, 1);
      vTaskDelay(pdMS_TO_TICKS(R_delay / 2));

      ESP_LOGW(tag, "sending %i to queue", i);
      if (!xQueueSend(GlobalQueue, &i, pdMS_TO_TICKS(100)))
      {
        ESP_LOGE(tag, "Error sending %i to queue", i);
      }

      gpio_set_level(ledR, 0);
      vTaskDelay(pdMS_TO_TICKS(R_delay / 2));
    }
    vTaskDelay(pdMS_TO_TICKS(7000));
  }
}

void vTaskG(void *pvParameters)
{
  int receivedValue = 0;

  while (1)
  {
    if (!xQueueReceive(GlobalQueue, &receivedValue, pdMS_TO_TICKS(100)))
    {
      ESP_LOGE(tag, "Error receiving value from queue");
    }
    else
    {
      gpio_set_level(ledG, 1);
      vTaskDelay(pdMS_TO_TICKS(G_delay / 2));

      ESP_LOGI(tag, "Value received %i from queue", receivedValue);

      gpio_set_level(ledG, 0);
      vTaskDelay(pdMS_TO_TICKS(G_delay / 2));
    }
  }
}