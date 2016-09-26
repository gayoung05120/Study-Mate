e./#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <SPI.h> //와이파이쉴드
#include <WiFi.h> //와이파이쉴드

//사운드모듈 시작
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11); //RX 10번, TX11번
//사운드모듈 
i
#define MATRIX_EYES         0
#define MATRIX_MOUTH_LEFT   1
#define MATRIX_MOUTH_MIDDLE 2
#define MATRIX_MOUTH_RIGHT  3 

Adafruit_8x8matrix matrix[4] = {
  Adafruit_8x8matrix(), Adafruit_8x8matrix(),
  Adafruit_8x8matrix(), Adafruit_8x8matrix() }; //8x8 Martix

static const uint8_t matrixAddr[] = { 0x70, 0x71, 0x72, 0x73 };

static const uint8_t PROGMEM // Bitmaps are stored in program memory
  blinkImg[][8] = {    // 눈 애니메이션
  { B00111100,         // 눈을 떴을 때
    B01111110,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B01111110,
    B00111100 },
  { B00000000,
    B01111110,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B01111110,
    B00111100 },
  { B00000000,
    B00000000,
    B00111100,
    B11111111,
    B11111111,
    B11111111,
    B00111100,
    B00000000 },
  { B00000000,
    B00000000,
    B00000000,
    B00111100,
    B11111111,
    B01111110,
    B00011000,
    B00000000 },
  { B00000000,         // 눈을 감았을 때
    B00000000,
    B00000000,
    B00000000,
    B10000001,
    B01111110,
    B00000000,
    B00000000 } },
  mouthImg[][24] = {                 // 입 애니메이션
  { B00000000, B00000000, B00000000, // 입A
    B00000000, B00000000, B00000000,
    B01111111, B11111111, B11111110,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // 입B
    B00000000, B00000000, B00000000,
    B00111111, B11111111, B11111100,
    B00000111, B00000000, B11100000,
    B00000000, B11111111, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // 입C
    B00000000, B00000000, B00000000,
    B00111111, B11111111, B11111100,
    B00001000, B00000000, B00010000,
    B00000110, B00000000, B01100000,
    B00000001, B11000011, B10000000,
    B00000000, B00111100, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, // 입D
    B00000000, B00000000, B00000000,
    B00111111, B11111111, B11111100,
    B00100000, B00000000, B00000100,
    B00010000, B00000000, B00001000,
    B00001100, B00000000, B00110000,
    B00000011, B10000001, B11000000,
    B00000000, B01111110, B00000000 },
  { B00000000, B00000000, B00000000, // 입E
    B00000000, B00111100, B00000000,
    B00011111, B11000011, B11111000,
    B00000011, B10000001, B11000000,
    B00000000, B01111110, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00111100, B00000000, // 입F
    B00000000, B11000011, B00000000,
    B00001111, B00000000, B11110000,
    B00000001, B00000000, B10000000,
    B00000000, B11000011, B00000000,
    B00000000, B00111100, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 } };

uint8_t
  blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 }, 
  blinkCountdown = 100,  
  gazeCountdown  =  75, 
  gazeFrames     =  50, 
  mouthPos       =   0, 
  mouthCountdown =  10;
int8_t
  eyeX = 3, eyeY = 3,   
  newX = 3, newY = 3,   
  dX   = 0, dY   = 0;  

  
char ssid[] = "cutegayoung";      // network SSID (name)
char pass[] = "19930512";   // network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS; //기본예제 건드리지말것

int ledPin = 13; //인체감지센서(LED 연결핀)
int inputPin = 2; //인체감지센서(센서 시그널핀)
int val = 0; // Signal 입력값
int pirState = LOW;

int trig = 8;
int echo = 9;

int RED_LED = 45;
int YELLOW_LED = 47;
int GREEN_LED = 49;

WiFiServer server(80); //기본예제 건드리지말것

void setup() {
  randomSeed(analogRead(A0));
 
  for(uint8_t i=0; i<4; i++) {
    matrix[i].begin(matrixAddr[i]);
  }

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  BTSerial.begin(9600); //소리재생 통신 : 시리얼(TTL), 기본 9600전송속도(변경가능)
  //재생 : MP3, WAV, WMA 파일 지원

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
//  printWifiStatus();
}


void loop() {

   //인체감지 센서 시작
  val = digitalRead(inputPin); // 센서값 읽기
  if (val == HIGH) { // 인체감지시
    digitalWrite(ledPin, HIGH); // LED ON
    if (pirState == LOW) {
      // 시리얼모니터에 메시지 출력
      Serial.println("Motion detected!");
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW); // LED OFF
    if (pirState == HIGH) {
      // 시리얼모니터에 메시지 출력
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }
  //인체감지 센서 끝
  
  //초음파센서 시작
    digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  int distance = pulseIn(echo, HIGH) * 17 / 1000;
  Serial.print(distance);
  Serial.println("cm");
  delay(100);
  if (distance > 20)                                                     
  {
    digitalWrite(GREEN_LED, HIGH);                                     
    digitalWrite(YELLOW_LED, LOW);                                    
    digitalWrite(RED_LED, LOW);                                          
  }
  if (distance > 10 & distance <= 20)                          
  {
    digitalWrite(GREEN_LED, LOW);                                      
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  }
  if (distance > 0 & distance <= 10)
  {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }
  //초음파센서 끝

  matrix[MATRIX_EYES].clear();
  matrix[MATRIX_EYES].drawBitmap(0, 0,
    blinkImg[
      (blinkCountdown < sizeof(blinkIndex)) ? 
      blinkIndex[blinkCountdown] :            
      0                                      
    ], 8, 8, LED_ON);

  if(--blinkCountdown == 0) blinkCountdown = random(5, 180);

  if(--gazeCountdown <= gazeFrames) {
   
    matrix[MATRIX_EYES].fillRect(
      newX - (dX * gazeCountdown / gazeFrames),
      newY - (dY * gazeCountdown / gazeFrames),
      2, 2, LED_OFF);
      
    if(gazeCountdown == 0) {   
      eyeX = newX; eyeY = newY;
       
      do { 
        newX = random(7); newY = random(7);
        dX   = newX - 3;  dY   = newY - 3;
      } while((dX * dX + dY * dY) >= 10);      
      dX            = newX - eyeX;             
      dY            = newY - eyeY;             
      gazeFrames    = random(3, 15);          
      gazeCountdown = random(gazeFrames, 120); 
    }
    
  } else {
    matrix[MATRIX_EYES].fillRect(eyeX, eyeY, 2, 2, LED_OFF);
  }

 
  for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();

  delay(20); 
}


void drawMouth(const uint8_t *img) {
  for(uint8_t i=0; i<3; i++) {
    matrix[MATRIX_MOUTH_LEFT + i].clear();
    matrix[MATRIX_MOUTH_LEFT + i].drawBitmap(i * -8, 0, img, 24, 8, LED_ON);
  }

 

  //초음파센서 시작
    float duration, distance;
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
 
  // echoPin 이 HIGH를 유지한 시간을 저장 한다.
  duration = pulseIn(echo, HIGH);
  // HIGH 였을 때 시간(초음파가 보냈다가 다시 들어온 시간)을 가지고 거리를 계산 한다.
  // 340은 초당 초음파(소리)의 속도, 10000은 밀리세컨드를 세컨드로, 왕복거리이므로 2로 나눠준다.
  distance = ((float)(340 * duration) / 10000) / 2;
 
  //시리얼모니터에 Echo가 HIGH인 시간 및 거리를 표시해준다.
  Serial.print("Duration:");
  Serial.print(duration);
  Serial.print("\nDIstance:");
  Serial.print(distance);
  Serial.println("cm\n");
  delay(500);
  //초음파센서 끝

  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
} 

