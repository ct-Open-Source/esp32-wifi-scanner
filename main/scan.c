#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

void app_main() {
  tcpip_adapter_init();
  esp_event_loop_init(NULL, NULL);
  nvs_flash_init();

  wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_config);
  esp_wifi_start();

  while (true) {
    esp_wifi_scan_start(NULL, true);
    
    uint16_t ap_num;
    esp_wifi_scan_get_ap_num(&ap_num);
    wifi_ap_record_t ap_records[ap_num];
    esp_wifi_scan_get_ap_records(
        &ap_num, ap_records
        );
    
    printf("%d Netze gefunden:\n", ap_num);
    printf(" Kanal | RSSI | SSID \n");
    
    for(int i = 0; i < ap_num; i++)
      printf("%6d |%5d | %-27s \n", 
          ap_records[i].primary, 
          ap_records[i].rssi, 
          (char *)ap_records[i].ssid
            );
    
    printf("===========================================\n\n");
    
    vTaskDelay(1000);
  }
}
