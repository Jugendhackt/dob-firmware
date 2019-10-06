#include "send.h"

const char kartoffel[] = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit ";

void send_file(void *pvParameters){
    struct sockaddr_in destaddr = {
        .sin_addr = {
            .s_addr = inet_addr("192.168.4.1")
        },
        .sin_family = AF_INET,
        .sin_port = htons(PORT)
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