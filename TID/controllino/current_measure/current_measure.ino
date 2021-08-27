//SetupControllino MAXI as a network server: https://forum.controllino.biz/viewtopic.php?t=2131
#include <SPI.h>
#include <Ethernet.h>
#include <Controllino.h>
#include <time.h>

#define NUM_DEV 1 //change the number of devices configured to output current
#define RS 10 // Shunt resistor value (in ohms)
#define VOLTAGE_REF 5 // Reference voltage for analog read
#define PORT 65432

// Global Variables
EthernetClient client;
byte mac_addr[] = {0x50, 0xD7, 0x53, 0x00, 0x0A, 0xF9}; //from contact at: info@controllino.biz....: 50 D7 53 00 0A F9
IPAddress my_ip(192, 168, 1, 253); //the ip address of the Controllino or each Arduino... does this set it?
IPAddress dg_ip(192, 168, 1, 254); //not needed, but oh well
IPAddress server_ip(192, 168, 1, 1); //the ip address of the server (plotter.py script)

void setup() {
  // Initialize Pins
  pinMode(CONTROLLINO_A0, INPUT);
  pinMode(CONTROLLINO_A1, INPUT);
  pinMode(CONTROLLINO_A2, INPUT);
  pinMode(CONTROLLINO_A3, INPUT);
  pinMode(CONTROLLINO_A4, INPUT);

  // Initialize Time
  Controllino_RTC_init(0);
  Controllino_SetTimeDate(1,1,1,00,0,0,0); // set initial values to the RTC chip to: January 1st, 2000 @ 0000

  // Initialize serial monitor
  Serial.begin(9600);
  establishServerConn();

  Serial.print("IP ADDRESS: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  Serial.print("Query Time: ");
  //Serial.println("%c:%c:%c", Controllino_GetHour(), Controllino_GetMinute(), Controllino_GetSecond());

  if(!client.connected()) {
    establishServerConn();
  } else {
    readSensors(); //read input and calculate currents
  }

  // Set Interval for Current Reads to 100ms
  delay(100);
}

void establishServerConn() {
  //Open up server connection in setup so that loop() can repeatedly use same connection
  Ethernet.begin(mac_addr, my_ip, dg_ip, dg_ip);

  int conn_status = client.connect(server_ip, PORT);
  //client.connect Returns a 1 or 0 for success or failure.
  if(conn_status == 0) {
    Serial.println("connection failed...");
  }
}

void readSensors() {
  // Read values from the INA169 board: supports 5 current readings because that's how the hardware on the Controllino is setup.
  float pinA0Raw = analogRead(CONTROLLINO_A0); // returns 0 to 1023, depending on power input
  float pinA1Raw = analogRead(CONTROLLINO_A1); // returns 0 to 1023, depending on power input
  float pinA2Raw = analogRead(CONTROLLINO_A2); // returns 0 to 1023, depending on power input
  float pinA3Raw = analogRead(CONTROLLINO_A3); // returns 0 to 1023, depending on power input
  float pinA4Raw = analogRead(CONTROLLINO_A4); // returns 0 to 1023, depending on power input

  // Remap the ADC value into a voltage number (5V external reference)
  float pinA0Val = (pinA0Raw * VOLTAGE_REF) / 1023;
  float pinA1Val = (pinA1Raw * VOLTAGE_REF) / 1023;
  float pinA2Val = (pinA2Raw * VOLTAGE_REF) / 1023;
  float pinA3Val = (pinA3Raw * VOLTAGE_REF) / 1023;
  float pinA4Val = (pinA4Raw * VOLTAGE_REF) / 1023;

  // Follow the equation given by the INA169 datasheet to
  // determine the current flowing through RS. Assume RL = 10k
  // Is = (Vout x 1k) / (RS x RL)
  float A0Current = pinA0Val / (10 * RS);
  float A1Current = pinA1Val / (10 * RS);
  float A2Current = pinA2Val / (10 * RS);
  float A3Current = pinA3Val / (10 * RS);
  float A4Current = pinA4Val / (10 * RS);

  // Output value (in amps) to the serial monitor to 3 decimal
  Serial.print("Current on Board 1: ");
  Serial.print(A0Current, 3);
  Serial.println(" A");

  Serial.print("Current on Board 2: ");
  Serial.print(A1Current, 3);
  Serial.println(" A");

  Serial.print("Current on Board 3: ");
  Serial.print(A2Current, 3);
  Serial.println(" A");

  Serial.print("Current on Board 4: ");
  Serial.print(A3Current, 3);
  Serial.println(" A");

  Serial.print("Current on Board 5: ");
  Serial.print(A4Current, 3);
  Serial.println(" A");

  //Send all collected current measurements to the server:
  //FORMAT:  DEVICE_ID TIME_STAMP CURRENT READING

  //Resource Used: https://www.arduino.cc/en/Reference/Ethernet
  //https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/

  //String timeStamp = String(Controllino_GetHour()) + String(":") + String(Controllino_GetMinute()) + String(":") + String(Controllino_GetSecond());
  int timeStamp = 1;

  String A0_out = String("Board_1_A0") + String(" ") + String(timeStamp) + String(" ") + String(A0Current, 3);
  String A1_out = String("Board_2_A1") + String(" ") + String(timeStamp) + String(" ") + String(A1Current, 3);
  String A2_out = String("Board_3_A2") + String(" ") + String(timeStamp) + String(" ") + String(A2Current, 3);
  String A3_out = String("Board_4_A3") + String(" ") + String(timeStamp) + String(" ") + String(A3Current, 3);
  String A4_out = String("Board_5_A4") + String(" ") + String(timeStamp) + String(" ") + String(A4Current, 3);

  client.print(A0_out);
  client.print(A1_out);
  client.print(A2_out);
  client.print(A3_out);
  client.print(A4_out);
}
