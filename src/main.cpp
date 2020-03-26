/*
 * ESP8266 ALTIMETER for aircraft models 
 *
 */

#include "Arduino.h"
#include "system_status.h"
#include <ESP8266WiFi.h>
#include <FS.h> //Include File System Headers
#include "Calculate_Alt.h"
#include "Wire.h"
#include "Ticker.h"
#include "SVG_Web.h"
#include "Sp_File_Sys.h"
#include "Calculate_Alt.h"
#include "MS5611.h"

uint16_t Flight_Time[10000];
extern int8_t dFile_recorded;


Altitude fD;
Web_Graph wG;
char ledState = 0;
double dPS;

void WiFi_Start(void);
void Sensors_check_and_start(void);
void GPIO_TIM_setup(void);

void setup()
{
  delay(1000);
  Serial.begin(115200);
  Wire.begin(4, 5);
  Sensors_check_and_start();
  Initialize_File_System();
  GPIO_TIM_setup();
  wG.WiFi_Start();
}

void loop()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(2, HIGH);
    dFile_recorded = 0x00;
    fD.Write_Data_to_Massive();
    // delay(150);
    digitalWrite(2, LOW);
  }

  if (dFile_recorded == 0x00)
  {
    Open_and_Write_File();
    HTTP_Start();
  }
  
  if (dFile_recorded == 0x01)
  {
    wG.main_web_cycle();
  }
}

void ICACHE_RAM_ATTR onTimerISR()
{
  if (ledState == 0)
  {
    digitalWrite(ledPin, HIGH);
    ledState = 1;
  }
  else
  {
    digitalWrite(ledPin, LOW);
    ledState = 0;
  }
}

void Sensors_check_and_start()
{
#ifdef bSensor
Adafruit_BMP085 t;
  if (!t.begin())
  {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1)
    {
    }
  }
#endif
#ifdef mSensor
Altitude aTD;
  Serial.println(aTD.Pressure_in_Start());
  delay(1500);
#endif  
}

void GPIO_TIM_setup()
{
  pinMode(2, OUTPUT);
  pinMode(12, INPUT);
  // attachInterrupt(digitalPinToInterrupt(12), Run_Interrupt_func, RISING);
  timer1_attachInterrupt(onTimerISR);
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  timer1_write(1500);
}