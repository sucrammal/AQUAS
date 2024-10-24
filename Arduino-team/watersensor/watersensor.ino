// FS-IA6B reciever - Don't use top row. From top to bottom signal, power, ground

/*
  Arduino FS-I6X Demo
  fsi6x-arduino-uno.ino
  Read output ports from FS-IA6B receiver module
  Display values on Serial Monitor
  
  Channel functions by Ricardo Paiva - https://gist.github.com/werneckpaiva/
  
  DroneBot Workshop 2021
  https://dronebotworkshop.com
*/

// Define Input Connections
//#define CH1 3
//#define CH2 5
//#define CH3 6
//#define CH4 9
#define CH5 10
#define CH6 11

// Integers to represent values from sticks and pots
//int ch1Value;
//int ch2Value;
//int ch3Value;
//int ch4Value; 

// Boolean to represent switch value
bool ch5Value;
bool ch6Value;

// Read the number of a specified channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue){
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read the switch channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue){
  int intDefaultValue = (defaultValue)? 100: 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}

void setup(){
  // Set up serial monitor
  Serial.begin(115200);
  
  // Set all pins as inputs
//  pinMode(CH1, INPUT);
//  pinMode(CH2, INPUT);
//  pinMode(CH3, INPUT);
//  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);
}


void loop() {
  
  // Get values for each channel
//  ch1Value = readChannel(CH1, -100, 100, 0);
//  ch2Value = readChannel(CH2, -100, 100, 0);
//  ch3Value = readChannel(CH3, -100, 100, -100);
//  ch4Value = readChannel(CH4, -100, 100, 0);
//  ch5Value = readChannel(CH5, -100, 100, 0);
  ch5Value = readSwitch(CH5, false);
  ch6Value = readSwitch(CH6, false);
  
  // Print to Serial Monitor
//  Serial.print("Ch1: ");
//  Serial.print(ch1Value);
//  Serial.print(" | Ch2: ");
//  Serial.print(ch2Value);
//  Serial.print(" | Ch3: ");
//  Serial.print(ch3Value);
//  Serial.print(" | Ch4: ");
//  Serial.print(ch4Value);
  Serial.print(" | Ch5: ");
  Serial.print(ch5Value);
  Serial.print(" | Ch6: ");
  Serial.println(ch6Value);
  
  delay(500);
}
