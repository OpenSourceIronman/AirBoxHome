# AirBoxHome
ESP32 and Raspberry Pi code to enable human and drone login and control of an AirBox Home
https://airboxtechnologies.com

Author #1: Christopher Hewitt Email: hewittc@avcomofva.com Org: AVCOM of Virginia

Author #2: Blaze Sanders Email: blaze@robobev.com Skype: blaze.sanders Twitter: @BlazeDSanders

Drone Delivery Systems AirBox control software

Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.

---

Built and tested with esp-idf v3.0-rc1: https://github.com/espressif/esp-idf

1. Checkout supported esp-idf tag from Espressif's git respository
2. Follow Espressif's setup guide for your platform to install cross compiler
3. Run the following commands to build source and flash board connected to USB

  $ export IDF_PATH=/path/to/esp-idf
  $ make
  $ make flash
