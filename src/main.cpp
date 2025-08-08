#include <Arduino.h>

// Khai báo chân động cơ
const int ENA = 9;
const int IN1 = 4;
const int IN2 = 5;

const int ENB = 10;
const int IN3 = 6;
const int IN4 = 7;

const int trigPin = 12;
const int echoPin = 13;

// LED RGB
const int redPin = 3;
const int greenPin = 8;
const int bluePin = 11;   

const int motorSpeed = 150;     // Tốc độ động cơ
const int safeDistance = 30;    // Khoảng cách an toàn (cm)

// ========== Hàm điều khiển ==========

void moveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, speed+35);
  analogWrite(ENB, speed);
}

void moveBackward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, speed+35);
  analogWrite(ENB, speed);
}

void turnLeft(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, speed+35);
  analogWrite(ENB, speed);
}

void turnRight(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, speed+35);
  analogWrite(ENB, speed);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
int readUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

// Hàm điều khiển LED RGB
void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void blinkRedBlueDuringAvoid(unsigned long durationMs) {
  unsigned long startTime = millis();
  bool redOn = true;

  while (millis() - startTime < durationMs) {
    if (redOn) {
      setColor(255, 0, 0);
    } else {
      setColor(0, 0, 255);
    }
    redOn = !redOn;

    delay(100); // Tốc độ nháy (100ms)

    // Vừa nháy LED vừa để động cơ chạy
  }
}


void setup() {
  // Cấu hình các chân là OUTPUT
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(9600); // Debug nếu cần
}

void loop() {
  moveForward(motorSpeed);
  int distance = readUltrasonic();
  Serial.print("Khoang cach: ");
  Serial.print(distance);
  Serial.println(" cm");

  // === Hiệu ứng LED RGB theo khoảng cách === 
  if (distance >= safeDistance && distance <= 50) {
    // 30–50cm: màu xanh dương
    setColor(0, 0, 255);
  } 
  else if (distance > 50) {
    // >50cm: màu xanh lá
    setColor(0, 255, 0);
  } 
  else {
    // Nếu không đo được thì tắt đèn
    setColor(0, 0, 0);
  }

  //Nếu gặp vật cản
  if (distance <= safeDistance) {
    stopMotors();
    delay(300);

    //Quay trái 1 đoạn
    turnLeft(motorSpeed);
    blinkRedBlueDuringAvoid(400);
    stopMotors();
    delay(200);

    // Đi thẳng một đoạn để né vật
    moveForward(motorSpeed);
    blinkRedBlueDuringAvoid(1500);
    stopMotors();
    delay(200);

    // Quay phải gấp đôi thời gian đã quay trái
    turnRight(motorSpeed);
    blinkRedBlueDuringAvoid(800);
    stopMotors();
    delay(200);

    // Đi thẳng thêm để vượt qua vùng vật cản
    moveForward(motorSpeed);
    blinkRedBlueDuringAvoid(1500);
    stopMotors();
    delay(200);

    // Quay trái để quay lại trục cũ
    turnLeft(motorSpeed);
    blinkRedBlueDuringAvoid(400);
    stopMotors();
    delay(200);

    // Tiếp tục đi thẳng
    moveForward(motorSpeed);
  }
}


