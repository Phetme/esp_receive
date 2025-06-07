#include "Arduino.h"
#include "driver/gpio.h"
#include "driver/can.h"
#define LED_BUILTIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  can_general_config_t g_config = { // สร้างต้วแปร g_config ใช้กำหนดค่าเกี่ยวกับบัส CAN
    .mode = CAN_MODE_NORMAL,
    .tx_io = GPIO_NUM_27, // กำหนดขา TX ต่อกับ 26
    .rx_io = GPIO_NUM_26, // กำหนดขา TX ต่อกับ 27
    .clkout_io = ((gpio_num_t) - 1),
    .bus_off_io = ((gpio_num_t) - 1),
    .tx_queue_len = 5,
    .rx_queue_len = 5,
    .alerts_enabled = CAN_ALERT_NONE,
    .clkout_divider = 0
  };
  can_timing_config_t t_config = CAN_TIMING_CONFIG_500KBITS();
  can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();

  if (can_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
    Serial.println("Failed to install driver");
    return;
  }

  if (can_start() != ESP_OK) {
    Serial.println("Failed to start driver");
    return;
  }
}

void loop() {
  can_message_t message;

  if (can_receive(&message, pdMS_TO_TICKS(10000)) != ESP_OK) {
    return;
  }

  Serial.printf("Message received ID 0x%02x: %.*s\n", message.identifier, message.data_length_code, message.data);
  /* ---------- พิมพ์เฟรมที่รับ ---------- */
  for (uint8_t i = 0; i < message.data_length_code; i++) {
    Serial.printf(" %02X", message.data[i]);
  }
  Serial.println();

  /* ---------- ตรวจ byte แรก ---------- */
  /* msg.data[] เป็น ‘อาร์เรย์’ 8 ไบต์  
     ต้องระบุ index (msg.data[0]) ไม่สามารถเทียบ array ↔ ค่าเดี่ยวได้ */
  Serial.printf("RX  ID:0x%03X  DLC:%u  Data:", message.identifier, message.data_length_Acode);

  if (message.data_length_code > 0 && message.data[0] == 0x41) {   // 0x41 = 65 = 'A'
    Serial.println(">>> รับคำสั่ง A!");
    digitalWrite(LED_BUILTIN,HIGH);
   }else{

    digitalWrite(LED_BUILTIN,LOW);

  }
}