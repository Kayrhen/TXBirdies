
#include <WiFiUdp.h>

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h> 


int inval0 = 0;
int inval1 = 0;
bool outval0 = false;
bool outval1 = false;
bool flag0 = true;
bool flag1 = true;

String apiKey = "49SXL9USANK2ICFU";
 char* ssid = "ParkWest.SynergyWifi.com";
 char* password = "Synergy.203.280.2029";
const char* server = "api.thingspeak.com";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

WiFiClient client;

void setup() 
{
  Serial.begin(115200);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

 /* WiFi.begin(ssid, password);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());*/

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(-21600);
  
  pinMode(D0, INPUT);
  pinMode(D8, INPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);

  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
}

void loop() 
{
  
  if(flag0)
  {
    inval0 = digitalRead(D0);
    if(inval0 == HIGH)
    {
      digitalWrite(D5, HIGH);
      outval0 = true;
      
      while(!timeClient.update()) 
      {
        timeClient.forceUpdate();
      }
      // The formattedDate comes with the following format:
      // 2018-05-28T16:00:13Z
      // We need to extract date and time
      formattedDate = timeClient.getFormattedDate();
      Serial.println(formattedDate);
    
      // Extract date
      int splitT = formattedDate.indexOf("T");
      String year = formattedDate.substring(0, 4);
      dayStamp = formattedDate.substring(5, splitT);
      dayStamp = dayStamp + "-" + year;
      
      Serial.print("DATE: ");
      Serial.println(dayStamp);
      // Extract time
      timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
      Serial.print("HOUR: ");
      Serial.println(timeStamp);
    }
  }
  
  if(flag1)
  {
    inval1 = digitalRead(D8);
    if(inval1 == HIGH)
    {
      digitalWrite(D6, HIGH);
      outval1 = true;
      
      while(!timeClient.update()) 
      {
        timeClient.forceUpdate();
      }
      // The formattedDate comes with the following format:
      // 2018-05-28T16:00:13Z
      // 0123456789
      // We need to extract date and time
      formattedDate = timeClient.getFormattedDate();
      Serial.println(formattedDate);
    
      // Extract date
      int splitT = formattedDate.indexOf("T");
      String year = formattedDate.substring(0, 4);
      dayStamp = formattedDate.substring(5, splitT);
      dayStamp = dayStamp + "-" + year;
      
      Serial.print("DATE: ");
      Serial.println(dayStamp);
      // Extract time
      timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
      Serial.print("HOUR: ");
      Serial.println(timeStamp);
    }
  }

  if(client.connect(server,80)) 
  {
    String postStr = apiKey;
    
    postStr +="&field3=";
    if(outval0 == 0)
      postStr += String(outval0);
    
    if(outval0 == 1)
    {
        flag0 = false;
        postStr += dayStamp + " ";
        postStr += timeStamp;
    }

    postStr += "&field4=";
    if(outval1 == 0)
      postStr += String(outval1);
      
      if(outval1 == 1)
      {
        flag1 = false;
        postStr += dayStamp + " ";
        postStr += timeStamp;
      }     
        
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  client.stop();

  Serial.println("Waiting 20 secs");
// thingspeak needs at least a 15 sec delay between updates
// 20 seconds to be safe
    delay(20000);
  //digitalWrite(13, HIGH);
}
