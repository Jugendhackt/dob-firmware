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
    char rx_buffer[BUFFERSIZE];

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = PIN_NUM_MISO;
    slot_config.gpio_mosi = PIN_NUM_MOSI;
    slot_config.gpio_sck  = PIN_NUM_CLK;
    slot_config.gpio_cs   = PIN_NUM_CS;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 4,
        .allocation_unit_size = 0
    };

    sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

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
                FILE* f = fopen(rx_buffer, "r");
                if (f == NULL)
                {
                    f = fopen(rx_buffer, "w");
                    uint8_t y = 1;
                    send(sock, &y, 1, 0);
                    size_t filesize;
                    recv(sock, &filesize, sizeof(filesize), 0);
                    ESP_LOGI(RECTAG, "filesize: %d", filesize);
                    int imax = filesize / BUFFERSIZE;
                    for(int i = 0; i < imax; i++){
                        uint8_t recv_buffer[BUFFERSIZE];
                        recv(sock, recv_buffer, BUFFERSIZE, 0);
                        ESP_LOGI(RECTAG, "%s", recv_buffer);
                        fwrite(recv_buffer, sizeof(recv_buffer[0]), BUFFERSIZE, f);
                    }
                    uint8_t recv_buffer[filesize % BUFFERSIZE];
                    recv(sock, recv_buffer, filesize % BUFFERSIZE, 0);
                    ESP_LOGI(RECTAG, "%s", recv_buffer);
                    fwrite(recv_buffer, sizeof(recv_buffer[0]), filesize % BUFFERSIZE, f);
                    fclose(f);
                }else
                {
                    uint8_t y = 0;
                    send(sock, &y, 1, 0);
                    fclose(f);
                }
                
                
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