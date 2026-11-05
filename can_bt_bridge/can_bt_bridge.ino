// CAN bus -> Bluetooth bridge for an e-bike: ESP32 + CAN transceiver.
// Rewritten in 2026 for this repo. Not the original sketch.
//
// Every frame is sent to the phone as one line: millis,id,length,data
// (a remote frame has no data, so its line ends in R)

#include "driver/twai.h"
#include "BluetoothSerial.h"

const gpio_num_t CAN_TX = GPIO_NUM_5;  // to the transceiver's TXD
const gpio_num_t CAN_RX = GPIO_NUM_4;  // to the transceiver's RXD

BluetoothSerial bt;

void setup() {
  Serial.begin(115200);
  bt.begin("ebike-can");  // the name the phone pairs with

  // listen only: never transmit, never acknowledge
  twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX, CAN_RX, TWAI_MODE_LISTEN_ONLY);
  g.rx_queue_len = 32;  // the default 5 overflows while a line is printed
  twai_timing_config_t t = TWAI_TIMING_CONFIG_250KBITS();  // must match the bus
  twai_filter_config_t f = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&g, &t, &f) != ESP_OK || twai_start() != ESP_OK) {
    Serial.println("CAN start failed");
    while (true) delay(1000);
  }
  Serial.println("listening");
}

void loop() {
  twai_message_t msg;
  if (twai_receive(&msg, pdMS_TO_TICKS(100)) != ESP_OK) {
    return;  // nothing on the bus for 100 ms
  }

  char line[64];
  int n = snprintf(line, sizeof(line), "%lu,%lX,%d,",
                   millis(), (unsigned long)msg.identifier, msg.data_length_code);
  if (msg.rtr) {
    snprintf(line + n, sizeof(line) - n, "R");  // remote frame: no data bytes
  } else {
    int len = msg.data_length_code > 8 ? 8 : msg.data_length_code;  // a DLC of 9 to 15 still means 8 bytes
    for (int i = 0; i < len; i++) {
      n += snprintf(line + n, sizeof(line) - n, "%02X", msg.data[i]);
    }
  }

  bt.println(line);
  Serial.println(line);
}
