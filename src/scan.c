#include "scan.h"
#define SCANTAG "scan"

void on_scan_done(){
    ESP_LOGI(SCANTAG,"on scan done");
    uint16_t scanCount;
    esp_wifi_scan_get_ap_num(&scanCount);
    if(scanCount > 8){
        scanCount = 8;
    }
    wifi_ap_record_t scanResult[scanCount];
    if(scanCount) {
        if(esp_wifi_scan_get_ap_records(&scanCount, (wifi_ap_record_t*)scanResult) != ESP_OK) {
            scanCount = 0;
        }
    }
    
    //WiFiGenericClass::setStatusBits(WIFI_SCAN_DONE_BIT);
    //xEventGroupSetBits(wifi_event_group, WIFI_SCAN_DONE_BIT);
    //WiFiGenericClass::clearStatusBits(WIFI_SCANNING_BIT);
    //xEventGroupClearBits(wifi_event_group, WIFI_SCANNING_BIT);
    for (size_t i = 0; i < scanCount; i++){
        wifi_ap_record_t* it = &scanResult[i];
        if(!it) {
            return false;
        }
        ESP_LOGI("SSID", "%s", it->ssid);
        ESP_LOGI("Auth", "%d", it->authmode);
        ESP_LOGI("rssi", "%d", it->rssi);
        ESP_LOGI("bssid", "%x:%x:%x:%x:%x:%x", it->bssid[0], it->bssid[1], it->bssid[2], it->bssid[3], it->bssid[4], it->bssid[5]);
        ESP_LOGI("Prim", "%d", it->primary);
        if(0 == strcmp((char*)it->ssid, SSID)){
            uint8_t my_mac[6];
            esp_wifi_get_mac(ESP_IF_WIFI_STA, my_mac);
            bool same = true;
            for (size_t j = 0; j < 6; j++){
                ESP_LOGI(SCANTAG, "%d %d", my_mac[j], it->bssid[j]);
                if(my_mac[j] != it->bssid[j]){
                    same = false;
                    break;
                }
            }
            if(!same){
                ESP_LOGI(" ", "FOUND!");
                found = true;
                memcpy(sec_mac, it->bssid, 6);
            }
        }
        ESP_LOGI(" ", "\n\n\n\n");
    }
}


bool wait_device(){
    wifi_scan_config_t config = {
        .ssid = 0,
        .bssid = 0,
        .channel = WIFICHAN,
        .show_hidden = 0,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active = {
            .min = 100,
            .max = 250
        }
    };
    while (!found){
        ESP_ERROR_CHECK(esp_wifi_scan_start(&config, false));
        vTaskDelay(2500 / portTICK_PERIOD_MS);
        ESP_LOGI(SCANTAG, "Scan cycle done");
    }
    esp_wifi_scan_stop();
    vTaskDelay(2500 / portTICK_PERIOD_MS);
        
    bool first = false;
    uint8_t my_mac[6];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, my_mac);                
    for (size_t i = 0; i < 6; i++){
        ESP_LOGI(SCANTAG, "%d > %d", my_mac[i], sec_mac[i]);
        if(my_mac[i] != sec_mac[i]){
            first = my_mac[i] > sec_mac[i];
            break;
        }
    }
    ESP_LOGI("first", "%d", first);
    return first;
}