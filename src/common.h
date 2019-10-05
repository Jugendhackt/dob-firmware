#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#define SSID "DoB DoB DoB DoB"
#define PASSWD "BoD BoD BoD BoD"
#define PORT 1234
#define BUFFERSIZE 128


#ifndef _COMMON_H_
#define _COMMON_H_

const int IPV4_GOTIP_BIT;
const int IPV6_GOTIP_BIT;
EventGroupHandle_t wifi_event_group;

esp_err_t event_handler(void *ctx, system_event_t *event);
void initialise_wifi();

#endif