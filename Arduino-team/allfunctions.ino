//code for both sensors and stepper motor to work at same time
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Stepper.h>

// Temperature sensor definitions
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);   
DallasTemperature sensors(&oneWire);

// pH sensor definitions
#define pH_SensorPin 0
#define Offset 0.00
unsigned long int avgValue;

// TDS sensor definitions
#define TdsSensorPin A1
#define VREF 5.0
#define SCOUNT 30
int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0;


// Initialize the stepper library on pins 8 through 11:
const int stepsPerRevolution = 200;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 9, 10, 11);

// Control variable for data collection
int state = 1;

bool clockwise = true;
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0; 

void setup()
{
  // Initialize sensors
  sensors.begin();
  pinMode(pH_SensorPin, INPUT);
  pinMode(TdsSensorPin, INPUT);

  // Set the motor speed (RPMs):
  myStepper.setSpeed(100);

  // Initialize Serial communication
  Serial.begin(115200);
  Serial.println("Ready");

}

void loop()
{
  currentMillis = millis();

  if (currentMillis - previousMillis >= 2000){//replaces delay function so loop always running
    previousMillis = currentMillis;
    if(state == 0){
      Serial.println("case 0");
    }else if(state == 1){
      delay(100);
      Serial.println("case 1");
      // pH Measurement
      int buf[10];
      for (int i = 0; i < 10; i++)
      {
        buf[i] = analogRead(pH_SensorPin);
        delay(10);
      }
      for (int i = 0; i < 9; i++)
      {
        for (int j = i + 1; j < 10; j++)
        {
          if (buf[i] > buf[j])
          {
            int temp = buf[i];
            buf[i] = buf[j];
            buf[j] = temp;
          }
        }
      }
      avgValue = 0;
      for (int i = 2; i < 8; i++)
        avgValue += buf[i];

      float phValue = (float)avgValue * 5.0 / 1024 / 6;
      phValue = 3.5 * phValue + Offset;

      // Temperature Measurement
      sensors.requestTemperatures();
      float tempC = sensors.getTempCByIndex(0);
      float tempF = (tempC * 9.0) / 5.0 + 32.0;

      // TDS Measurement
      static unsigned long analogSampleTimepoint = millis();
      if (millis() - analogSampleTimepoint > 40U)
      {
        analogSampleTimepoint = millis();
        analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);
        analogBufferIndex++;
        if (analogBufferIndex == SCOUNT)
          analogBufferIndex = 0;
      }
      static unsigned long printTimepoint = millis();
      if (millis() - printTimepoint > 800U)
      {
        printTimepoint = millis();
        for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
          analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
        averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * VREF / 1024.0;
        float compensationCoefficient = 1.0 + 0.02 * (tempC - 25.0);
        float compensationVoltage = averageVoltage / compensationCoefficient;
        tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

        // Print results to Serial
        Serial.print("Temperature: ");
        Serial.print(tempC);
        Serial.print("C | ");
        Serial.print(tempF);
        Serial.print("F | pH: ");
        Serial.print(phValue, 2);
        Serial.print(" | TDS Value: ");
        Serial.print(tdsValue, 0);
        Serial.println("ppm");
      }
      //state = 2;
    }else if(state==2){
      Serial.println("case 2");
      if(clockwise){
        myStepper.step(200);
        clockwise = false;
      }else{
        myStepper.step(-200);
        clockwise = true;
      }
      state=1;
    }else{
      Serial.println("default case");
    }
    Serial.println("end loop action");
  }
  
}

int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    return bTab[(iFilterLen - 1) / 2];
  else
    return (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
}