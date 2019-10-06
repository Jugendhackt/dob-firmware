#include "send.h"

#define SENDTAG "send: "

const char kartoffel[] = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit ";

void send_file(void *pvParameters){
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


    struct sockaddr_in destaddr = {
        .sin_addr = {
            .s_addr = inet_addr("192.168.4.1")
        },
        .sin_family = AF_INET,
        .sin_port = htons(1234)
    };

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    connect(sock, (struct sockaddr *)&destaddr, sizeof(destaddr));

    FILE* index = fopen("/sdcard/index.dob", "r");
    int lastChar = 0;
    while(lastChar != EOF){
        ESP_LOGI(SENDTAG, "While run, last char: %d", lastChar);
        char line[BUFFERSIZE];
        int lenght = 0;
        int run = 0;
        while (run == 0)
        {
            int c = fgetc(index);
            lastChar = c;
            switch (c)
            {
            case EOF:
                run = 1;
                break;
            case '\n':
                run = 2;
                break;
            default:
                line[lenght] = (char)0xff&c;
                lenght++;
                break;
            }
        }
        line[lenght] = '\0';
        ESP_LOGI(SENDTAG, "Line: %s", line);
        if (run == 2){
            send(sock, line, sizeof(line), 0);
            uint8_t accepted;
            recv(sock, &accepted, 1, 0);
            if (accepted)
            {
                FILE* f = fopen(line, "rb");
                if(!f){
                    ESP_LOGE(" ", "X_X");
                }
                fseek(f, 0, SEEK_END);
                size_t filesize = ftell(f);
                ESP_LOGI(SENDTAG, "Size: %d", filesize);
                send(sock, &filesize, sizeof(filesize), 0);
                int imax = filesize / BUFFERSIZE;
                fseek(f, 0, SEEK_SET);
                for(int i = 0; i < imax; i++){
                    char read_buffer[BUFFERSIZE];
                    fread(read_buffer, sizeof(read_buffer[0]), BUFFERSIZE, f);
                    send(sock, read_buffer, BUFFERSIZE, 0);
                }
                char read_buffer[filesize % BUFFERSIZE];
                fread(read_buffer, sizeof(read_buffer[0]), filesize % BUFFERSIZE, f);
                ESP_LOGI(SENDTAG, "Buffer: %s", read_buffer);
                send(sock, read_buffer, filesize % BUFFERSIZE, 0);
                fclose(f);
            }
            
        }
    }
    fclose(index);

    shutdown(sock, 0);
    close(sock);
    esp_vfs_fat_sdmmc_unmount();

    vTaskDelete(NULL);
}