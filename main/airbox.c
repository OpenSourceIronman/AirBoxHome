/**
 * @file airbox.c
 * @author Christopher Hewitt (AVCOM)
 * @email hewittc@avcomofva.com
 * @created ?
 *
 * @author Blaze Sanders (ROBO BEV)
 * @email blaze@robobev.com
 * @updated 1 APRIL 2018
 * @version 1.1
 *
 * @brief Drone Delivery Systems AirBox control software
 *
 * TO-DO:
 * Three buttons: Open and Close box. Check battery level
 * Any time box is close its also locked
 *
 * @section DESCRIPTION
 */

#include "airbox.h"
//#include "userLogin.h"

// data from fuel gauge
static uint32_t version = 0;
static double temp = 0.0;
static double voltage = 0.0;
static double rsoc = 0.0;

// GPIO pin constants
static uint32_t LOCKED = 0;
static uint32_t UNLOCKED = 1;
static uint32_t OPEN_DOORS = 2;
static uint32_t CLOSE_DOORS = 3;

// test status
static bool test1 = false;
static bool test2 = false;

// wifi enablement status
static bool wifi = false;

// actuator status
static bool busy = false;

// static index page
static const char http_index[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-type: text/html\r\n\r\n"
    "<html>\r\n"
    "<head>\r\n"
    "<title>AirBox Control</title>\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "<center>\r\n"
    "<img src=\"http:\" alt="">\r\n"
    "<img src=\"http:\\"customadesign.info/airboxtechnologies-new/wp-content/uploads/2018/03/logo.png\"  alt="">\r\n"
    "<br>\r\n"
    "<label for=\"name\">Box ID: 420 </label>\r\n"  //TODO ACCESS USER DATABASE USING LOGIN TOKEN, REMOVE 420, AND DISPLAY BOX ID
    "<br><br>\r\n"
    "<button style=\"background-color: #CC322A; color: #FFF; display: inline-bl$  </button>\r\n"
    "<button style=\"background-color: #0011FF; color: #FFF; display: inline-bl$  </button>\r\n"
    "<button style=\"background-color: #343A40; color: #FFF; display: inline-bl$  </button>\r\n"
    "</center>\r\n"

    "<a href=\"/open\">open</a><br>\r\n"
    "<a href=\"/close\">close</a><br><br>\r\n"
    "<a href=\"/battery\">battery</a><br><br>\r\n"
    "<a href=\"/test1\">test to 0%</a><br>\r\n"
    "<a href=\"/test2\">test to 10%</a><br>\r\n"
    "<a href=\"/stop\">stop</a>\r\n"
    "</body>\r\n"
    "</html>\r\n\r\n";

static const char battery_info[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-type: application/json\r\n\r\n"
    "{\"voltage\":\"%.2f\",\"rsoc\":\"%.0f\",\"temp\":\"%.1f\"}\r\n";

/**
 * @brief WiFi event handler
 */
static esp_err_t wifi_event_handler(void *ctx, system_event_t *event) {
    ESP_LOGI("wifi", "received event %d", event->event_id);
    switch (event->event_id) {
        case SYSTEM_EVENT_AP_START:
            ESP_LOGI("wifi", "access point started");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI("wifi", "client connected to access point");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGI("wifi", "client disconnected from access point");
            break;
        default:
            break;
    }

    return ESP_OK;
}

/**
 * @brief Initialize and start WiFi access point.
 */
bool airbox_init_wifi(const char *ssid, const char *pass) {
    esp_err_t ret;

    // enable flash for configuration storage
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ret = nvs_flash_erase();
        ESP_ERROR_CHECK(ret);

        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // initialize tcp/ip stack
    tcpip_adapter_init();
    tcpip_adapter_ip_info_t info = { 0, };

    // initialize event loop
    ret = esp_event_loop_init(wifi_event_handler, NULL);
    ESP_ERROR_CHECK(ret);

    // load host address, gateway, and mask
    IP4_ADDR(&info.ip, 192, 168, 8, 1);
    IP4_ADDR(&info.gw, 192, 168, 8, 1);
    IP4_ADDR(&info.netmask, 255, 255, 255, 0);

    // disable dhcp service
    ret = tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);
    ESP_ERROR_CHECK(ret);

    // configure host address, gateway, and mask
    ret = tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info);
    ESP_ERROR_CHECK(ret);

    // enable dhcp service
    ret = tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);
    ESP_ERROR_CHECK(ret);

    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();

    // allocate and initialize wifi resources to defaults
    ret = esp_wifi_init(&init);
    ESP_ERROR_CHECK(ret);

    // store wifi configuration in memory
    ret = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    ESP_ERROR_CHECK(ret);

    // set null mode before configuration
    ret = esp_wifi_set_mode(WIFI_MODE_NULL);
    ESP_ERROR_CHECK(ret);

    // create authentication parameters
    wifi_config_t config = {
        .ap = {
            .ssid = "",
            .ssid_len = 0,
            .max_connection = 4,
            .password = "",
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };

    // copy authentication parameters into access point configuration
    strncpy((char *) config.ap.ssid, ssid, sizeof(config.ap.ssid));
    if (pass) {
        if (strnlen(pass, 64) != 0 && strnlen(pass, 64) < 8) {
            ESP_LOGE("wifi", "access point passphrase less than eight characters");

            return false;
        }
        strncpy((char *) config.ap.password, pass, sizeof(config.ap.password));
    }

    // use open mode if no passphrase specified
    if (strnlen(pass, 64) == 0) {
        ESP_LOGW("wifi", "access point is not using a passphrase");
        config.ap.authmode = WIFI_AUTH_OPEN;
    }

    // set soft access point mode
    ret = esp_wifi_set_mode(WIFI_MODE_AP);
    ESP_ERROR_CHECK(ret);

    // enable wifi configuration
    ret = esp_wifi_set_config(ESP_IF_WIFI_AP, &config);
    ESP_ERROR_CHECK(ret);

    return true;
};

/**
 * @brief Initialize general purpose I/O.
 */
void airbox_init_gpio() {
    gpio_config_t config;

    // configure enable line for 6 volt regulator
    config.intr_type = GPIO_PIN_INTR_DISABLE;
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = ((1 << POWER_6V_EN_PIN)); //TODO Understand what 1 means
    config.pull_down_en = true;
    config.pull_up_en = false;

    gpio_config(&config);

    // disable 6 volt regulator
    gpio_set_level(POWER_6V_EN_PIN, 0);


    //Configure unlock pins for mini push-pull solenoids
    config.intr_type = GPIO_PIN_INTR_DISABLE;
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = ((1 << UNLOCK_DOOR_1_PIN)); //TODO Understand what 1 means
    config.pin_bit_mask = ((1 << UNLOCK_DOOR_2_PIN)); //TODO Understand what 1 means
    config.pull_down_en = true;
    config.pull_up_en = false;

    gpio_config(&config);

    //Lock both doors
    gpio_set_level(UNLOCK_DOOR_1_PIN, LOCKED);
    gpio_set_level(UNLOCK_DOOR_2_PIN, LOCKED);

}

/**
 * @brief Initialize I2C master.
 */
void airbox_init_i2c_master() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_io_num = I2C_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_SCL_FREQ_HZ;

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER, conf.mode, 0, 0, 0));
}

/**
 * @brief Initialize PWM timers for servo control.
 */
void airbox_init_mcpwm() {
    // first servo on pwm0a
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_ONE_PIN);

    // second servo on pwm0b
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, SERVO_TWO_PIN);

    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50; // frequency = 50 Hz
    pwm_config.cmpr_a = 0;     // duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;     // duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    // configure pwm0a and pwm0b
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
}

/**
 * @brief Task for opening the box.
 */
void task_airbox_open(void *pvParameters) {
    busy = true;

    ESP_LOGI("open", "opening the box");

    // enable 6 volt supply
    gpio_set_level(POWER_6V_EN_PIN, 1);

    // wait for steady state
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // actuate servo 1
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, SERVO_PULSE_OPEN_US);

    // stagger servo 2
    vTaskDelay(SERVO_DELAY_US / portTICK_PERIOD_MS);

    // actuate servo 2
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, SERVO_PULSE_OPEN_US);

    // continue opening
    vTaskDelay(SERVO_DURATION_US / portTICK_PERIOD_MS);

    // disable 6 volt supply
    gpio_set_level(POWER_6V_EN_PIN, 0);

    // idle servo control lines
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, SERVO_PULSE_IDLE_US);
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, SERVO_PULSE_IDLE_US);

    ESP_LOGI("open", "opening the box: done");

    busy = false;

    vTaskDelete(NULL);
}

/**
 * @brief Task for closing the box.
 */
void task_airbox_close(void *pvParameters) {
    busy = true;

    ESP_LOGI("close", "closing the box");

    // enable 6 volt supply
    gpio_set_level(POWER_6V_EN_PIN, 1);

    // wait for steady state
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // actuate servo 2
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, SERVO_PULSE_CLOSE_US);

    // stagger servo 1
    vTaskDelay(SERVO_DELAY_US / portTICK_PERIOD_MS);

    // actuate servo 1
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, SERVO_PULSE_CLOSE_US);

    // continue closing
    vTaskDelay(SERVO_DURATION_US / portTICK_PERIOD_MS);

    // disable 6 volt supply
    gpio_set_level(POWER_6V_EN_PIN, 0);

    // idle servo control lines
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, SERVO_PULSE_IDLE_US);
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, SERVO_PULSE_IDLE_US);

    ESP_LOGI("close", "closing the box: done");

    busy = false;

    vTaskDelete(NULL);
}

/**
 * @brief Periodically query battery fuel gauge to drive WiFi state
 *
 * - Enable WiFi when RSoC > RSOC_THRESHOLD_WAKE
 * - Disable WiFi and close box when RSoC <= RSOC_THRESHOLD_SLEEP
 *
 * Note: WiFi must be disabled until sufficient RSoC otherwise current draw
 *       is too high for battery trickle charge state. Do not modify this
 *       task unless complete discharge & charge cycles verified by testing!
 */
void task_airbox_battery(void *pvParameters) {
    if (gauge_version(I2C_MASTER, &version)) {
        ESP_LOGI("battery", "fw version: %08x", version);
    } else {
        ESP_LOGE("battery", "failed to acquire firmware version");
    }

    while (1) {
        if (gauge_voltage(I2C_MASTER, &voltage) && gauge_rsoc(I2C_MASTER, &rsoc) && gauge_temperature(I2C_MASTER, &temp)) {
            ESP_LOGI("battery", "voltage: %.2fV, rsoc: %.0f%%, temp: %.1f'C", voltage, rsoc, temp);

            if (rsoc >= RSOC_THRESHOLD_WAKE && !wifi) {
                ESP_LOGI("battery", "battery state (%.0f%%) greater than threshold (%.0f%%), enabling wifi", rsoc, RSOC_THRESHOLD_WAKE);
                ESP_ERROR_CHECK(esp_wifi_start());
                wifi = true;
            } else if (rsoc <= RSOC_THRESHOLD_SLEEP && wifi) {
                ESP_LOGI("battery", "battery state (%.0f%%) less than threshold (%.0f%%), resetting fuel gauge to verify", rsoc, RSOC_THRESHOLD_SLEEP);

                gauge_reset(I2C_MASTER);

                vTaskDelay(1000 / portTICK_PERIOD_MS);

                if (rsoc <= RSOC_THRESHOLD_SLEEP && wifi) {
                    ESP_LOGI("battery", "battery state (%.0f%%) less than threshold (%.0f%%), closing and disabling wifi", rsoc, RSOC_THRESHOLD_SLEEP);

                    vTaskDelay(SERVO_DURATION_US / portTICK_PERIOD_MS);
                    xTaskCreate(task_airbox_close, "airbox_close", 4096, NULL, 20, NULL);
                    vTaskDelay(SERVO_DURATION_US / portTICK_PERIOD_MS);

                    ESP_ERROR_CHECK(esp_wifi_stop());
                    wifi = false;
                }
            }
        } else {
            ESP_LOGE("battery", "failed to acquire battery status");
        }

        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Main control loop.
 */
void task_airbox_control(void *pvParameters) {

    esp_err_t err;

    struct netconn *server;
    struct netconn *client;

    struct netbuf *nb;

    void *data;
    uint16_t datalen;

    char uri[MAX_URI_LEN];

    char *split1;
    char *split2;
    int len;

    char battery[512];

    while (1) {
        // create socket for network connections
        server = netconn_new(NETCONN_TCP);
        if (!server) {
            ESP_LOGI("http", "failed to allocate socket");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        // add socket timeout to prevent lockups
        netconn_set_recvtimeout(server, 10000);

        // listen server for http requests
        netconn_bind(server, IP_ADDR_ANY, HTTP_PORT);
        netconn_listen(server);

        // http request loop
        while (1) {
            // accept new connection
            if ((err = netconn_accept(server, &client)) == ERR_OK) {
                // receive data 
                if ((err = netconn_recv(client, &nb)) == ERR_OK) {
                    // copy data from buffer
                    netbuf_data(nb, &data, &datalen);

                    // check request
                    if (!strncmp(data, "GET ", 4)) {
                        split1 = data + 4;
                        split2 = memchr(split1, ' ', MAX_URI_LEN);
                        len = split2 - split1;

                        memcpy(uri, split1, len);

                        uri[len] = '\0';

                        ESP_LOGI("http", "client requested: %.*s", len, uri);

                        // spawn open/close tasks or show battery info based on request
                        if (!strncmp(uri, "/open", MAX_URI_LEN)) {
                            if (!busy && !test1 && !test2) {
                                xTaskCreate(task_airbox_open, "airbox_open", 4096, NULL, 5, NULL);
                            }
                            netconn_write(client, http_index, strnlen(http_index, 512), NETCONN_COPY);
                        } else if (!strncmp(uri, "/close", MAX_URI_LEN)) {
                            if (!busy && !test1 && !test2) {
                                xTaskCreate(task_airbox_close, "airbox_close", 4096, NULL, 5, NULL);
                            }
                            netconn_write(client, http_index, strnlen(http_index, 512), NETCONN_COPY);
                        } else if (!strncmp(uri, "/battery", MAX_URI_LEN)) {
                            snprintf(battery, sizeof(battery), battery_info, voltage, rsoc, temp);
                            netconn_write(client, battery, strnlen(battery, 512), NETCONN_COPY);
                        } else if (!strncmp(uri, "/test1", MAX_URI_LEN)) {
                            ESP_LOGI("test", "requested test, repeatedly opening and closing until depleted");
                            test1 = true;
                            test2 = false;
                            netconn_write(client, http_index, strnlen(http_index, 512), NETCONN_COPY);
                        } else if (!strncmp(uri, "/test2", MAX_URI_LEN)) {
                            ESP_LOGI("test", "requested test, repeatedly opening and closing until 10%%");
                            test1 = false;
                            test2 = true;
                            netconn_write(client, http_index, strnlen(http_index, 512), NETCONN_COPY);
                        } else if (!strncmp(uri, "/stop", MAX_URI_LEN)) {
                            ESP_LOGI("test", "requested test stop");
                            test1 = false;
                            test2 = false;
                            netconn_write(client, http_index, strnlen(http_index, 512), NETCONN_COPY);
                        } else {
                            netconn_write(client, http_index, strnlen(http_index, 512), NETCONN_COPY);
                        }
                    }

                    netconn_close(client);
                    netconn_delete(client);

                    netbuf_delete(nb);
                } else if (err != ERR_TIMEOUT) {
                    ESP_LOGE("http", "recv failed with error %d", err);

                }
            } else if (err != ERR_TIMEOUT) {
                ESP_LOGE("http", "accept failed with error %d", err);
            }
        }

        netconn_close(server);
        netconn_delete(server);
    }
}

void task_airbox_test(void *pvParameters) {
    while (1) {
        if (test1 || (test2 && wifi)) {
            xTaskCreate(task_airbox_open, "airbox_open", 4096, NULL, 5, NULL);
            vTaskDelay(20000 / portTICK_PERIOD_MS);

            xTaskCreate(task_airbox_close, "airbox_close", 4096, NULL, 5, NULL);
            vTaskDelay(20000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    // initialize peripherals
    airbox_init_gpio();
    airbox_init_i2c_master();
    airbox_init_mcpwm();
    airbox_init_wifi(WIFI_SSID, WIFI_PASSPHRASE);

    // reset fuel gauge
    gauge_reset(I2C_MASTER);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // create tasks
    xTaskCreate(task_airbox_battery, "airbox_battery", 4096, NULL, 15, NULL);
    xTaskCreate(task_airbox_control, "airbox_control", 4096, NULL, 10, NULL);
    xTaskCreate(task_airbox_test, "airbox_test", 4096, NULL, 5, NULL);
}

// vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4
