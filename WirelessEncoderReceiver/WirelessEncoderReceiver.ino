/*
 * Wireless encoder receiver
 * 
 * This sketch receives the commands coming from the transmitter
 * and then simulates the rotary encoder and the enter button.
 * 

 NRF24 connections:
 MISO -> 12
 MOSI -> 11
 SCK -> 13
 CE -> 9
 CSN -> 10

 Packet data format:
 0000 0000
 |         Button state
  |        Step direction
   || |||| Number of steps
 */

#include <SPI.h>
#include <RH_NRF24.h>

#define ENC_PIN_A 6
#define ENC_PIN_B 5
#define ENTER_PIN 9

static int encoderStates[4] = {
  0, 1, 3, 2
};

int encoderState;
RH_NRF24 nrf24;

void setup() {
  encoderState  = 0;

  pinMode(ENC_PIN_A, OUTPUT);
  pinMode(ENC_PIN_B, OUTPUT);
  pinMode(ENTER_PIN, OUTPUT);

  digitalWrite(ENC_PIN_A, encoderStates[encoderState] & 2);
  digitalWrite(ENC_PIN_B, encoderStates[encoderState] & 1);
  digitalWrite(ENTER_PIN, HIGH);

  Serial.begin(115200);

  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

  Serial.println("Wireless encoder receiver starting...");
}


void loop() {
  if (nrf24.available()) {
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN] = {0};
    uint8_t len = sizeof(buf);

    if (nrf24.recv(buf, &len) && len > 0) {
      Serial.print("Got packet: ");
      Serial.println(buf[0]);

      //Step the encoder
      if (buf[0] && (uint8_t)0x3F) {
        step(((((buf[0] >> 6) & 1) ? -1 : 1)) * (buf[0] & (uint8_t)0x3F));
      }

      //Press the button
      if (((buf[0] >> 7) & 1)) {
        digitalWrite(ENTER_PIN, LOW);
        delay(100);
        digitalWrite(ENTER_PIN, HIGH);
      }
    } else
    {
      Serial.println("recv failed");
    }
  }
}

void step(int steps) {

  for (int i = abs(steps); i > 0; i--) {
    if (steps < 0) {
      encoderState--;

      if (encoderState < 0) {
        encoderState = 3;
      }
    }
    else {
      encoderState++;

      if (encoderState > 3) {
        encoderState = 0;
      }
    }

    digitalWrite(ENC_PIN_A, encoderStates[encoderState] & 2);
    digitalWrite(ENC_PIN_B, encoderStates[encoderState] & 1);
    delay(5);
  }
}

































