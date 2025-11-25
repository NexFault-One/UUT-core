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
    if (Serial2.available() > 0)
    {
        uint8_t buffer[PROTOBUF_BUFFER_SIZE];
        size_t bytes_read = 0;
        unsigned long last_byte_time = millis();

        // keep reading until no new bytes arrive for 50 ms or buffer full
        while ((millis() - last_byte_time) < 50 && bytes_read < PROTOBUF_BUFFER_SIZE)
        {
            if (Serial2.available())
            {
                buffer[bytes_read++] = Serial2.read();
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
