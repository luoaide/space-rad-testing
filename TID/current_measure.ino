//SetupControllino MAXI as a network server: https://forum.controllino.biz/viewtopic.php?t=2131
#include <SPI.h>
#include <Ethernet.h>

//mac address: fill with the proper address
byte mac_addr[] = {0xE8, 0x2A, 0xEA, 0x4B, 0x1F, 0xC3};

//the ip address of the Controllino
IPaddress my_ip(192, 168, 6, 1);

EthernetServer server(80); //80 is the port number on which Controllino will listen. We're making a webserver

Ethernet.begin(mac_addr, my_ip);


// Constants
const int SENSOR_PIN = A0; // Input pin for measuring Vout
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
  sensorValue = analogRead(SENSOR_PIN);

  // Remap the ADC value into a voltage number (5V reference)
  sensorValue = (sensorValue * VOLTAGE_REF) / 1023;

  // Follow the equation given by the INA169 datasheet to
  // determine the current flowing through RS. Assume RL = 10k
  // Is = (Vout x 1k) / (RS x RL)
  current = sensorValue / (10 * RS);

  // Output value (in amps) to the serial monitor to 3 decimal
  // places
  Serial.print(current, 3);
  Serial.println(" A");

  //update the webserver
  listenForEthernetClients();

  // Delay program for a few milliseconds
  delay(500);
}



//The Arduino board communicates with the shield using the SPI bus. This is on digital pins 11, 12, and 13 on the Uno... https://www.arduino.cc/en/Reference/Ethernet

//code adapted from: https://www.arduino.cc/en/Tutorial/LibraryExamples/BarometricPressureWebServer#code
//ALSO: https://www.arduino.cc/en/Reference/Ethernet

void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient conn_client = server.available();

  if(conn_client) {
    Serial.println("Got a client");

    // an http request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          // print the current readings, in HTML format:
          client.print("Temperature: ");
          client.print(temperature);
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}
