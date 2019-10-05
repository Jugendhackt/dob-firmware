#include "rec.h"

#define RECTAG "recv"

int await_connection(){
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;
    
    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(RECTAG, "Unable to create socket: errno %d", errno);
        return -1;
    }
    ESP_LOGI(RECTAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(RECTAG, "Socket unable to bind: errno %d", errno);
        return -1;
    }
    ESP_LOGI(RECTAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(RECTAG, "Error occurred during listen: errno %d", errno);
        return -1;
    }
    ESP_LOGI(RECTAG, "Socket listening");

    struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
    uint addr_len = sizeof(source_addr);
    int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
    if (sock < 0) {
        ESP_LOGE(RECTAG, "Unable to accept connection: errno %d", errno);
        return -1;
    }
    ESP_LOGI(RECTAG, "Socket accepted");
    close(listen_sock);
    return sock;
}

void rec_file(void *pvParameters){
    char rx_buffer[128];
    char addr_str[128];

    while (1) {
        int sock = await_connection();

        while (1) {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer), 0);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(RECTAG, "recv failed: errno %d", errno);
                break;
            }
            // Connection closed
            else if (len == 0) {
                ESP_LOGI(RECTAG, "Connection closed");
                break;
            }
            // Data received
            else {
                ESP_LOGI(RECTAG, "Received %d bytes:", len);
                ESP_LOGI(RECTAG, "%s", rx_buffer);
            }
        }

        if (sock != -1) {
            ESP_LOGE(RECTAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
            vTaskDelay(100);
        }
    }
    vTaskDelete(NULL);
}