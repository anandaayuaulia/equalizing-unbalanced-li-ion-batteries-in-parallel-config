#include <Arduino.h>
#include <U8g2lib.h>

// Initialize sensor
// batt1 sensor
#define isensor1 A0
#define vsensor1 A4
#define isensor2 A1
// batt2 sensor
#define isensor3 A2
#define vsensor2 A5
#define isensor4 A3

const int in1 = 5;
const int in2 = 6;
const int in3 = 7;
const int in4 = 8; 
long number=1;
float numtest=1;
long sigsit1 = 0;
long sigsit2 = 0;


int stat1;
int stat2;

//current
float I1 = 0; 
float I2 = 0;

//ocv
float Ut1 = 0;
float Uocv1 = 0;
float sococv1 = 0;  
int sococvi1 = 0;
float Ut2 = 0;
float Uocv2 = 0;
float sococv2 = 0;  
int sococvi2 = 0;
//soc-ocv
float socv = 1.9;
 
int a=1;
//testing
int vtest1 = 19;
int vtest2 = 92;


//lcd
char tmp_soc1[8];
char tmp_soc2[8];
char tmp_I1[8];
char tmp_I2[8];
char tmp_Uocv1[8];
char tmp_Uocv2[8];
U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  dtostrf(sococvi1,3,0,tmp_soc1);
  dtostrf(sococvi2,3,0,tmp_soc2);

  dtostrf(I1,3,1,tmp_I1);
  dtostrf(I2,3,1,tmp_I2);

  dtostrf(Uocv1,3,1,tmp_Uocv1);
  dtostrf(Uocv2,3,1,tmp_Uocv2);
  
  u8g2.setFont(u8g_font_unifont);
  u8g2.drawStr( 0, 10, "Batt 1");
  u8g2.drawStr( 65, 10, "Batt 2");
  
  u8g2.drawStr( 0, 23, "SOC");
  u8g2.drawStr( 25, 23,tmp_soc1);
  u8g2.drawStr( 50, 23, "%");
  
  u8g2.drawStr( 65, 23, "SOC");
  u8g2.drawStr( 90, 23,tmp_soc2);
  u8g2.drawStr( 115, 23, "%");
  
  u8g2.drawStr( 0, 36, "V : ");
  u8g2.drawStr( 25, 36,tmp_Uocv1);
  
  u8g2.drawStr( 65, 36, "V : ");
  u8g2.drawStr( 90, 36,tmp_Uocv2);
  
  u8g2.drawStr( 0, 49, "I : ");
  u8g2.drawStr( 25, 49,tmp_I1);
  
  u8g2.drawStr( 65, 49, "I : ");
  u8g2.drawStr( 90, 49,tmp_I2);

  //if (stat1==1)
  if (sococvi1<90)
  {
    u8g2.drawStr( 0, 62, "dschrge");
  }
  //else if (stat1==2)
  else if (sococvi1>=90)
  {
    u8g2.drawStr( 0, 62, "dschrge");
  }

  //if (stat2==3)
  if (sococvi2<=20)
  {
    u8g2.drawStr( 65, 62,"chrge");
  }
  //else if (stat2==4)
  else if (sococvi2>20)
  {
    u8g2.drawStr( 65, 62,"dschrge");
  }

}

void setup() {
  // put your setup code here, to run once:
u8g2.begin();
u8g2.enableUTF8Print();
u8g2.setFont(u8g_font_unifont); 
u8g2.setColorIndex(1);

pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);
digitalWrite(in1, LOW);
digitalWrite(in2, LOW);
digitalWrite(in3, LOW);
digitalWrite(in4, LOW);
Serial.begin(9600);

}

void loop() {
  unsigned long bt1 = millis()/1000.0;
  //Initialize ocv batt1
  int count = 0;
  float battvoltsum = 0;
  while (count < 10000) {
    int battrawvolt = analogRead(vsensor1);
    double mv = map(battrawvolt,0,1023,0,2500) + 20;
    float battvoltage = mv/100;
    battvoltsum += battvoltage;
    count++;
  }
  Uocv1 = battvoltsum / count;
  Uocv1 = Uocv1 - 0.8; // charging - 1, discharging - 0.8
//testing
//Uocv1 = 12.25+(numtest/10);

  //soc-ocv-batt1
  float vocv1 = 13.5-Uocv1;
  float tempocv1 = vocv1/socv;
  float ocv1 = 1-tempocv1;
  sococv1 = ocv1*100;  
  sococvi1 = ocv1*100;

  //if( sococvi1 <= 20)
  if(vtest1 <= 20)
  {
    //initialize current sensor dc - battery
    count = 0;
    float currentsum = 0;
    while (count < 10000) {
     int rawcurrent = analogRead(isensor1);
     float voltage = (rawcurrent*(5.0/1023.0));
     float current = ((voltage - 2.5)/0.066);
     currentsum += current;
     count++;
    }
    float Itemp1 = currentsum / count;

    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    I1 = Itemp1;
    I1 = abs(I1);
    I1 = I1+1;
    stat1= 1;
  }

  //else if (sococvi1 <= 90 && sococvi1 > 20)
  else if(vtest1 > 20)
  {
    //initialize current sensor load - battery
    count = 0;
    float currentsum = 0;
    while (count < 10000) {
      int rawcurrent = analogRead(isensor2);
      float voltage = (rawcurrent*(5.0/1023.0));
      float current = ((voltage - 2.5)/0.066);
      currentsum += current;
      count++;
    }
    float Itemp2 = currentsum / count;

    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    I1 = Itemp2;
    I1 = abs(I1);
    stat1 = 2;
  }
  unsigned long at1 = millis()/1000.0;
  unsigned long sit1 = at1-bt1;

  unsigned long bt2 = millis()/1000.0;
  //Initialize ocv batt2
  count = 0;
  battvoltsum = 0;
  while (count < 10000) {
    int battrawvolt = analogRead(vsensor2);
    double mv = map(battrawvolt,0,1023,0,2500) + 20;
    float battvoltage = mv/100;
    battvoltsum += battvoltage;
    count++;
  }
  Uocv2 = battvoltsum / count;
  Uocv2 = Uocv2+0.1; //charging - 1, discharging - 0.8
//testing
//Uocv2 = 13.10-(numtest/10);

  //soc-ocv-batt2
  float vocv2 = 13.5-Uocv2;
  float tempocv2 = vocv2/socv;
  float ocv2 = 1-tempocv2;
  sococv2 = ocv2*100;  
  sococvi2 = ocv2*100;
  
  //if(sococvi2 < 90)
  if(vtest2 < 90)
  {
    //initialize current sensor dc - battery
    count = 0;
    float currentsum = 0;
    while (count < 10000) {
     int rawcurrent = analogRead(isensor3);
     float voltage = (rawcurrent*(5.0/1023.0));
     float current = ((voltage - 2.5)/0.066);
     currentsum += current;
     count++;
    }
    float Itemp1 = currentsum / count;

    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    I2 = Itemp1;
    I2 = abs(I2);
    stat2=3;
  }

  //else if (sococvi2 >= 90)
  else if (vtest2 >= 90)
  {
    //initialize current sensor load - battery
    count = 0;
    float currentsum = 0;
    while (count < 10000) {
      int rawcurrent = analogRead(isensor4);
      float voltage = (rawcurrent*(5.0/1023.0));
      float current = ((voltage - 2.5)/0.066);
      currentsum += current;
      count++;
    }
    float Itemp2 = currentsum / count;

    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    I2 = Itemp2;
    I2 = abs(I2);
    stat2 = 4;
  }
  unsigned long at2 = millis()/1000.0;
  unsigned long sit2 = at2-bt2;
//testing
//I1 = 1.56+(numtest/10);
//I1 = 0;
//Uocv1 = 0;
//testing
//I2 = 1.56+(numtest/10);
//I2 = 0;
//Uocv2= 0;
  //soc-cc
  //batt1
  sigsit1 = sigsit1 + sit1;
  float cc1 = I1*sit1;
  //batt2
  sigsit2 = sigsit2 + sit2;
  float cc2 = I2*sit2;
  
  if(a==1)
  {
    Serial.print("NO,V1,I1,cc1,sococv1,Time,TotalTime,Relay1,V2,I2,cc2,sococv2,TotalTime,Relay2\n"); 
    a=0;
  } 
  Serial.print("\n");
  Serial.print(number); 
  Serial.print(","); 
  Serial.print(Uocv1); 
  Serial.print(","); 
  Serial.print(I1); 
  Serial.print(",");
  Serial.print(cc1); 
  Serial.print(",");
  Serial.print(sococv1); 
  Serial.print(",");
  Serial.print(sit1); 
  Serial.print(","); 
  Serial.print(sigsit1); 
  Serial.print(","); 
  Serial.print(stat1);
  Serial.print(","); 
  Serial.print(Uocv2); 
  Serial.print(","); 
  Serial.print(I2); 
  Serial.print(",");
  Serial.print(cc2); 
  Serial.print(",");
  Serial.print(sococv2); 
  Serial.print(",");
  Serial.print(sit2); 
  Serial.print(","); 
  Serial.print(sigsit2); 
  Serial.print(","); 
  Serial.print(stat2);
  Serial.print(","); 

  // picture loop
  
  u8g2.firstPage();  
  do {
    draw();
  } while( u8g2.nextPage() );
  
  // rebuild the picture after some delay
  delay(50);
  
  number++;
//numtest++;
  
}
