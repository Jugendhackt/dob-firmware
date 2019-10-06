#include "common.h"
#define COMMTAG "common"

const int IPV4_GOTIP_BIT = BIT0;
const int IPV6_GOTIP_BIT = BIT1;
bool found = false;
uint8_t sec_mac[6];

esp_err_t event_handler(void *ctx, system_event_t *event){
    switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(COMMTAG, "SYSTEM_EVENT_STA_START");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        /* enable ipv6 */
        tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_STA);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(COMMTAG, "SYSTEM_EVENT_STA_GOT_IP");
        //xEventGroupSetBits(wifi_event_group, 1);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently auto-reassociate. */
        esp_wifi_connect();
        //xEventGroupClearBits(wifi_event_group, IPV4_GOTIP_BIT);
        //xEventGroupClearBits(wifi_event_group, IPV6_GOTIP_BIT);
        break;
    case SYSTEM_EVENT_AP_STA_GOT_IP6:
        //xEventGroupSetBits(wifi_event_group, IPV6_GOTIP_BIT);
        ESP_LOGI(COMMTAG, "SYSTEM_EVENT_STA_GOT_IP6");

        char *ip6 = ip6addr_ntoa(&event->event_info.got_ip6.ip6_info.ip);
        ESP_LOGI(COMMTAG, "IPv6: %s", ip6);
    case SYSTEM_EVENT_SCAN_DONE:
        on_scan_done();
    default:
        break;
    }
    return ESP_OK;
}


void initialise_ap(){
	tcpip_adapter_init();
	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP|WIFI_MODE_STA) );
	wifi_config_t config = {
	   .ap = {
	      .ssid=SSID,
	      .ssid_len=0,
	      .password=PASSWD,
	      .channel=WIFICHAN,
	      .authmode=WIFI_AUTH_WPA2_PSK,
	      .ssid_hidden=0,
	      .max_connection=4,
	      .beacon_interval=100
	   }
	};
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void initialise_sta(){
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t config = {
       .sta = {
            .ssid = "DoB DoB DoB DoB",
            .password = "BoD BoD BoD BoD",
            .channel = WIFICHAN,
            .scan_method = WIFI_FAST_SCAN
        }
	};
    memcpy(config.sta.bssid, sec_mac, 6);
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}
