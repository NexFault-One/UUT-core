#include <Arduino.h>

// Pins used by the other code: keep Serial2 on same pins
#define RX_PIN 18
#define TX_PIN 17
#define USB_BAUD 9600
#define UART2_BAUD 115200

void setup() {
  // USB debug console
  Serial.begin(USB_BAUD);
  while (!Serial) { ; }
  delay(50);

  // UART used to talk with DSI
  // Serial2: RX=18, TX=17 (same mapping used in your DSI firmware)
  Serial2.begin(UART2_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(50);

  Serial.println("UUT: Silent echo firmware started. Ready.");
}

void loop() {
  // If there's data from the DSI (on Serial2), forward it to the USB console.
  // If nothing's incoming, do nothing (no spam).
  if (Serial2.available()) {
    // Read available bytes into a small buffer and print hex + ASCII
    const size_t BUF_SZ = 128;
    uint8_t buf[BUF_SZ];
    size_t n = 0;
    while (Serial2.available() && n < BUF_SZ) {
      int b = Serial2.read();
      if (b < 0) break;
      buf[n++] = (uint8_t)b;
    }

    // Print a compact hex dump on USB console
    Serial.print("RX(");
    Serial.print(n);
    Serial.print("): ");
    for (size_t i = 0; i < n; ++i) {
      if (buf[i] < 0x10) Serial.print('0');
      Serial.print(buf[i], HEX);
      if (i + 1 < n) Serial.print(' ');
    }
    Serial.println();
  }

  // Small yield to avoid busy-looping
  delay(2);
}