#include "common.h"
#include "rec.h"
#include "send.h"

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    initialise_wifi();

    xTaskCreate(rec_file, "rec", 4096, NULL, 5, NULL);
    xTaskCreate(send_file, "send", 4096, NULL, 5, NULL);
}