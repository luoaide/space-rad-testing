//SetupControllino MAXI as a network server: https://forum.controllino.biz/viewtopic.php?t=2131
#include <SPI.h>
#include <Ethernet.h>

#define NUM_DEV 1

// Constants
const int SENSOR_PIN = A0; // Input pin for measuring Vout

const int RS = 10; // Shunt resistor value (in ohms)
const int VOLTAGE_REF = 5; // Reference voltage for analog read

// Global Variables
float sensorValue; // Variable to store value from analog read
float current; // Calculated current value
EthernetClient client;

void setup() {
  // Initialize serial monitor
  Serial.begin(9600);

  //I probably eventually want to open up the server connection here so taht I don't have
  // to start and stop the connection every loop!

  byte mac_addr[] = {0xA8, 0x61, 0x0A, 0xAE, 0x75, 0xEF};

  //the ip address of the Controllino or each Arduino
  IPAddress my_ip(169, 254, 138, 1); //what is it actually? does it matter?
  IPAddress server_ip(169, 254, 138, 192);

  //EthernetClient client;

  Ethernet.begin(mac_addr, my_ip);

  client.connect(server_ip, 65432);

  client.print("0 1 5");
}

void loop() {
  client.print("0 1 5");
  /*
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
  int currents[NUM_DEV];
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
  pushToServer(int* currents, NUM_DEV);
  // Delay program for a few milliseconds
  */
  delay(100);
}
//The Arduino board communicates with the shield using the SPI bus. This is on digital pins 11, 12, and 13 on the Uno... https://www.arduino.cc/en/Reference/Ethernet

//code adapted from: https://www.arduino.cc/en/Tutorial/LibraryExamples/BarometricPressureWebServer#code
//ALSO: https://www.arduino.cc/en/Reference/Ethernet

void pushToServer(int* currents, int len) {
  //mac address: fill with the proper address
  byte mac_addr[] = {0xA8, 0x61, 0x0A, 0xAE, 0x75, 0xEF};

  //the ip address of the Controllino or each Arduino
  IPAddress my_ip(169, 254, 138, 1); //what is it actually? does it matter?
  IPAddress server_ip(169, 254, 138, 192);

  EthernetClient client;

  Ethernet.begin(mac_addr, my_ip);

  client.connect(server_ip, 65432);

  delay(10);


  for(int i = 0; i<NUM_DEV; i){
    //FORMAT:  DEVICE_ID TIME_STAMP CURRENT READING
    int deviceID = i;
    int timeStamp = 2;
    int currentReading = currents[i];
    client.print(deviceID);
    client.print(" ");
    client.print(timeStamp);
    client.print(" ");
    client.print(currents[i], 3);
  }

  delay(10);

  client.stop();
}
