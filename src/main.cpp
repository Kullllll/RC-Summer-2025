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

const int motorSpeed = 200;     // Tốc độ động cơ
const int safeDistance = 20;    // Khoảng cách an toàn (cm)

// ========== Hàm điều khiển ==========

void moveForward(int speed) {
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);

  digitalWrite(IN3, 1);
  digitalWrite(IN4, 0);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed+40);
}

void moveBackward(int speed) {
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void turnLeft(int speed) {
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void turnRight(int speed) {
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, speed);
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

  Serial.begin(9600); // Debug nếu cần
}

void loop() {
  moveForward(motorSpeed);
  int distance = readUltrasonic();
  Serial.print("Khoang cach: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance <= safeDistance) {
    stopMotors();
    delay(300);

    // Ghi lại thời điểm bắt đầu quay trái
    unsigned long t_start = millis();

    // Quay trái cho đến khi không còn vật cản
    while (readUltrasonic() <= safeDistance) {
      turnLeft(motorSpeed);
      delay(50);
    }

    // Tính thời gian đã quay trái
    unsigned long t_end = millis();
    unsigned long t_turn = t_end - t_start;

    stopMotors();
    delay(200);

    // Đi thẳng một đoạn để né vật
    moveForward(motorSpeed);
    delay(2000);
    stopMotors();
    delay(200);

    // Quay phải gấp đôi thời gian đã quay trái
    turnRight(motorSpeed);
    delay(t_turn * 2);
    stopMotors();
    delay(200);

    // Đi thẳng thêm để vượt qua vùng vật cản
    moveForward(motorSpeed);
    delay(2000);
    stopMotors();
    delay(200);

    // Quay trái để quay lại trục cũ
    turnLeft(motorSpeed);
    delay(t_turn);
    stopMotors();
    delay(200);

    // Tiếp tục đi thẳng
    moveForward(motorSpeed);
  }
}


