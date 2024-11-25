message(STATUS "Compiling SRCS: ${SRCS}")
idf_component_register(
SRCS
    "adc.c"
    "DS4432U.c"
    "EMC2101.c"
    "fonts.c"
    "i2c_bitaxe.c"
    "INA260.c"
    "led_controller.c"
    "main.c"
    "nvs_config.c"
    "system.c"
    "TPS546.c"
    "vcore.c"
    "work_queue.c"
    "nvs_device.c"
    $<$<BOOL:${CONFIGENABLE_SELF_TEST}>:./self_test/self_test.c>
    "./tasks/stratum_task.c"
    "./tasks/create_jobs_task.c"
    "./tasks/asic_task.c"
    "./tasks/asic_result_task.c"
    $<$<BOOL:${CONFIG_ENABLE_POWER_MANAGEMENT}>:./tasks/power_management_task.c>

INCLUDE_DIRS
    "."
    "tasks"
    #"self_test"
    "../components/asic/include"
    "../components/wifi/include"
    "../components/stratum/include"

PRIV_REQUIRES
    "app_update"
    "driver"
    "esp_adc"
    "esp_app_format"
    "esp_event"
    "esp_http_server"
    "esp_netif"
    "esp_timer"
    "esp_wifi"
    "json"
    "nvs_flash"
    "spiffs"
    "vfs"
    "esp_driver_i2c"
)


