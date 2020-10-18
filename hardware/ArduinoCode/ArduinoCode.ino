#include <hcsr04.h>


#include <Servo.h>
#define STEP 2
Servo arm;
String incomingString = "";
const int trig1 = 6;
const int echo1 = 7;
const int trig2 = 8;
const int echo2 = 9;

const int threshold1 = 40, threshold2 = 40;
long avg1[50];
long avg2[50];
long FIRout1 = 0, FIRout2 = 0;
int counter = 0, counter2 = 0;
int prev1, prev2;
int distance1, distance2;
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
  for (int i = 0; i < 50; i++) {
    avg1[i] = 0;
    avg2[i] = 0;
  }
}

void loop() {

  while (Serial.available() > 0) {
    if (!entered) {
      distance1 = ultrasonicDistance(trig1, echo1);
    }
    if (!exited) {
      distance2 = ultrasonicDistance(trig2, echo2);
    }
    //filter
    if (counter % STEP == 0) {
      avg1[49] = distance1;
      avg2[49] = distance2;

      FIRout1 = distance1;
      FIRout2 = distance2;
      for (int i = 1; i < 50; i++) {
        avg1[i - 1] = avg1[i];
        FIRout1 += (avg1[i - 1]) / 50;

        avg2[i - 1] = avg2[i];
        FIRout2 += (avg2[i - 1]) / 50;
      }
      if (FIRout1 > 400) {
        FIRout1 = 400;
      }
      if (FIRout1 > maxDist) {
        maxDist = FIRout1;
      }

      if (FIRout2 > 500) {
        FIRout2 = 500;
      }

      if (FIRout2 > maxDist) {
        maxDist = FIRout1;
      }

      //calculate falling edge
      fallEdge1(FIRout1, prev1);
      fallEdge2(FIRout2, prev2);
    }

    counter++;

    // after both beams crossed
    if (entered && exited) {
      if (enterTime > exitTime) {
        Serial.println("exited");
        occupancy -= 1;

      }
      else {
        Serial.println("entered");
        occupancy += 1;
      }
      entered = false;
      exited = false;
      enterTime = millis();
      exitTime = millis();
    }

    prev1 = FIRout1;
    prev2 = FIRout2;

    if (millis() - enterTime > 20000) {
      entered = false;
    }
    if (millis() - exitTime > 20000) {
      exited = false;
    }

  }

}



void fallEdge1(long FIRout1, long prev1) {
  if (prev1 - FIRout1 > threshold1) {
    Serial.println("SENSOR 1 HIT");
    enterTime = millis();
    entered = true;
  }
}

void fallEdge2(long FIRout2, long prev2) {
  if (prev2 - FIRout2 > threshold2) {
    Serial.println("SENSOR 2 HIT");
    exitTime = millis();
    exited = true;
  }
}

int ultrasonicDistance (const int trigPin, const int echoPin) {
  long duration;
  float distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  if (distance > 2000) {
    return 2000;
  }
  else {
    return distance;
  }
}
