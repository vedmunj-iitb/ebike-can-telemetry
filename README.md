# CAN Bus Telemetry for an Electric Bike

An ESP32 that listened to an e-bike's CAN bus and relayed battery and motor-controller data over
Bluetooth to a phone.

E-bike data acquisition at IIT Bombay, Jul 2026 to Oct 2026.

## What I did

- Wired a CAN transceiver to an ESP32 to listen to the e-bike's CAN bus, which its battery and
  motor controller use.
- Read battery and motor-controller messages off the bus, including the signals for battery
  health and motor speed.
- Relayed the bus data from the ESP32 over Bluetooth to a phone. The phone then uploaded it to a
  remote server.

```
battery + motor controller --CAN--> transceiver --> ESP32 --Bluetooth--> phone --> server
```

## The sketch in this repo (a 2026 rewrite)

`can_bt_bridge/can_bt_bridge.ino` was written in 2026 for this repo. It is not the original
sketch. Everything in this section describes the rewrite. Its pins, bit rate and listen-only
mode are its own settings, not facts about the original setup.

- It uses the ESP32's built-in CAN controller (TWAI) in listen-only mode. In that mode the ESP32
  never puts anything on the bus, not even an acknowledge bit.
- Its bit rate is 250 kbit/s. Change it to match the bus you are reading. The bit rate of
  the bike's bus is not recorded in this repo.
- GPIO 5 goes to the transceiver's TXD pin and GPIO 4 to its RXD pin.
- It accepts every frame and does not decode anything. Each frame goes to the phone as one text
  line, `millis,id,length,data`. For example (made-up values), a frame with ID 0x1A0 and the
  data bytes 01 to 08, received 1532 ms after start-up, is sent as
  `1532,1A0,8,0102030405060708`. A remote frame has no data bytes, so its line ends in `R`
  instead. The same line is also printed on the USB serial port at 115200 baud. That copy is
  the slowest step, so on a very busy bus some frames can be lost.
- The phone pairs with the Bluetooth name `ebike-can`. The sketch uses Classic Bluetooth serial
  (the `BluetoothSerial` library), so it needs a chip with Classic Bluetooth, such as the
  original ESP32. The ESP32-S3 and ESP32-C3 have Bluetooth LE only.

The rewrite has not been compiled or tested on an ESP32 yet.

## Not in this repo

The original sketch, the phone app and the server.
