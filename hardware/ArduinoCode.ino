#include <Servo.h>

Servo arm;
String incomingString = "";
const int trig1 = 6;
const int echo1 = 7;
const int trig2 = 8;
const int echo2 = 9;
const int threshold1 = 60, threshold2 = 40;
long avg1[50];
long avg2[50];
long FIRout1 = 0, FIRout2 = 0;
int counter1 = 0, counter2 = 0;
int prev1, prev2;
bool entered = false, exited = false;
unsigned long enterTime, exitTime;
int maxDist = 0; 
int covidCap = 0, occupancy = 0;

void setup() {
  pinMode(trig1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echo1, INPUT); // Sets the echoPin as an Input
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT); 

  arm.attach(10);
  arm.write(180);
  
  Serial.begin(19200); // Baudrate to 19200
  Serial.println("Enter capacity: ");
  covidCap = Serial.readString().toInt();
  Serial.print(covidCap);  
  for (int i = 0; i < 50; i++){
    avg1[i] = 0; 
    avg2[i] = 0;
  }
}

void loop() {
  
while (Serial.available() > 0) { 
  int distance1 = ultrasonicDistance(trig1, echo1);
  //filter
  if(counter1 % 20 == 0){
    avg1[49] = distance1;
    FIRout1 = distance1;
    for (int i = 1; i < 50; i++){
      avg1[i-1] = avg1[i];
      FIRout1 += (avg1[i-1]) / 50;
    }
    if(FIRout1 > 500){
      FIRout1 = 500;
    }
    if (FIRout1 > maxDist) {
      maxDist = FIRout1; 
    }
  }


  int distance2 = ultrasonicDistance(trig2, echo2);
  if(counter2 % 10 == 0){
    avg2[49] = distance2;
    FIRout2 = distance2;
    for (int i = 1; i < 50; i++){
      avg2[i-1] = avg2[i];
      FIRout2 += (avg2[i-1]) / 50;
    }
    if(FIRout2 > 500){
      FIRout2 = 500;
    }
    
    if (FIRout2 > maxDist) {
       maxDist = FIRout1; 
    } 
  }

  counter1++;
  counter2++;
  fallEdge1(FIRout1, prev1);
  fallEdge2(FIRout2, prev2);
  
  // after both beams crossed 
  if(entered && exited){
      if(enterTime > exitTime){
//        Serial.println("exited");
          occupancy -= 1;
        
      }
      else{
//        Serial.println("entered");
          occupancy += 1;
      }
      entered = false;
      exited = false;
    }
  Serial.print("Occupancy: ");
  Serial.println(occupancy);  
  prev1 = FIRout1;
  prev2 = FIRout2;
   
/*  Serial.print(distance1);
  Serial.print(" ");
  Serial.println(distance2);
  
    
    if(entered && exited){
      if(enterTime > exitTime){
//        Serial.println("exited");
      }
      else{
//        Serial.println("entered");
      }
      entered = false;
      exited = false;
    }
    
    if(millis() - entered > 50000){
      entered = false;
    }
    if(millis() - exited > 50000){
      exited = false;
    }
    
    Serial.print("Entered: ");
    Serial.println(entered);
    Serial.print("");
    Serial.print("Exited: ");
    Serial.println(exited);
    */
//  Serial.println("\nDistance 2: ");
//  int distance2 = ultrasonicDistance(trig2,echo2);
/*  Serial.print(FIRout1);
  Serial.print(" ");
  Serial.println(FIRout2); 
*/
}
  
}
void fallEdge1(long FIRout1, long prev1){
  if(prev1 - FIRout1 > threshold1){
//    Serial.println("SENSOR 1 HIT");
    enterTime = millis();
    entered = true;
  }
}
void fallEdge2(long FIRout2, long prev2){
  if(prev2 - FIRout2 > threshold2){
//    Serial.println("SENSOR 2 HIT");
    exitTime = millis();
    exited = true;
  }
}
int ultrasonicDistance (const int trigPin, const int echoPin){
  long duration;
  float distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  if (distance > 2000){
    return 2000;
  }
  else{
    return distance;
  }
}
