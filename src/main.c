#include "common.h"
#include "rec.h"
#include "long-string.h"

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

void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    initialise_wifi();

    xTaskCreate(rec_file, "rec", 4096, NULL, 5, NULL);
    xTaskCreate(task_send_file, "tcp_client", 4096, NULL, 5, NULL);
}