#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

void app_main() {
	// the tcpip adapter is a requirement for the wifi scan
	tcpip_adapter_init();
	// the esp event loop must be initialized, but doesn't need to do anything
	esp_event_loop_init(NULL, NULL);
	
	// try to init the nvs flash. it is required for the wifi data. This has been modified from the original code presented in c't to increase compatibility
	esp_err_t ret = nvs_flash_init();
	// if the init fails, try to format the nvs and try another init
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	// ESP_ERROR_CHECK is a macro usable for all esp function calls. it will produce a human readable error description.
	ESP_ERROR_CHECK(ret);
	
	// initialize the wifi config with the default data
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	// initialize the wifi with the config data
	esp_wifi_init(&wifi_init_config);
	// set the mode of the wifi to STA[tion]. this an additional modification from the original article. otherwise the scan will fail sometimes
	esp_wifi_set_mode(WIFI_MODE_STA);
	// start the wifi driver
	esp_wifi_start();

	while (true) {
		// begin the scan with default parameters (NULL) and delay the code execution until the scan is complete (true).
		esp_wifi_scan_start(NULL, true);

		uint16_t ap_num;
		// get the number of found aps 
		esp_wifi_scan_get_ap_num(&ap_num);
		wifi_ap_record_t ap_records[ap_num];
		// get the list of found aps
		esp_wifi_scan_get_ap_records(
				&ap_num, ap_records
				);
		// print the list
		printf("%d Netze gefunden:\n", ap_num); // %d Networks found
		printf(" Kanal | RSSI | SSID \n");	// Channel | RSSI | SSID
		// walk the list of aps and print their data
		for(int i = 0; i < ap_num; i++)
			printf("%6d |%5d | %-27s \n", 
					ap_records[i].primary, 
					ap_records[i].rssi, 
					(char *)ap_records[i].ssid
			      );
		// print a line to increase legibility
		printf("===========================================\n\n");
		// wait for five seconds til the next scan begins. this has also been modified from the original code
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}
