/*
 * Christopher Hewitt
 * hewittc@avcomofva.com
 *
 * Drone Delivery Systems AirBox control software
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#include "airbox.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_attr.h"
#include "esp_log.h"

#include "driver/i2c.h"

/**
 * @brief Write to BQ34110 on I2C bus
 */
esp_err_t gauge_write(i2c_port_t port, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    ret = ESP_OK;

    if (len < 1) {
        return ret;
    }

#ifdef DEBUG_I2C
    ESP_LOGI("i2c", "writing %d bytes to %02x", len, reg);
#endif

    for (int i = 0; i < len; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();

        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, GAUGE_ADDR | I2C_MASTER_WRITE, true));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg++, true));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, data[i], true));
        ESP_ERROR_CHECK(i2c_master_stop(cmd));

        ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);

        i2c_cmd_link_delete(cmd);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    return ret;
}

/**
 * @brief Read from BQ34110 on I2C bus
 */
esp_err_t gauge_read(i2c_port_t port, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    ret = ESP_OK;

    if (len < 1) {
        return ret;
    }

#ifdef DEBUG_I2C
    uint8_t nreg;
    nreg = reg;

    ESP_LOGI("i2c", "reading %d bytes from %02x", len, reg);
#endif

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, GAUGE_ADDR | I2C_MASTER_WRITE, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg, true));

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, GAUGE_ADDR | I2C_MASTER_READ, true));

    bool ack;
    ack = false;

    for (int i = 0; i < len; i++) {
        if (i >= len - 1) {
            ack = true;
        }

#ifdef DEBUG_I2C
        ESP_LOGI("i2c", "#: %d, reg: %02x, ack: %d", i, nreg++, ack);
#endif

        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data + i, ack));
    }

    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    return ret;
}

/**
 * @brief Issue a subcommand to BQ34110
 */
esp_err_t gauge_control(i2c_port_t port, uint16_t subcmd, uint8_t *res, uint8_t len) {
    esp_err_t ret;
    ret = ESP_OK;

    uint8_t data[] = {
        (subcmd >> 0) & 0xff,
        (subcmd >> 8) & 0xff
    };

    ret = gauge_write(port, GAUGE_MAC, data, 2);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = gauge_read(port, GAUGE_MAC, data, 2);
    if (ret != ESP_OK) {
        return ret;
    }

    uint16_t req;
    req = (data[1] << 8) | data[0];

    if (subcmd == req) {
        uint8_t buf[len];
        ret = gauge_read(port, GAUGE_MAC_DATA, buf, len);

        for (int i = 0, j = len - 1; i < len; i++) {
            res[i] = buf[j--];
        }
    }

    return ret;
}

/**
 * @brief Perform a full reset on BQ34110
 */
void gauge_reset(i2c_port_t port) {
    esp_err_t ret;
    ret = gauge_control(port, GAUGE_SUB_RESET, NULL, 0);
    if (ret != ESP_OK) {
        ESP_LOGE("battery", "failed to issue fuel gauge reset");
    }
}

/**
 * @brief Query version number from BQ34110
 */
bool gauge_version(i2c_port_t port, uint32_t *version) {
    uint8_t v[4];

    esp_err_t ret;
    ret = gauge_control(port, GAUGE_SUB_FW_VERSION, v, sizeof(v));
    if (ret != ESP_OK) {
        return false;
    }

    *version = (v[3] << 24) | (v[2] << 16) | (v[1] << 8) | v[0];
    return true;
}

/**
 * @brief Query temperature from BQ34110 (degrees Celsius)
 */
bool gauge_temperature(i2c_port_t port, double *temp) {
    uint8_t t[2];

    esp_err_t ret;
    ret = gauge_read(port, GAUGE_TEMP, t, sizeof(t));
    if (ret != ESP_OK) {
        return false;
    }

    *temp = ((double) ((t[1] << 8) | t[0]) / 10.0) - 273.15;
    return true;
}

/**
 * @brief Query voltage from BQ34110 (Volts)
 */
bool gauge_voltage(i2c_port_t port, double *voltage) {
    uint8_t v[2];

    esp_err_t ret;
    ret = gauge_read(port, GAUGE_VOLT, v, sizeof(v));
    if (ret != ESP_OK) {
        return false;
    }

    *voltage = (double) ((v[1] << 8) | v[0]) / 1000.0;
    return true;
}

/**
 * @brief Query relative state of charge from BQ34110 (percent)
 */
bool gauge_rsoc(i2c_port_t port, double *rsoc) {
    uint8_t r[2];

    esp_err_t ret;
    ret = gauge_read(port, GAUGE_RSOC, r, sizeof(r));
    if (ret != ESP_OK) {
        return false;
    }

    *rsoc = (double) ((r[1] << 8) | r[0]);
    return true;
}

// vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4
