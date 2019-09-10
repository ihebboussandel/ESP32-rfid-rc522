#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus

#include <WiFi.h> // init wifi lib 
#include <HTTPClient.h> // int http client
#include <WebServer.h>
#include <WiFiClient.h> // for the web pages 

#include "Pin_config.h"
#include "index.h"  //Web page header file
#include "loggedin.h"


//String enter;
int freq = 2000;
int channel = 0;
int resolution = 8;

//wifi data

const char* ssid= "Faty";
const char* password =  "38057b7d";

//used in authentication
MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 

WebServer server(80);

String card_status;
String auth = "no";
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
 //void handleLogin() {
// String s = LOGIN_page; //Read HTML contents
// server.send(200, "text/html", s); //Send web page
//}
 void handleLoginactionadd() {
  if (auth=="yes"){
    
 }
 }

void handleRFID() {
 int a = analogRead(A0);
 String adcValue = String(a);
 
 server.send(200, "text/plane", card_status); //Send cardstatus value only to client ajax request
}



void setup() 
{
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(14, channel);
  // Init MFRC522
  mfrc522.PCD_Init(); 
  delay(4000);   //Delay needed before calling the WiFi.begin
//ESP32 connects to your wifi -----------------------------------
WiFi.mode(WIFI_STA); //Connectto your wifi
WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to ");
     Serial.print(ssid);
  }
 
//If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

//----------------------------------------------------------------
 
  server.on("/", handleRoot);      //This is display page
  server.on("/readrfid", handleRFID);//To get update of rfid Value only
// server.on("/logged", handleLogin);//To get logged page
 // server.on("/logged?action=1", handleLoginactionadd);//To get logged page
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");


}

void loop() {
  server.handleClient();
  
   // Aguarda a aproximacao do cartao
   //waiting the card approach
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select a card
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

    // Dump debug info about the card; PICC_HaltA() is automatically called
//  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));</p><p>  //call menu function and retrieve the desired option
  int op = 0;
    readingData();
  /*menu();

  if(op == 0) 
    readingData();
  else if(op == 1) 
    Serial.print("wrintgdatawritingData();");
  else {
    Serial.println(F("Incorrect Option!"));
    return;
  }*/
 
//instructs the PICC when in the ACTIVE state to go to a "STOP" state
  mfrc522.PICC_HaltA(); 
  // "stop" the encryption of the PCD, it must be called after communication with authentication, otherwise new communications can not be initiated
  mfrc522.PCD_StopCrypto1();  
}

//reads data from card/tag
void readingData()
{
  //prints the technical details of the card/tag
  //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); 

   String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(mfrc522.uid.uidByte[i], HEX) +
    (i!=3 ? "" : "");
  }
  strID.toUpperCase();
  alert_bip();
  httprequest_toserver(strID);
Serial.println(strID);
//card_status=strID;
  //prepare the key - all keys are set to FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  //buffer for read data
  byte buffer[SIZE_BUFFER] = {0};
 
  //the block to operate
  byte block = 1;
  byte size = SIZE_BUFFER; //authenticates the block to operate
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(redPin, HIGH);
    delay(1000);
    digitalWrite(redPin, LOW);
    return;
  }

  //read data from block
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    digitalWrite(redPin, HIGH);
    delay(1000);
    digitalWrite(redPin, LOW);
    return;
  }
  else{
      digitalWrite(greenPin, HIGH);
      delay(1000);
      digitalWrite(greenPin, LOW);
  }

  //Serial.print(F("\nData from block ["));
  //Serial.print(block);Serial.print(F("]: "));

 //prints read data
  for (uint8_t i = 0; i < MAX_SIZE_BLOCK; i++)
  {
      Serial.write(buffer[i]);
  }
  Serial.println(" ");
}

int menu()
{
  Serial.println(F("\nChoose an option:"));
  Serial.println(F("0 - Reading data"));
  Serial.println(F("1 - Writing data\n"));

  //waits while the user does not start data
  while(!Serial.available()){};
  
  //retrieves the chosen option
  int op = (int)Serial.read();
  
  //remove all characters after option (as \n per example)
  while(Serial.available()) {
    if(Serial.read() == '\n') break; 
    Serial.read();
  }
  return (op-48);//subtract 48 from read value, 48 is the zero from ascii table
}
void alert_bip(){
     ledcWriteTone(channel, 500);
     delay(500);
    ledcWriteTone(channel, 0);
    delay(1000);
}

void httprequest_toserver(String id){
   if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;   
 
   http.begin("http://"+HOST_NAME+":8080/submit.php");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");             //Specify content-type header
 //String rfid="number="+id+"&auth="+auth;
 String rfid="number="+id+"&submit=enter";
   int httpResponseCode = http.POST(rfid);   //Send the actual POST request
 
   if(httpResponseCode>0){
 
    String response = http.getString();                       //Get the response to the request
   card_status=response;
   if(response.indexOf("welcome")>= 0){
    auth="yes";
     Serial.println(auth);
   }else if(response.indexOf("goodbye ")>= 0){
    auth="no";
    Serial.println(auth);
    
   }
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
 
   }else{
 
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
 
   }
 
   http.end();  //Free resources
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
}
