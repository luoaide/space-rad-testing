//SetupControllino MAXI as a network server: https://forum.controllino.biz/viewtopic.php?t=2131
#include <SPI.h>
#include <Ethernet.h>

#define NUM_DEV 8

// Constants
const int SENSOR_PINS = {A0, A0, A0, A0, A0, A0, A0, A0}; // Input pin for measuring Vout

const int RS = 10; // Shunt resistor value (in ohms)
const int VOLTAGE_REF = 5; // Reference voltage for analog read

// Global Variables
float sensorValue; // Variable to store value from analog read
float current; // Calculated current value

void setup() {
  // Initialize serial monitor
  Serial.begin(9600);
}

void loop() {
  // Read a value from the INA169 board
  int sensorValues[8];

  for(int i = 0; i<NUM_DEV; i++){
    sensorValues[i] = analogRead(SENSOR_PINS[i]);
  }

  // Remap the ADC value into a voltage number (5V reference)
  for(int i = 0; i<NUM_DEV; i++){
    sensorValues[i] = (sensorValues[i] * VOLTAGE_REF) / 1023;
  }

  // Follow the equation given by the INA169 datasheet to
  // determine the current flowing through RS. Assume RL = 10k
  // Is = (Vout x 1k) / (RS x RL)
  int currents[NUM_DEV]];
  for(int i = 0; i<NUM_DEV; i++){
    currents[i] = sensorValues[i] / (10 * RS);
  }

  // Output value (in amps) to the serial monitor to 3 decimal
  // places
  Serial.print("Start of Loop Output");
  for(int i = 0; i<NUM_DEV; i){
    Serial.printf("Current on Board %d:", i+1);
    Serial.print(currents[i], 3);
    Serial.println(" A");
  }

  //update the webserver
  pushToServer(int* currents, NUM_DEV);

  // Delay program for a few milliseconds
  delay(500);
}
//The Arduino board communicates with the shield using the SPI bus. This is on digital pins 11, 12, and 13 on the Uno... https://www.arduino.cc/en/Reference/Ethernet

//code adapted from: https://www.arduino.cc/en/Tutorial/LibraryExamples/BarometricPressureWebServer#code
//ALSO: https://www.arduino.cc/en/Reference/Ethernet

void pushToServer(int* currents, int len) {
  //mac address: fill with the proper address
  byte mac_addr[] = {0xE8, 0x2A, 0xEA, 0x4B, 0x1F, 0xC3};

  //the ip address of the Controllino or each Arduino
  IPaddress my_ip(192, 168, 1, 254);
  IPaddress server_ip(192, 168, 1, 1);

  EthernetServer client;

  Ethernet.begin(mac_addr, my_ip);

  client.connect(server_ip, 65432);

  delay(10);


  for(int i = 0; i<NUM_DEV; i){
    //FORMAT:  DEVICE_ID TIME_STAMP CURRENT READING
    int deviceID = "deviceNUM" + str(i);
    int timeStamp =
    int currentReading =
    client.print(devidID);
    client.print(" ");
    client.print(timeStamp);
    client.print(" ");
    client.print(currents[i], 3);
  }

  client.println("DATA TO SEND");
  /*How to format DATA
    SETUP: send some coordination info to sync the client with the server.
    DEVICE_ID TIME_STAMP CURRENT READING
  */

  delay(10);

  client.stop();
}
