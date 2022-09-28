/* Non-Volatile Storage (NVS) Read and Write a Value - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>

void app_main()
{
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  // Open
  printf("\n");
  printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle my_handle;
  err = nvs_open("storage", NVS_READWRITE, &amp; my_handle);
  if (err != ESP_OK)
  {
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  }
  else
  {
    printf("Done\n");

    // Read
    printf("Reading string from NVS ... ");
    char buffer[1000];
    int textsize = 0;
    char test_string[] = "Here is a load of text to test the NVS string storage. ";
    size_t required_size;
    err = nvs_get_str(my_handle, "buffer", NULL, &amp; required_size);
    err = nvs_get_str(my_handle, "buffer", (char *)&amp; buffer, &amp; required_size);
    switch (err)
    {
    case ESP_OK:
      printf("Done\n\n");
      printf("Buffer = %s\n\n", buffer);
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      printf("The value is not initialized yet!\n");
      memset(buffer, 0, sizeof(buffer));
      break;
    default:
      printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
    textsize = strlen(buffer);
    printf("Buffer is %d characters in size\n", textsize);
    if (textsize & gt; 940)
    {
      printf("Text is getting too large, so restarting the ESP32!\n\n");
      for (int i = 6; i &gt; = 0; i--)
      {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
      }
      esp_restart();
    }

    // Write
    printf("Adding text to NVS ... ");
    strncat(buffer, (const char *)test_string, sizeof(test_string));
    err = nvs_set_str(my_handle, "buffer", (const char *)buffer);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    printf("Committing updates in NVS ... ");
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    // Close
    nvs_close(my_handle);
  }

  printf("\n");

  // Restart module
  for (int i = 4; i &gt; = 0; i--)
  {
    printf("Restarting in %d seconds...\n", i);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  printf("Restarting now.\n");
  fflush(stdout);
  esp_restart();
}