//SetupControllino MAXI as a network server: https://forum.controllino.biz/viewtopic.php?t=2131
#include <SPI.h>
#include <Ethernet.h>
#include <time.h>

#define NUM_DEV 1 //change the number of devices configured to output current
#define RS 10 // Shunt resistor value (in ohms)
#define VOLTAGE_REF 5 // Reference voltage for analog read
#define PORT 65432

//length of SENSOR_PINS must agree with the number of devices defined above...
int SENSOR_PINS[] = {A0}; // Input pin for measuring Vout. Will change when implemented with Controllino

// Global Variables
EthernetClient client;
float *sensorValues;
time_t startTime;

byte mac_addr[] = {0xA8, 0x61, 0x0A, 0xAE, 0x75, 0xEF};
IPAddress my_ip(169, 254, 138, 1); //the ip address of the Controllino or each Arduino... does this set it?
IPAddress server_ip(169, 254, 138, 192); //the ip address of the server (plotter.py script)

void setup() {
  // Initialize serial monitor
  Serial.begin(9600);
  startTime = time(NULL);

  establishServerConn();
}

void loop() {
  Serial.print("starting\n");
  if(!client.connected()) {
    establishServerConn();
  }
  else {
    readSensors(); //read input and calculate currents
    pushToServer(); //send the calculated current values to the server through a helper function
  }

  // Delay program for a few milliseconds
  delay(100);
}

void establishServerConn() {
  //Open up server connection in setup so that loop() can repeatedly use same connection
  Ethernet.begin(mac_addr, my_ip);
  int conn_status = client.connect(server_ip, PORT);
  //.connect Returns an int (1,-1,-2,-3,-4) indicating connection status :
  // SUCCESS 1
  // TIMED_OUT -1
  // INVALID_SERVER -2
  // TRUNCATED -3
  // INVALID_RESPONSE -4
}

void readSensors() {
  sensorValues = (float *)malloc(sizeof(float)*NUM_DEV); // Variable to store value from analog reads
  if(sensorValues == NULL) {
    Serial.println("malloc failed");
  }

  // Read a value from the INA169 board
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
  for(int i = 0; i<NUM_DEV; i++){
    sensorValues[i] = sensorValues[i] / (10 * RS);
  }

  // Output value (in amps) to the serial monitor to 3 decimal
  for(int i = 0; i<NUM_DEV; i++){
    Serial.print("Current on Board ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(sensorValues[i], 3);
    Serial.println(" A");
  }
}


//Resource Used: https://www.arduino.cc/en/Reference/Ethernet
void pushToServer() {
  for(int i = 0; i<NUM_DEV; i++){
    //FORMAT:  DEVICE_ID TIME_STAMP CURRENT READING
    int deviceID = i;
    int timeStamp = time(NULL) - startTime;
    float currentReading = sensorValues[i];
    //https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/
    String output = String(deviceID) + String(" ") + String(timeStamp) + String(" ") + String(sensorValues[i], 3);
    client.print(output);
  }
  free(sensorValues); //free the malloced memory after this iteration of the loop.
}
