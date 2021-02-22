  // Constants
  const int SENSOR_PIN = A0; // Input pin for measuring Vout
  const int RS = 10; // Shunt resistor value (in ohms)
  const int VOLTAGE_REF = 5; // Reference voltage for analog re   ad
  // Global Variables
  float sensorValue; // Variable to store value from analog read
  float current; // Calculated current value

void setup() {
  // Initialize serial monitor
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
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
  // Delay program for a few milliseconds
  delay(500);
}
