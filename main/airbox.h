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

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "lwip/api.h"

#include "esp_attr.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"

#include "nvs_flash.h"

#include "driver/mcpwm.h"
#include "driver/i2c.h"

#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#define WIFI_SSID       "AirBoxNetwork"
#define WIFI_PASSPHRASE "AirBoxHome"
#define IP_ADDRESS      "192.168.8.1:5000"
#define HTTP_PORT            5000

#define MAX_URI_LEN            16

#define RSOC_THRESHOLD_SLEEP 10.0 // relative state of charge to sleep
#define RSOC_THRESHOLD_WAKE  30.0 // relative state of charge to wake

#define POWER_6V_EN_PIN        25
#define UNLOCK_DOOR_1_PIN      26 //TO-DO: DETERMINE WHICH PIN TO CONNECT TO

#define SERVO_ONE_PIN          32
#define SERVO_TWO_PIN          33

#define SERVO_PULSE_IDLE_US     0 // idle pulse width (microseconds)
#define SERVO_PULSE_OPEN_US  2000 // open pulse width (microseconds)
#define SERVO_PULSE_CLOSE_US 1000 // close pulse width (microseconds)

#define SERVO_DURATION_US    6000 // duration of servo movement (microseconds)
#define SERVO_DELAY_US       2000 // delay before moving second servo (microseconds)

#define I2C_MASTER      I2C_NUM_0

#define I2C_SDA_IO             21
#define I2C_SCL_IO             22
#define I2C_SCL_FREQ_HZ      1000

#define GAUGE_BAT_LOW_PIN      23

#define GAUGE_ADDR           0xaa

#define GAUGE_CNTL           0x00
#define GAUGE_TEMP           0x06
#define GAUGE_VOLT           0x08
#define GAUGE_BSTAT          0x0a
#define GAUGE_CURRENT        0x0c
#define GAUGE_RC             0x10
#define GAUGE_FCC            0x12
#define GAUGE_AI             0x14
#define GAUGE_TTE            0x16
#define GAUGE_TTF            0x18
#define GAUGE_AP             0x24
#define GAUGE_INTTEMP        0x28
#define GAUGE_CC             0x2a
#define GAUGE_RSOC           0x2c
#define GAUGE_SOH            0x2e
#define GAUGE_CV             0x30
#define GAUGE_CI             0x32
#define GAUGE_DESCAP         0x3c
#define GAUGE_MAC            0x3e
#define GAUGE_MAC_DATA       0x40
#define GAUGE_MAC_DATA_SUM   0x60
#define GAUGE_MAC_DATA_LEN   0x61

#define GAUGE_SUB_CONTROL_STATUS        0x0000
#define GAUGE_SUB_DEVICE_TYPE           0x0001
#define GAUGE_SUB_FW_VERSION            0x0002
#define GAUGE_SUB_HW_VERSION            0x0003
#define GAUGE_SUB_IF_SUM                0x0004
#define GAUGE_SUB_STATIC_DF_SUM         0x0005
#define GAUGE_SUB_PREV_MACWRITE         0x0007
#define GAUGE_SUB_BOARD_OFFSET          0x0009
#define GAUGE_SUB_CC_OFFSET             0x000a
#define GAUGE_SUB_CC_OFFSET_SAVE        0x000b
#define GAUGE_SUB_ALL_DF_SUM            0x0010
#define GAUGE_SUB_SET_SNOOZE            0x0013
#define GAUGE_SUB_CLEAR_SNOOZE          0x0014
#define GAUGE_SUB_ACCUM_DSG_EN          0x001e
#define GAUGE_SUB_ACCUM_CHG_EN          0x001f
#define GAUGE_SUB_IGNORE_SELFDSG_EN     0x0020
#define GAUGE_SUB_EOS_EN                0x0021
#define GAUGE_SUB_PIN_CONTROL_EN        0x0022
#define GAUGE_SUB_LIFETIME_EN           0x0023
#define GAUGE_SUB_WHR_EN                0x0024
#define GAUGE_SUB_CAL_TOGGLE            0x002d
#define GAUGE_SUB_SEAL                  0x0030
#define GAUGE_SUB_SECURITY_KEYS         0x0035
#define GAUGE_SUB_AUTHENTICATION_KEY    0x0037
#define GAUGE_SUB_EOS_START_LEARN       0x0039
#define GAUGE_SUB_EOS_ABORT_LEARN       0x003a
#define GAUGE_SUB_EOS_RCELL_RRATE_LEARN 0x003b
#define GAUGE_SUB_EOS_WARNCLR           0x003c
#define GAUGE_SUB_EOS_INITIAL_RCELL     0x003e
#define GAUGE_SUB_EOS_INITIAL_RRATE     0x003f
#define GAUGE_SUB_RESET                 0x0041
#define GAUGE_SUB_EOS_LOAD_ON           0x0044
#define GAUGE_SUB_EOS_LOAD_OFF          0x0045
#define GAUGE_SUB_DEVICE_NAME           0x004a
#define GAUGE_SUB_ACCUM_RESET           0x004b
#define GAUGE_SUB_EOS_SAFETY_STATUS     0x0051
#define GAUGE_SUB_OPERATION_STATUS      0x0054
#define GAUGE_SUB_GAUGING_STATUS        0x0056
#define GAUGE_SUB_MANUFACTURING_STATUS  0x0057
#define GAUGE_SUB_ALERT1_STATUS         0x005e
#define GAUGE_SUB_ALERT2_STATUS         0x005f
#define GAUGE_SUB_LT_DATA_1             0x0060
#define GAUGE_SUB_PIN_ALERT1_SET        0x0068
#define GAUGE_SUB_PIN_ALERT1_RESET      0x0069
#define GAUGE_SUB_PIN_ALERT2_SET        0x006a
#define GAUGE_SUB_PIN_ALERT2_RESET      0x006b
#define GAUGE_SUB_PIN_VEN_SET           0x006c
#define GAUGE_SUB_PIN_VEN_RESET         0x006d
#define GAUGE_SUB_PIN_LEN_SET           0x006e
#define GAUGE_SUB_PIN_LEN_RESET         0x006f
#define GAUGE_SUB_MANU_DATA             0x0070
#define GAUGE_SUB_EOSSTATUS1            0x007a
#define GAUGE_SUB_EOSSTATUS2            0x007b
#define GAUGE_SUB_ENTER_ROM             0x0f00
#define GAUGE_SUB_STORE_REMCAP_A        0x22c1
#define GAUGE_SUB_STORE_REMCAP_B        0x28a0


/** Copy of rc.local.sh file at filepath /etc/rc.local
  #disable pause frame support
  ethtool -A eth0 autoneg off rx off tx off
  #reset link to promote auto negotiation.
  ethtool -r eth0
  echo "ESP32 - rc.local: Disabled pause frame support and reset eth0" > /dev/kmsg
  echo 160 > /sys/class/gpio/export
  echo 161 > /sys/class/gpio/export
  echo 162 > /sys/class/gpio/export
  echo 163 > /sys/class/gpio/export
  echo 164 > /sys/class/gpio/export
  echo 165 > /sys/class/gpio/export
  echo 166 > /sys/class/gpio/export
  echo 83 > /sys/class/gpio/export
  echo out > /sys/class/gpio/gpio164/direction
  echo out > /sys/class/gpio/gpio165/direction
  echo out > /sys/class/gpio/gpio166/direction
  echo out > /sys/class/gpio/gpio83/direction
  chmod a+rw /sys/class/gpio/gpio160/value
  chmod a+rw /sys/class/gpio/gpio161/value
  chmod a+rw /sys/class/gpio/gpio162/value
  chmod a+rw /sys/class/gpio/gpio163/value
  chmod a+rw /sys/class/gpio/gpio164/value
  chmod a+rw /sys/class/gpio/gpio165/value
  chmod a+rw /sys/class/gpio/gpio166/value
  chmod a+rw /sys/class/gpio/gpio83/value
  chmod a+rw /sys/class/gpio/gpio160/edge
  chmod a+rw /sys/class/gpio/gpio161/edge
  chmod a+rw /sys/class/gpio/gpio162/edge
  chmod a+rw /sys/class/gpio/gpio163/edge
*/

//Pin value constants
#define LOW   0
#define HIGH  1
#define UNDEFINED -1
//TO-DO: #define PWM_TWO_PERCENT to PWM_HUNDRED_PERCENT in two percent steps

//Pin direction constants
#define INPUT_PIN 0
#define OUTPUT_PIN 1

#define CONNECTED_BIT        BIT0
#define DISCONNECTED_BIT     BIT1

extern bool DEBUG_STATEMENTS_ON;  //Toggle error messages on and off

// battery fuel gauge functions in battery.c
esp_err_t gauge_write(i2c_port_t port, uint8_t reg, uint8_t *data, uint8_t len);
esp_err_t gauge_read(i2c_port_t port, uint8_t reg, uint8_t *data, uint8_t len);
esp_err_t gauge_control(i2c_port_t port, uint16_t subcmd, uint8_t *res, uint8_t len);
void gauge_reset(i2c_port_t port);
bool gauge_version(i2c_port_t port, uint32_t *version);
bool gauge_temperature(i2c_port_t port, double *temp);
bool gauge_voltage(i2c_port_t port, double *voltage);
bool gauge_rsoc(i2c_port_t port, double *rsoc);

// vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4

