#include "common.h"
#include "rec.h"
#include "send.h"
#include "scan.h"

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    initialise_ap();
    if(wait_device()){
        initialise_sta();
        vTaskDelay(2500 / portTICK_PERIOD_MS);
        xTaskCreate(send_file, "send", 4096, NULL, 5, NULL);
    }else{
        xTaskCreate(rec_file, "rec", 4096, NULL, 5, NULL);
    }

}