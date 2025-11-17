#include <Arduino.h>
#include "../include/proto_codec/proto_communication.h"
#include "../include/proto_codec/proto_communication.c"
#include "../protobuf_msgs/proto_msgs/uart_data.pb.c"


#define TX_PIN 17
#define RX_PIN 18
#define HOST_BAUD 9600
#define DEVICE_BAUD 115200


void setup() {
  // host connection debug console
  Serial.begin(HOST_BAUD);
  // UART used to talk with DSI
  Serial2.begin(DEVICE_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(500);
  
  Serial.println("UUT: Ready.");
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
  if(Serial2.available())
  {
    uint8_t buffer[PROTOBUF_BUFFER_SIZE];
    size_t bytes_read = 0;

    while(Serial2.available() && bytes_read < PROTOBUF_BUFFER_SIZE)
    {
      buffer[bytes_read++] = Serial2.read();
    }
    Serial.print("RX bytes (");
    Serial.print(bytes_read);
    Serial.println("):");
    for (size_t i = 0; i < bytes_read; ++i) {
      if (buffer[i] < 0x10) Serial.print('0');
      Serial.print(buffer[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
    bool printable = true;
    for (size_t i = 0; i<bytes_read;++i)
    {
      if(buffer[i] < 0x20 || buffer[i] > 0x7E) {printable = false; break;}

    }
    if(printable)
    {
      Serial.print("ASCII message: ");
      for(size_t i = 0; i < bytes_read ; ++i)
      {
        Serial.write(buffer[i]);
      }
      Serial.println();
    }
    else {
      Serial.println("non printable data received...");
    }
  }
  delay(50);
}