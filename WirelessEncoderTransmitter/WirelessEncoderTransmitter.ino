/* 
 * Wireless encoder transmitter
 *  
 * This sketch reads the rotary encoder and the capacitive button  
 * and then sends the the state information over to the receiving
 * end to be simulated.

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

#include <Encoder.h>
#include <CapacitiveSensor.h>
#include <SPI.h>
#include <RH_NRF24.h>

#define ENC_PIN_A 8
#define ENC_PIN_B 7

#define CAPSENSE_PIN_SEND 4
#define CAPSENSE_PIN_RECEIVE 2

#define TOUCH_LIMIT 1000
#define TOUCH_TIME_LIMIT 80
#define TOUCH_TIME_DEBOUNCE 40

Encoder enc(ENC_PIN_A, ENC_PIN_B);

// 1M and 1K resistor between pins 4 & 2, pin 2 is sensor pin.
CapacitiveSensor capacitiveBtn = CapacitiveSensor(CAPSENSE_PIN_SEND, CAPSENSE_PIN_RECEIVE);

// Singleton instance of the radio driver
RH_NRF24 nrf24(9, 10);

long oldPosition;
boolean oldButtonPressed;

void setup() {
  Serial.begin(115200);

  oldPosition  = 0;
  oldButtonPressed = false;

  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

  Serial.println("Wireless encoder transmitter starting...");
}

void loop() {
  boolean touchStarted = false, buttonPressed = false;
  long touchStart, touchEnd, touchDif, mes, newPosition;
  uint8_t packet[1] = {0};
  int8_t stepDiff = 0;

  while (1) {

    // Handle the capacitibe select button
    mes =  capacitiveBtn.capacitiveSensor(20);

    // Start timer for touch length
    if (mes > TOUCH_LIMIT && (millis() - touchEnd) > TOUCH_TIME_DEBOUNCE) {
      if (!touchStarted) {
        touchStarted = true;
        touchStart = millis();
      }
    }
    else {
      // Stop timer if capacitive treshold drop below the limit
      if (touchStarted) {
        touchStarted = false;
        touchDif = millis() - touchStart;

        // The touch is interpeted as a tap if the touch time is below the touch time limit
        if (touchDif < TOUCH_TIME_LIMIT) {
          buttonPressed = true;
        }

        touchEnd = millis();
      }
    }

    // Handle the rotary encoder
    newPosition  = enc.read();
    stepDiff = (oldPosition - newPosition) / 4;

    // If the state of the encoder or the button has changed send the information
    if (abs(stepDiff) | buttonPressed) {
      oldPosition = newPosition;

      // Set num of steps
      packet[0] = (uint8_t)(0x3F & (abs(stepDiff)));

      // Set direction
      if (stepDiff < 0) {
        packet[0] |= (uint8_t)1 << (uint8_t)6;
      }

      // Set button status
      if (buttonPressed) {
        packet[0] |= (uint8_t)1 << (uint8_t)7;
      }

      //nrf24.waitPacketSent();
      nrf24.send(packet, sizeof(packet));

      Serial.print("Packet sent steps: ");
      Serial.print(stepDiff);
      Serial.print(" button: ");
      Serial.print(buttonPressed);
      Serial.print(" packet: ");
      Serial.println((int)packet[0]);

      buttonPressed = false;
      packet[0] = 0;
    }
  }
}
















