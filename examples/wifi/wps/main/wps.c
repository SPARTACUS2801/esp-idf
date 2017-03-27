/* WiFi Connection Example using WPS
 * 
 * WPS_TYPE_PBC: Start esp32 and it will inter wps PBC mode. Then push the buttom of wps on router down. The esp32 will connected to the router.
 *
 * WPS_TYPE_PIN: Start esp32, You'll see PIN code which is a eight-digit number showing on COM. Enter the PIN code in router and then the esp32 will connected to router.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_wps.h"
#include "esp_event_loop.h"


/*set wps mode here*/
#define WPS_TEST_MODE WPS_TYPE_PBC
//#define WPS_TEST_MODE WPS_TYPE_PIN
//#define WPS_TEST_MODE WPS_TYPE_MAX 


#ifndef PIN2STR
#define PIN2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7]
#define PINSTR "%c%c%c%c%c%c%c%c"
#endif

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

static const char *TAG = "example_wps";


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
	ESP_LOGI(TAG, "got ip:%s\n",
		ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
	ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
	/*point: the function esp_wifi_wps_start() only get ssid & password
	 * and we suggest you call the function esp_wifi_connect() here
	 * */
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
	ESP_ERROR_CHECK(esp_wifi_wps_disable());
	ESP_ERROR_CHECK(esp_wifi_connect());
	break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
	ESP_ERROR_CHECK(esp_wifi_wps_disable());
	ESP_ERROR_CHECK(esp_wifi_wps_enable(WPS_TEST_MODE));
	ESP_ERROR_CHECK(esp_wifi_wps_start(0));
	break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
	ESP_ERROR_CHECK(esp_wifi_wps_disable());
	ESP_ERROR_CHECK(esp_wifi_wps_enable(WPS_TEST_MODE));
	ESP_ERROR_CHECK(esp_wifi_wps_start(0));
 	break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_PIN");
	/*show the PIN code here*/
	ESP_LOGI(TAG, "WPS_PIN = "PINSTR, PIN2STR(event->event_info.sta_er_pin.pin_code));
	break;
    default:
        break;
    }
    return ESP_OK;
}

/*init wifi as sta and start wps*/
static void start_wps(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "start wps...");
    //ESP_ERROR_CHECK(esp_wifi_wps_enable(WPS_TYPE_PBC));
    ESP_ERROR_CHECK(esp_wifi_wps_enable(WPS_TEST_MODE));
    ESP_ERROR_CHECK(esp_wifi_wps_start(0));
}

void app_main()
{
    start_wps();
}
