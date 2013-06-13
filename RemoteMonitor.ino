/* ********************************************************
   Title: Remote Monitoring and Control using GSM-SMS 
   Author: Peter John
   Date: 8th June 2013
   Version: v1.01
***********************************************************/
#include <Bounce.h>
#include <Metro.h>
#include <SHT1x.h>
#include <EEPROM.h>
#include <EEPROMData.h>

/** IO Pins **/
/* Leds For Status Checking */
const int ledOrange = 13; // Teensy 3.0 has LED on 13
const int ledGreen = 16;
const int ledRed = 15;
const int ledBlue = 14;
/* States for each LED */
int ledOrangeState = LOW;   // Orange indicates Board Running
int ledGreenState = LOW; // Green indicates Success
int ledRedState = LOW;  // Red indicates Error
int ledBlueState = LOW; // Blue indicates Network Connected and SIM Present and Ready
/* Timers for each LED */
Metro ledOrangeBlink = Metro(1000);  // Led Blink Event Timer

/* Buzzer for Alarms */
const int buzzerPin = 23;

/* Debouncer for Button 1 and 2*/
const int btn1Pin = 12;
Bounce btn1 = Bounce(btn1Pin, 10);  // 10 ms debounce
const int btn2Pin = 11;
Bounce btn2 = Bounce(btn2Pin, 10);  // 10 ms debounce

/** Temperature Variables **/
const int tempPin = A3; //17
Metro tempReadTimer = Metro(2000);  // Temperature Read Event Timer
float tempC = 0;
float voltage = 0;
int readValue = 0;
float tempSetPoint = 40.00; // 40.00 degrees celsius

/** Humidity Variables **/
const int humidityDataPin = 20;
const int humidityClkPin = 21;
Metro humidityReadTimer = Metro(2000);  // Humidity Read Event Timer
float humidityRH = 0;
SHT1x sht1x(humidityDataPin, humidityClkPin);

/** Temperature Control Variables **/
enum {AUTO, MAN};
boolean tempMode = AUTO;
boolean running = true; // Initially Heater is Running
int relayState = HIGH;  // Initially Relay is ON
const int relayPin = 19; // Relay is on pin 19
 
/** Serial Port Received Strings **/
String inputString = "";         // Incoming data from GSM
Metro commTimer = Metro(10000);  // Red Led Blink Event Timer
boolean commError = false;
boolean simReady = false;
boolean netReady = false;
Metro statusTimer = Metro(10000);  // Blue Led Event Timer
char charBuf[15];
String mobileNo = "9036658002";
void sendMessage(String a,float b);

/* Setup Function
   This function runs just once during the startup of the Board
   1.The serialport buffer is reserved 100 bytes of memory
   2.All Digital Pins are configured as INPUT/INPUT_PULLUP/OUTPUT
   3.Serial Port to PC is initialized at 9600 baud rate
   4.Serial Port to GSM Module is initialized at 9600 baud rate
   5.ALL LEDS are on for about 10 seconds indicating the board is switched ON
   6.Recieve Notification of GSM Modem is Set
*/
void setup() {
  inputString.reserve(100);
  pinMode(btn1Pin, INPUT_PULLUP);
  pinMode(btn2Pin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledOrange, OUTPUT); // initialize the digital pin as an output.
  pinMode(ledGreen, OUTPUT); // initialize the digital pin as an output.
  pinMode(ledRed, OUTPUT); // initialize the digital pin as an output.
  pinMode(ledBlue, OUTPUT); // initialize the digital pin as an output.
  pinMode(relayPin, OUTPUT); // initialize the digital pin as an output.
  Serial.begin(9600); // Serial port to the computer
  Serial2.begin(9600); // Serial port to GSM Module
  blink(ledOrange, ledOrangeState);
  blink(ledGreen, ledGreenState);
  blink(ledRed, ledRedState);
  blink(ledBlue, ledBlueState);
  startupAlarm();
  delay(10000);
  Serial2.println("ATE0"); //Switch off ECHO
  delay(100);
  Serial2.println("AT+CNMI=1,2,0,0,0");// Notify message directly
  delay(50);
  blink(ledGreen, ledGreenState);
  blink(ledRed, ledRedState);
  blink(ledRed, ledBlueState);
  beep();
  twoposControl();
}

/* Loop Function
   This function runs over and over again as long as the board has power
   1.The GSM Modem is first Checked for proper connectivity
   1.The Button is monitored for forced message sending and testing
   3.Then the Temperature if read from LM35 and checked
   4.Any serialport Event is monitored
   5.To indicate Board is running properly the onBoard Orange Led is made to Blink
*/
void loop() {
  powerOn();
  checkComm();
  checkStatus();
  checkBtn1();
  checkBtn2();
  checkLM35();
  if(tempMode == AUTO)
    tempAutomaticControl();
  else
    tempManualControl();
  checkSHT75();
  serialEvent2();
}

/* Blink onBoard Orange LED to indicate Power ON */
void powerOn(){
  if (ledOrangeBlink.check() == 1)
    blink(ledOrange, ledOrangeState);
}

/* Buzzer Beep Sound */
void beep(){
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
  delay(100);
}

/* StartUp Sound Function */
void startupAlarm(){
  for(int i=0;i<4;i++){
    beep();
  }
}

/* Alarm Sound Function */
void soundAlarm(){
  for(int i=0;i<6;i++){
    beep();
  }
}

/* Button1 Function
   This checks whether the Button state is changed/updated and
   gives whether the button is rising or falling
   Falling: Button is pressed - Sends Message of Temperature
   Rising: Button is released
*/
void checkBtn1(){
   if (btn1.update()) {
    if (btn1.fallingEdge()) {
       Serial.println("btn1 fall");
       sendMessage("Temperature: ", tempC);
     }
     if (btn1.risingEdge()) {
       Serial.println("btn1 rise");     
     }
   }
}

/* Button2 Function
   This checks whether the Button state is changed/updated and
   gives whether the button is rising or falling
   Falling: Button is pressed - Sends Message of Humidity
   Rising: Button is released
*/
void checkBtn2(){
   if (btn2.update()) {
    if (btn2.fallingEdge()) {
       Serial.println("btn2 fall");
       sendMessage("Humidity: ", humidityRH);
     }
     if (btn1.risingEdge()) {
       Serial.println("btn2 rise");     
     }
   }
}

/* Blinking Function
   This Toggles the state of the LED
*/
void blink(const int led,int &state){
  if (state == LOW)
      state = HIGH;
    else
      state = LOW;
    digitalWrite(led, state);
}

void checkComm(){
  if(commTimer.check() == 1){
    if(!Serial2.available()){
      commError = true;
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledRed, HIGH);
      Serial.println("commError");
    }
    else{
      commError = false;
      digitalWrite(ledGreen, HIGH);
      digitalWrite(ledRed, LOW);
      Serial.println("comm Working");
    }
  }
}

/* SerialPort Monitoring Function
   This checks any incoming data at serialport and then splits the String based on 
   the LineFeed Character '\n' and sends it to the parser function
   [GSM Modem SIM300 sends "\r\n" Carraige Return(CR) and LineFeed(LF) in every response]
*/

void serialEvent2(){
  while (Serial2.available()) {
    // get the new byte:
    char inChar = (char)Serial2.read();
    inputString += inChar;  // add it to the inputString:
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
       parseString();
    } 
  }
}


/** GSM Related Functions **/
/* Network Connection Function
   This checks whether the SIM in GSM Module SIM300 is connected and 
   whether network is connected based on which blue LED is turned ON or OFF
*/
void checkStatus(){
  if(statusTimer.check() == 1){
    checkSim();
    delay(50);
    checkNet();
    delay(50);
    if(simReady){
      if(netReady)
        digitalWrite(ledBlue, HIGH);
    }
    else{
      digitalWrite(ledBlue, LOW);
    }
  }
}

void checkSim(){
   Serial2.println("AT+CSMINS?");
   delay(20);
}

void checkNet(){
   Serial2.println("AT+CREG?");
   delay(20);
}

void sendMessage(String text, float value = 0.0){
  Serial2.println("AT+CMGF=1");
  delay(50);
  Serial2.println("AT+CMGS=\""+mobileNo+"\"");
  delay(50);
  Serial2.print(text);
  if(value != 0.0){
    sprintf(charBuf,"%f", value);
    Serial2.print(charBuf);
  } 
  Serial2.print("\r\n\x1A"); //CR+LF+EOF
  delay(100);
}

/* Parser Function
   This checks any incoming String from serialport for necessary monitoring 
   the communication from GSM Module SIM300
*/
void parseString(){
  Serial.println(inputString);
  if(contains("+CREG: 0,0")){
    Serial.println("No Network Connection");
    netReady = false;
  }
  else if(contains("+CREG: 0,1") || contains("+CREG: 0,2") || contains("+CREG: 0,3")){
    Serial.println("Net Ready");
    netReady = true;
  }
  else if(contains("+CSMINS: 0,1")){//inputString.substring(0) == ){ //+CPIN: READY RDY +SCKS:0
    Serial.println("SIM READY");
    simReady = true;
  }
  else if(contains("+CSQ:")){ //RF Signal Strength
    Serial.println(inputString); // Turn Green Led ON
  }
  else if(contains("OK")){
    Serial.println(inputString); // Turn Green Led ON
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
  }
  else if(contains("Error")){
    Serial.println(inputString); // Turn Red Led ON
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
    delay(1000);
    if(contains("+CMS ERROR: 517")){ //SIM Wait
      Serial.println("SIM NOT READY");
      simReady = false;
    }
    else if(contains("+CMS ERROR: 515")){ //Busy
      Serial.println("Please Wait");
      simReady = false;
    }
  }
  else if(contains("TEMP")){ //The String contains TEMP
    Serial.println("TEMP Received");
    digitalWrite(ledGreen, HIGH);
    delay(500); // 515 error busy please wait
    sendMessage("Temperature: ", tempC);
  }
  else if(contains("HUM")){ //The String contains HUM
    Serial.println("HUM Received");
    digitalWrite(ledGreen, HIGH);
    delay(500); // 515 error busy please wait
    sendMessage("Humidity: ", humidityRH);
  }
  else if(contains("MAN")){
    Serial.println("MAN Received");
    digitalWrite(ledGreen, HIGH);
    delay(500);
    if(tempMode == MAN){
      sendMessage("Already in Manual Mode");
    }
    else{
    sendMessage("Manual Mode is Set");
    tempMode = MAN;
    }
  }
  else if(contains("AUTO")){
    Serial.println("AUTO Received");
    digitalWrite(ledGreen, HIGH);
    delay(500);
    if(tempMode == AUTO){
      sendMessage("Already in Automatic Mode");
    }
    else{
    sendMessage("Automatic Mode is Set");
    tempMode = AUTO;
    running = true;
    }
  }
  else if(contains("MOB")){
    Serial.println("MOB Received");
    digitalWrite(ledGreen, HIGH);
    char newChar[9];
    for(int i=0;i<10;i++)
      newChar[i] = inputString[i+4];
    mobileNo = newChar;
    Serial.println("New Mobile no:"+mobileNo);
    delay(500); // 515 error busy please wait
    sendMessage("New Mobile Number: "+mobileNo);
  }
  else if(contains("ON")){
    Serial.println("ON Received");
    if(tempMode == MAN){
    running = true;
    delay(500); // 515 error busy please wait
    sendMessage("Bulb is on");
    }
    else{
      sendMessage("Please Change to manual mode");
    }
  }
  else if(contains("OFF")){
    Serial.println("OFF Received");
    if(tempMode == MAN){
    running = false;
    delay(500); // 515 error busy please wait
    sendMessage("Bulb is off");
    }
    else{
      sendMessage("Please Change to manual mode");
    }
  }
  else if(contains("SET")){
    Serial.println("SET Received");
    char newChar[1];
    newChar[0] = inputString[4];
    newChar[1] = inputString[5];
    tempSetPoint = atof(newChar);// Converting tempSetPoint from message to float
    Serial.println(tempSetPoint);
    delay(500); // 515 error busy please wait
    sendMessage("tempSetPoint: ", tempSetPoint);
  }
  inputString = ""; // clear the string:
}

/* This checks if a search string is present in the  inputString */
int contains(String search) {
    int max = inputString.length() - search.length();
    int lgsearch = search.length();
    for (int i = 0; i <= max; i++) {
        if (inputString.substring(i, i + lgsearch) == search) return true;
    }
 return false;
}

/** Monitor and Control Related Functions **/
/* Temperature Monitoring Function
   This checks the temperature every 2second
*/
void checkLM35(){
   if (tempReadTimer.check() == 1){ // check if the metro has passed it's interval
       readLM35();
       Serial.print("Temperature: ");
       Serial.print(readValue);
       Serial.print(" - ");
       Serial.print(tempC);
       Serial.println("c");
   }
}

/* Temperature Reading Function 
   Reads 8 samples from the ADC pin A4/18 and then takes average
   Voltage is calculated by multiplying by 3.3v reference voltage and dividing by 1024 10-bit ADC value
   Temperature is calculated by multiplying voltage with 100 as LM35 has 10mV/C ex: 28c is 280mV or 0.28V
*/
void readLM35(){
  tempC = 0;
  voltage = 0;
  readValue = 0;
  for(int i = 0;i<=7;i++){ // gets 8 samples from adc
  readValue += analogRead(tempPin);
  delay(20);
  }
  readValue = readValue/8;
  voltage = (3.3 * readValue)/1024;
  tempC = voltage * 100;
}

/* Temperature Automatic Control Function
   Automatically controls the temperature based on tempSetPoint value
*/
void tempAutomaticControl(){
  if(tempC >= tempSetPoint){
         if(running){ // This is to prevent the system from sending messages and alarm continously
           Serial.print("Temperature >= ");
           Serial.println(tempSetPoint);
           running = false;
           relayState = LOW;
           sendMessage("Temperature: ", tempC);
           soundAlarm();
           twoposControl();
         }
       }
       else{ // If Temperature dips below tempSetPoint then return to normal operation
           if(!running){
             Serial.print("Temperature < ");
             Serial.println(tempSetPoint);
             running = true;
             relayState = HIGH;
             twoposControl();
             beep();
           }
       }
}

/* Temperature Manual Control Function
   Manually controls the temperature based on used Input ON/OFF
*/
void tempManualControl(){
  if(running)
    relayState = HIGH;
  else
    relayState = LOW;
  twoposControl();
}

/* Humidity Monitoring Function
   This checks the humidity every 2seconds and performs necessary controlling
*/
void checkSHT75(){
   if (humidityReadTimer.check() == 1){
     humidityRH = sht1x.readHumidity();
     Serial.print("Humidity: ");
     Serial.print(humidityRH);
     Serial.println("%");
   }
}

/* 2-Position Controller Mode 
   This Turns ON or OFF the Relay based on tempSetPoint */
void twoposControl(){
    digitalWrite(relayPin, relayState);
}

/** GPRS Related Functions **/
void initGPRS(){
  Serial2.println("AT+CGATT=1"); // - Attach to GPRS Service 
  Serial2.println("AT+CGDCONT=1,\"IP\",\"wap.cingular\""); //- Define PDP Context (cid, PDP type, APN) 
  Serial2.println("AT+CDNSCFG=\"208.67.222.222\",\"208.67.220.220\""); //- Configure Domain Name Server (primary DNS, secondary DNS) 
  Serial2.println("AT+CSTT=\"wap.cingular\",\"\",\"\"");//- Start Task & set APN, User ID, and password 
  Serial2.println("AT+CIICR");  //- Bring up wireless connection with GPRS - THIS MAY TAKE A WHILE
  Serial2.println("AT+CIFSR"); // - Get Local IP address 
}

void senddToServer(){
   Serial2.println("AT+CIPHEAD=1"); //- Tells module to add an 'IP Header' to receive data 
   Serial2.println("AT+CDNSORIP=1"); //- Indicates whether connection request will be IP address (0), or domain name (1) 
   Serial2.println("AT+CIPSTART=\"TCP\",\"www.google.com\",\"80\""); 
   //- Start up TCP connection (mode, IP address/name, port) CONNECT OK - Indicates you've connected to the server 
   //- IT MAY TAKE A WHILE FOR THIS TO BE RETURNED 
   Serial2.println("AT+CIPSEND"); //- Issue Send Command - wait for module to return'>'
   Serial2.print("GET / HTTP/1.1\r\n\x1A");  //- Send data - this example is an HTTP request for the default page
}


void checkGPRS(){
  Serial2.println("AT+CGATT?"); // +CGATT:1 Gprs is active, 0:de:active
  Serial2.println("AT+CIPSTATUS"); // check GPRS Status STATE: CONNECT OK or IP STATUS
}

void closeGPRS(){
  Serial2.println("AT+CGATT=0"); // close gprs
  Serial2.println("AT+CIPSHUT"); // close gprs
  Serial2.println("AT+CFUN=0,1"); // close gprs
  Serial2.println("AT+CPOWD=1"); // close gprs
}

void sendPacket(){
   Serial2.println("AT+CIPSTART=\"TCP\",\"211.136.42.12\",\"2020\""); // OK,CONENECT OK
   Serial2.println("AT+CIPSEND");
   Serial2.print("This is my data\r\n\x1A"); // SEND OK
 }
 
void getIP(){
  Serial2.print("AT+CIFSR");
}

void tcpServer(){
  Serial2.print("AT+CIPCSGP=1\"CMNET\""); // OK
  Serial2.print("AT+CLPORT=\"TCP\",\"2020\""); //OK
  Serial2.print("AT+CIPSERVER"); // SERVER OK
  Serial2.print("AT+CIPCCON=2"); // OK
  Serial2.print("AT+CIFSR"); // IP nl REMOTE IP: 10.112.255.207:2020 Then CIPSEND
}



