#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>
#include <lwip/sockets.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <long-string.h>
#define TAG_WIFI "Wifi: "


esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

static void task_send_file(void *pvParameters){
    struct sockaddr_in destaddr = {
        .sin_addr = {
            .s_addr = inet_addr("192.168.4.1")
        },
        .sin_family = AF_INET,
        .sin_port = htons(1234)
    };

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    connect(sock, (struct sockaddr *)&destaddr, sizeof(destaddr));
    int imax = sizeof(kartoffel) / 128;
    for(int i = 0; i < imax; i++){
        send(sock, kartoffel + 128 * i, 128, 0);
    }
    send(sock, kartoffel+128*imax, sizeof(kartoffel) % 128, 0);

    shutdown(sock, 0);
    close(sock);

    vTaskDelete(NULL);
}

void app_main(){
    nvs_flash_init();
    tcpip_adapter_init();
    esp_event_loop_init(event_handler, NULL);
    ESP_LOGI(TAG_WIFI, "Init Wifi");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t cfg_sta = {
        .sta = {
            .ssid = "DoB DoB DoB DoB",
            .password = "BoD BoD BoD BoD",
            .scan_method = WIFI_FAST_SCAN,
        }
    };
    esp_wifi_set_config(WIFI_IF_STA, &cfg_sta);
    esp_wifi_start();
    esp_wifi_connect();

    vTaskDelay(4000 / portTICK_PERIOD_MS);

    ESP_LOGI("Sending: ", "Paket from Task");
    xTaskCreate(task_send_file, "tcp_client", 4096, NULL, 5, NULL);
}