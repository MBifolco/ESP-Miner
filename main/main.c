
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

// #include "protocol_examples_common.h"
#include "main.h"

#include "asic_result_task.h"
#include "asic_task.h"
#include "create_jobs_task.h"
#include "esp_netif.h"
#include "system.h"
#include "nvs_config.h"
#include "serial.h"
#include "stratum_task.h"
#include "i2c_bitaxe.h"
#include "adc.h"
#include "nvs_device.h"
#include "driver/uart.h"
#include "fayksic.h"
#include "stratum_api.h"
#include "mining.h"

//#include "self_test.h"

static GlobalState GLOBAL_STATE = {
    .extranonce_str = NULL, 
    .extranonce_2_len = 0, 
    .abandon_work = 0, 
    .version_mask = 0,
    .ASIC_initalized = false
};

static const char * TAG = "miner";


void app_main(void)
{
    ESP_LOGI(TAG, "Welcome to the bitaxe - hack the planet!");

    // Init I2C
    ESP_ERROR_CHECK(i2c_bitaxe_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    //wait for I2C to init
    vTaskDelay(100 / portTICK_PERIOD_MS);

    //Init ADC
    ADC_init();

    //initialize the ESP32 NVS
    if (NVSDevice_init() != ESP_OK){
        ESP_LOGE(TAG, "Failed to init NVS");
        return;
    }
    
    //parse the NVS config into GLOBAL_STATE
    if (NVSDevice_parse_config(&GLOBAL_STATE) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to parse NVS config");
        return;
    }
     
   
    SERIAL_init();
     
    //should we run the self test?
    //if (should_test(&GLOBAL_STATE)) {
    //    self_test((void *) &GLOBAL_STATE);
    //    vTaskDelay(60 * 60 * 1000 / portTICK_PERIOD_MS);
    //}
   
    SYSTEM_init_system(&GLOBAL_STATE);
    
    // copy the wifi ssid to the global state
    strncpy(GLOBAL_STATE.SYSTEM_MODULE.ssid, WIFI_SSID, sizeof(GLOBAL_STATE.SYSTEM_MODULE.ssid));
    GLOBAL_STATE.SYSTEM_MODULE.ssid[sizeof(GLOBAL_STATE.SYSTEM_MODULE.ssid)-1] = 0;

    // init and connect to wifi
    wifi_init(WIFI_SSID, WIFI_PASS, "espminer");
   
    //SYSTEM_init_peripherals(&GLOBAL_STATE);
    //xTaskCreate(SYSTEM_task, "SYSTEM_task", 4096, (void *) &GLOBAL_STATE, 3, NULL);
    //xTaskCreate(POWER_MANAGEMENT_task, "power mangement", 8192, (void *) &GLOBAL_STATE, 10, NULL);


    // set the startup_done flag
    
    GLOBAL_STATE.SYSTEM_MODULE.startup_done = true;
    GLOBAL_STATE.new_stratum_version_rolling_msg = false;
    
    
    if (GLOBAL_STATE.ASIC_functions.init_fn != NULL) {

        queue_init(&GLOBAL_STATE.stratum_queue);
        ESP_LOGI(TAG, "Stratum queue initialized");
        queue_init(&GLOBAL_STATE.ASIC_jobs_queue);

        
        //(*GLOBAL_STATE.ASIC_functions.init_fn)(GLOBAL_STATE.POWER_MANAGEMENT_MODULE.frequency_value, GLOBAL_STATE.asic_count);
        SERIAL_set_baud((*GLOBAL_STATE.ASIC_functions.set_max_baud_fn)());
        SERIAL_clear_buffer();

        GLOBAL_STATE.ASIC_initalized = true;

        xTaskCreate(stratum_task, "stratum admin", 8192, (void *) &GLOBAL_STATE, 5, NULL);
        xTaskCreate(create_jobs_task, "stratum miner", 8192, (void *) &GLOBAL_STATE, 10, NULL);
        xTaskCreate(ASIC_task, "asic", 8192, (void *) &GLOBAL_STATE, 10, NULL);
        //xTaskCreate(ASIC_result_task, "asic result", 8192, (void *) &GLOBAL_STATE, 15, NULL);
    }
    
}

void MINER_set_wifi_status(wifi_status_t status, uint16_t retry_count)
{
    if (status == WIFI_CONNECTED) {
        snprintf(GLOBAL_STATE.SYSTEM_MODULE.wifi_status, 20, "Connected!");
        return;
    }
    else if (status == WIFI_RETRYING) {
        snprintf(GLOBAL_STATE.SYSTEM_MODULE.wifi_status, 20, "Retrying: %d", retry_count);
        return;
    } else if (status == WIFI_CONNECT_FAILED) {
        snprintf(GLOBAL_STATE.SYSTEM_MODULE.wifi_status, 20, "Connect Failed!");
        return;
    }
}
