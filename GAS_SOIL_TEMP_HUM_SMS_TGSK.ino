









































































#include <dht11.h>
#define DHT11PIN A3
#define pir 8
#define led 9
dht11 DHT11; 
#define gasPin A0
#define soilPin A2
#define echoPin 13 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 12 //attach pin D3 Arduino to pin Trig of HC-SR04
// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
#define RELAYT 11

int timesTosend=1;
int count=0;
int g=0;
int sn=0;
int sp=0;
int chk =0;
int t=0;
int h=0;
//char phone_no[] = "9032880328";  //phone number to alert
char phone_no[] = "9642484174";  //phone number to alert
//char phone_no[] = "9652961775";  //phone number to alert
//char phone_no[] = "9491632771";  //phone number to alert

#define RELAYPIN 10 // Arduino pin tied to Relaypin of the Relay Circuit
#define BUZZER 4 // Arduino pin tied to +ve terminal of the Buzzer

void setup()
{
    Serial.begin(9600);  //Open Serial connection at baud 9600
    delay(2000);
    pinMode(led,OUTPUT);// Relay pin as output pin
    digitalWrite(led,LOW);
    pinMode(pir,INPUT);// Relay pin as output pin
    digitalWrite(pir,LOW);
    pinMode(RELAYPIN,OUTPUT);// Relay pin as output pin
    digitalWrite(RELAYPIN,LOW); //Turn off the relay
    pinMode(BUZZER,OUTPUT);// Buzzer pin as output pin
    digitalWrite(BUZZER,LOW);
    pinMode(trigPin, OUTPUT);
    digitalWrite(trigPin, LOW);// Sets the trigPin as an OUTPUT
    pinMode(echoPin, INPUT);
    digitalWrite(echoPin, LOW);
    pinMode(RELAYT,OUTPUT);// Relay pin as output pin
    digitalWrite(RELAYT,LOW);//Turn off the Buzzer  
    Serial.println("AT+CMGF=1"); //set GSM to text mode
    delay(200);
    Serial.println("AT+CNMI=2,2,0,0,0\r");
    delay(100);
}

void loop()
{
  t=DHT11.temperature;
  h=DHT11.humidity;
  count=0;
  g= analogRead(gasPin);
  sn= analogRead(soilPin);
  int chk = DHT11.read(DHT11PIN);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.034) / 2;
//.....................................sensor data on serial monitor -> LCD try it..............................

  Serial.print("gas sensor:");
  Serial.println(g);
  delay(1000);
  Serial.print("soil sensor:");
  Serial.println(sn);
  delay(1000);
  Serial.print("Humidity (%): ");
  Serial.println(h);
  delay(1000);
  Serial.print("Temperature (C): ");
  Serial.println(t);
  delay(2000);
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

//.......................................Connecting to THINGSPEAK....................................................

          if (Serial.available())
              Serial.write(Serial.read());
           
            Serial.println("AT");
            delay(1000);
           
            Serial.println("AT+CPIN?");
            delay(1000);
           
            Serial.println("AT+CREG?");
            delay(1000);
           
            Serial.println("AT+CGATT?");
            delay(1000);
           
            Serial.println("AT+CIPSHUT");
            delay(1000);
           
            Serial.println("AT+CIPSTATUS");
            delay(2000);
           
            Serial.println("AT+CIPMUX=0");
            delay(2000);
           
            ShowSerialData();
           
            Serial.println("AT+CSTT=\"bsnlnet\"");//start task and setting the APN,
            delay(1000);
           
            ShowSerialData();
           
            Serial.println("AT+CIICR");//bring up wireless connection
            delay(3000);
           
            ShowSerialData();
           
            Serial.println("AT+CIFSR");//get local IP adress
            delay(2000);
           
            ShowSerialData();
           
            Serial.println("AT+CIPSPRT=0");
            delay(3000);
           
            ShowSerialData();
            
            Serial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
            delay(6000);
           
            ShowSerialData();
           
            Serial.println("AT+CIPSEND");//begin send data to remote server
            delay(4000);
            ShowSerialData();
            
            String str= "GET https://api.thingspeak.com/update?api_key=X54NE8J38RP6FUOD&field1=" +String(t) +"&field2=" +String(h)+ "&field3=" +String(g) +"&field4=" +String(sn);
            Serial.println(str);//begin send data to remote server
            
            delay(4000);
            ShowSerialData();
           
            Serial.println((char)26);//sending
            delay(5000);//waitting for reply, important! the time is base on the condition of internet 
            Serial.println();
           
            ShowSerialData();
           
            Serial.println("AT+CIPSHUT");//close the connection
            delay(100);
            ShowSerialData(); 
            delay(5000);

//.............................................SMS & CALL if sensors cross threshold values..............................
          Serial.print("AT + CMGS=\"");
          Serial.print(phone_no);
          Serial.println("\"");
          delay(3000); 
  if (g >= 400 || sn>=600 || t>31) // humidity need to be added.
  {
        while(count<timesTosend)
        {
          delay(1500);
          if(g>=200)
          {
            Serial.print("CAUTION...FIRE ALARM !!!!!!!!!!");//SMS body
            digitalWrite(BUZZER,HIGH);
            delay(5000);
            digitalWrite(BUZZER,LOW);
          }
          if(t>31)
          {
            Serial.print("CAUTION...TEMP ALARM !!!!!!!!!!");
            digitalWrite(BUZZER,HIGH);
            delay(5000);
            digitalWrite(BUZZER,LOW);
          }
//          if(h<95)
//          {
//            Serial.print("CAUTION...Humidity ALARM !!!!!!!!!!");
//            digitalWrite(BUZZER,HIGH);
//            delay(5000);
//          }         
         if(sn>500 && sp<500)
         {
          Serial.print("CAUTION MOISTURE ALARM,MOTOR ACTIVATED");
         digitalWrite(BUZZER, HIGH); 
         delay(5000);
         digitalWrite(BUZZER,LOW);
         digitalWrite(RELAYPIN,HIGH);
         }
                     
            delay(1000);
            count++;
      }
      delay(2000);
  }
  else if(sn<500 && sp>500)
  {
     Serial.print("MOTOR OFF");
     digitalWrite(BUZZER, LOW); 
     digitalWrite(RELAYPIN,LOW);
  } 
  
  
  if(distance<=5)
  {
    Serial.println("Tank is full,Motor off");
    digitalWrite(RELAYT,LOW);
  }
  else if(distance>15)
  {
    Serial.println("Tank is Not full,Motor ON");
    digitalWrite(RELAYT,HIGH);
  }
  
 
//....................................PIR...........................................

if(digitalRead(pir)== HIGH)
    {  
      //Serial.println("OK");
      digitalWrite(led,HIGH);
      delay(2000);
//      Serial.print("AT + CMGS=\"");
//      Serial.print(phone_no);
//      Serial.println("\"");
      Serial.println("....THIEF DETECTED....");
      delay(5000);
      digitalWrite(BUZZER, HIGH); 
      delay(5000);
      digitalWrite(BUZZER,LOW); 
    }
     Serial.write(0x1A);  // sends ctrl+z end of message
    delay(1000);

    sp=sn;
} 
//....................................end of loop...................................................
void ShowSerialData()
{
  while(Serial.available()!=0)
  Serial.write(Serial.read());
  delay(3000); 
  
}
