#include <SPI.h>

#undef PIN_SPI_MISO
#undef PIN_SPI_MOSI
#undef PIN_SPI_SCK
#undef PIN_SPI_SS


// Define SPI pins for NodeMCU (ESP8266) in Slave Mode
#define PIN_SPI_MISO  12  // Master In Slave Out (D6)
#define PIN_SPI_MOSI  13  // Master Out Slave In (D7)
#define PIN_SPI_SCK   14  // Clock (D5)
#define PIN_SPI_SS    4   // Chip Select (D2)

volatile byte receivedData = 0;
volatile bool dataReceived = false;

void ICACHE_RAM_ATTR spiInterrupt() {
  static byte dataBuffer = 0;
  static int bitCount = 0;

  if (digitalRead(PIN_SPI_SCK) == HIGH) {
    dataBuffer <<= 1;
    if (digitalRead(PIN_SPI_MOSI)) {
      dataBuffer |= 1;
    }
    bitCount++;
  }
  
  if (bitCount == 8) {
    receivedData = dataBuffer;
    dataReceived = true;
    bitCount = 0;
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_SPI_MISO, OUTPUT);
  pinMode(PIN_SPI_MOSI, INPUT);
  pinMode(PIN_SPI_SCK, INPUT);
  pinMode(PIN_SPI_SS, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(PIN_SPI_SCK), spiInterrupt, CHANGE);
  Serial.println("ESP8266 SPI Slave Ready");
}

void loop() {
  if (dataReceived) {
    Serial.print("Received Data: 0x");
    Serial.println(receivedData, HEX);
    
    byte responseData = receivedData ^ 0xFF; // Example response
    for (int i = 7; i >= 0; i--) {
      digitalWrite(PIN_SPI_MISO, (responseData >> i) & 0x01);
      delayMicroseconds(10);
    }
    
    dataReceived = false;
  }
}
