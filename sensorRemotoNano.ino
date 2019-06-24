

//#include "ESP8266WiFi.h"


#include <OneWire.h> 
#include <DallasTemperature.h>


#include <Adafruit_Sensor.h>
#include <DHT.h>

#include <DHT_U.h>
#include <stdint.h>

#define RELE_PIN (9)
#define TEMP_PIN (10)
#define DELAY    (1000)    
#define DHTTYPE    (DHT11) 
#define MAX_TIME_RECORD (240u)

uint8_t  gReleStatus;
uint32_t delayMS;
float gTimeRecord[MAX_TIME_RECORD] = {0};
uint32_t gCountertimeRecord = 0u;
uint8_t bufferFull = false;
static uint32_t counterErrorsTemp = 0u;


OneWire oneWire(TEMP_PIN); 
DallasTemperature sensors(&oneWire);
DHT_Unified dht(14, DHTTYPE);



/*Functions*/
static void init_sensors(void);
static void status_temp(void);

static bool getReleStatus(void); 
static void setReleStatus(bool releStatus);


static float  getTemp(void);

static size_t getTempLastTSeconds(uint32_t seconds,float* temps);

static void resetTimeRecord();
static void newTemp(void);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Initialize device.
  init_sensors();
  status_temp();
  
  // Set delay between sensor readings based on sensor d  etails.
  delayMS = 500;

  
}

void loop() {
  
  delay(DELAY);

  
  static uint8_t counterTemp = 0;
  if (counterTemp % 2)
		{
			newTemp();
		}

  counterTemp++;

  
  uint8_t command;
  float temp1min[60u]={0};
  uint8_t i=0u;
  //Serial.println("Command: ");
  size_t elementToPrint=0u;
  if (Serial.available() > 0) 
    {
      command = Serial.read();
    
      //Serial.println("CommandRead");
      switch (command)
        {
        case ('1'):
          {
            //Serial.print("Temperature: ");
            Serial.print(getTemp());
            break;
          }
        case('2'):
          {
						// Serial.print("Temperature (1 min):  ");
            elementToPrint = getTempLastTSeconds(60u,temp1min); 
            for (;i<elementToPrint;i++)
              {
                Serial.print(",");
                Serial.print(temp1min[i]);
        
              }
            break;
          }
        case ('3'):
          {
            setReleStatus(!getReleStatus());
            break;
          }
				case ('4'):
          {
            Serial.println(counterErrorsTemp);
            break;
          }
        }
    }
   
}


static void init_sensors (void)
{

  
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, LOW);
  gReleStatus = LOW;

	sensors.begin();
 
 
}




static void status_temp(void)
{
  //Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  //sensor_t sensor;
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);

  if (temp != DEVICE_DISCONNECTED_C)
		{
			Serial.print(temp);
		}
 
  Serial.println(F("------------------------------------"));  
}


static bool getReleStatus(void)
{ 
  return gReleStatus;
}
static void setReleStatus(bool releStatus)
{
  gReleStatus = releStatus;
  digitalWrite(RELE_PIN, releStatus); 
  
}
  
static float getTemp(void)
{ 
  float auxTemp[10u]={0};
  float temp=0u;
  uint8_t i=0u;
  size_t counterTemperatures;
  counterTemperatures = getTempLastTSeconds(10u,auxTemp);
 
  for (;i<counterTemperatures;i++)
    {
			temp+= auxTemp[i];
    }
  temp = temp / counterTemperatures ;
  return temp;
}
static size_t getTempLastTSeconds(uint32_t seconds,float* temps)
{  
  uint32_t i;
  size_t elements = 0u;
  if (gCountertimeRecord >= seconds)
    {
      memcpy(temps,&gTimeRecord[gCountertimeRecord-seconds],sizeof(float)*seconds);
      elements = seconds;
    }
  else if (bufferFull)
    {
      memcpy(temps,gTimeRecord,sizeof(float)*gCountertimeRecord);
      memcpy(temps,&gTimeRecord[MAX_TIME_RECORD-seconds],sizeof(float)*seconds);
      elements = seconds;
    }
  else
    {
      memcpy(temps,gTimeRecord,sizeof(float)*gCountertimeRecord);
      elements = gCountertimeRecord;
    }
  
  return elements;
}
  
static void newTemp(void)
{
  static bool firstTemperature = true;
  static float avgTemperature = 0.0f;

  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);

  if (temp != DEVICE_DISCONNECTED_C)
		{
    
  
 

			if(firstTemperature)
				{
					avgTemperature = temp;
					firstTemperature = false;
				} 
			else if(temp > avgTemperature * 1.30f ||  temp < avgTemperature * 0.70)
				{
					counterErrorsTemp++;
				}
			else
				{
					gTimeRecord[gCountertimeRecord % MAX_TIME_RECORD]= temp;

					gCountertimeRecord == MAX_TIME_RECORD ? bufferFull = true: bufferFull = false;
  
					gCountertimeRecord = (gCountertimeRecord + 1) % MAX_TIME_RECORD; 
					avgTemperature = (avgTemperature + temp) /2 ;
				}
		}
  
}
