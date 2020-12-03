#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <SoftwareSerial.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
SoftwareSerial BTserial(0,1); //rx/tx

float roomTemp; // ambient temperature
float objectTemp, stemp; // object temperature

int readcount = 0;

float threshold= 00.00 ;  // for calibration with oral thermometer

// --------- Ultrasonic Sensor preparation
#define echoPin 11 // Echo Pin
#define trigPin 12 // Trigger Pin
int maximumRange = 25; // Maximum range needed
int minimumRange = 15; // Minimum range needed
long duration, distance; // Duration used to calculate distance
int dtime;

void setup() 
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  BTserial.begin(9600);
}

void loop()

{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  //Calculate the distance (in cm) based on the speed of sound.
  distance= duration*0.034/2;

  // reading object and ambient  temperature
  objectTemp = threshold + mlx.readObjectTempC() ;  
  roomTemp = mlx.readAmbientTempC() ;  

  // print to Serial port
 Serial.println("Object:" + String(objectTemp) + ", Ambient:" + String(roomTemp));
 Serial.println(distance);
 
  // data from sensor to device via blutooth

  BTserial.print("Dis:" + String(distance) + "cm");

  BTserial.print("Room:" + String(roomTemp).substring(0, 4)  + "C");

 if (distance > maximumRange) 
  {
    BTserial.print("GET CLOSER");
  }
  if (distance < minimumRange) 
  {
    BTserial.print("TOO CLOSE!");
 
  }
  if ((distance >= minimumRange) && (distance <= maximumRange)) 
   {
    if (readcount == 5) 
    {   // after reading 5 consecutive time
      disptemp();
      
    } 
    else
    {
      BTserial.print("HOLD ON"); // when in range, ask user to hold position
      stemp = stemp + objectTemp;
      readcount++;
      dtime = 200;      // until approx. 5 x 200 ms = 1 sec
    }
  }
  else
  {     
    // if user is out of range, reset calculation
    dtime = 100;
    readcount = 0;
    stemp = 0;
  }
  
  delay(dtime);
  BTserial.println("count  :"+String(readcount));
 }

 void disptemp()
 {
      objectTemp = stemp / 5;       // get the average reading of temp
      
      BTserial.print("YOUR TEMP:");
      BTserial.print(String(objectTemp).substring(0, 4) + "C");
      readcount = 0;
      stemp = 0;
      if (objectTemp >= 37.5) 
      {
        play_alert();
      }
      else
      {
        play_ok();
      }
      dtime = 5000; // wait for 5 seconds
 }

void play_ok() 
{  
  // play three sequential notes when object temperature is below 37.5C
  tone(3, 600, 1000);  // pin,frequency,duration
  delay(200);
  tone(3, 750, 500);
  delay(100);
  tone(3, 1000, 500);
  delay(200);
  noTone(3);
}

void play_alert() 
{
  // beep 3x when object temperature is >= 37.5C
  tone(3, 2000, 1000);
  delay(1000);
  tone(3, 3000, 1000);
  delay(1000);
  tone(3, 4000, 1000);
  delay(1000);
  noTone(3);
}
