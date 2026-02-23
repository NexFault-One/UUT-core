#include <Arduino.h>
#include <ModbusRTU.h>
#include "proto_codec/proto_communication.h"
#include "proto_msgs/uart_data.pb.h"

// nizar esp pins
#define TX_PIN_NZ 17
#define RX_PIN_NZ 18

// prototype pins, for real product. TX = 6 and RX = 5
#define TX_PIN 6
#define RX_PIN 5

// modbus pins
#define MODBUS_RX 15
#define MODBUS_TX 7
#define MODBUS_DE 42

// bauds
#define HOST_BAUD 9600
#define MODBUS_BAUD 19200
#define DEVICE_BAUD 115200

// to identify the esp32 for com port
#define DEVICE_ID "UUT"
#define SLAVE_ID 1

// registers for modbus for debug
#define REG_GENERIC 100

ModbusRTU mb;

// writing
uint16_t cbGeneric(TRegister* reg, uint16_t val)
{
  Serial.printf("[UUT] [CLEAN WRITE] Register %d (Generic) updated to: %u\n", REG_GENERIC, val);
  return val;
}

void performHandshake()
{
    String expectedAck = "<ACK:" DEVICE_ID ">";
    
    while(true)
  {
    Serial.printf("<HELLO_UI:%s>\n", DEVICE_ID);
    delay(1000);

    if(Serial.available())
    {
      String msg = Serial.readStringUntil('\n');
      msg.trim();
      if(msg == expectedAck)
      {
        Serial.printf("[HANDSHAKE OK] Device %s recognized by dashboard.\n", DEVICE_ID);
        break;
      }
    }
  }

}

void setup() {
  // host connection debug console
  Serial.begin(HOST_BAUD);
  // MODBUS
  Serial1.begin(MODBUS_BAUD, SERIAL_8N1, MODBUS_RX, MODBUS_TX);
  // UART used to talk with DSI
  Serial2.begin(DEVICE_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(500);

  pinMode(MODBUS_DE, OUTPUT);
  digitalWrite(MODBUS_DE, LOW);

  mb.begin(&Serial1, MODBUS_DE);

  mb.slave(SLAVE_ID);

  mb.addHreg(REG_GENERIC, 0);
  mb.onSetHreg(REG_GENERIC, cbGeneric);

  Serial.printf("[UUT] Listening on Serial1 (Slave ID: %d)...\n", SLAVE_ID);

  //performHandshake();
  
  Serial.println("[UUT] HANDSHAKE COMPLETED. UUT: Ready.");
}


// NIZAR: TODO need to rewrite to make it compatible with the library (especially hex data)
//void loop() {
//  // If there's data from the DSI (on Serial2), forward it to the USB console.
//  // If nothing's incoming, do nothing (no spam).
//  if (Serial2.available()) {
//    char payload_str[PROTOBUF_BUFFER_SIZE];
//
//    errors.payload.arg = payload_str;
//    errors.payload.funcs.decode = &protobuf_decode_string;
//
//    if(protobuf_receive(&Serial2, &errors, test_msgs_Error_Message_fields))
//    {
//      Serial.print("string (corrupted): ");
//      Serial.println(payload_str);
//    } else {
//      Serial.println("decoding failed!");
//    }
//  }
//
//  // Small yield to avoid busy-looping
//  delay(1000);
//}

// use for now -- temporary
void loop()
{
  mb.task();

  yield();
  
  if (Serial1.available() > 0)
  {
      uint8_t buffer[PROTOBUF_BUFFER_SIZE];
      size_t bytes_read = 0;
      unsigned long last_byte_time = millis();
      // keep reading until no new bytes arrive for 50 ms or buffer full
      while ((millis() - last_byte_time) < 50 && bytes_read < PROTOBUF_BUFFER_SIZE)
      {
          if (Serial1.available())
          {
              buffer[bytes_read++] = Serial1.read();
              last_byte_time = millis();  // reset timer after each byte
          }
      }
      if (bytes_read == 0)
          return;
      Serial.printf("RX bytes (%u):\n", bytes_read);
      for (size_t i = 0; i < bytes_read; ++i)
          Serial.printf("%02X ", buffer[i]);
      Serial.println();
      Serial.print("ASCII message: ");
      for (size_t i = 0; i < bytes_read; ++i)
          Serial.write(buffer[i]);
      Serial.println();
  }
  delay(10);
}
