#include <SoftwareSerial.h>
//蓝牙
int BT_TXD = 2;
int BT_RXD = 3;
SoftwareSerial BT(BT_TXD, BT_RXD);  //TXD, RXD

//寻迹
int MH_DP1 = 4;
int MH_DP2 = 5;
int MH_DP3 = 6;
int MH_DP4 = 7;

//轮子
int IN1 = 8;
int IN2 = 11;
int IN3 = 12;
int IN4 = 13;
int ENA = 10;
int ENB = 9;

//寻迹
int m1;
int m2;
int m3;
int m4;

bool mEnable = false;

const int mStrength = 128;
const int moveDelay = 60;  //操控持续时长

void setup() {
  Serial.begin(9600);
  BTSetup();
  MHSetup();
  wheelSetup();
}

bool isCross = false;
bool isLeft = false;
bool isRight = false;


//计时
long preDelay1 = 0;
const long delay1 = 1;

void loop() {
  long currentMillis = millis();
  m1 = digitalRead(MH_DP1);
  m2 = digitalRead(MH_DP2);
  m3 = digitalRead(MH_DP3);
  m4 = digitalRead(MH_DP4);

  if (isCross) {
    if (currentMillis - preDelay1 >= delay1) {
      autoMode();
      delay(delay1);
      reset();
      if (m1 == m2 && m2 == m3 && m3 == m4) {
        isCross = false;
        BT.write("C\r");
        return;
      }
      preDelay1 = currentMillis;
    }
  }

  if (isLeft || isRight) {
    if (currentMillis - preDelay1 >= delay1) {
      if (m1 == HIGH || m2 == HIGH || m3 == HIGH || m4 == HIGH) {
        if (isLeft) {
          isLeft = false;
          BT.write("L\r");
        } else if (isRight) {
          isRight = false;
          BT.write("R\r");
        }
        return;
      }
      rotateMode();
      delay(delay1);
      reset();
      preDelay1 = currentMillis;
    }
  }

  //寻迹
  if (mEnable) {
    if (currentMillis - preDelay1 >= delay1) {
      autoMode();
      delay(delay1);
      reset();
      preDelay1 = currentMillis;
    }
  }

  if (BT.available()) {
    char msg = BT.read();
    Serial.println(msg);
    processMsg(msg);
  }

  if (Serial.available()) {
    char msg = Serial.read();
    processMsg(msg);
    BT.write(msg);
  }

  //debug
  //forward(mStrength, mStrength);
  if (m1 == HIGH) {
    BT.write("1");
  }
  if (m2 == HIGH) {
    BT.write("2");
  }
  if (m3 == HIGH) {
    BT.write("3");
  }
  if (m4 == HIGH) {
    BT.write("4");
  }
}

void autoMode() {
  if (m1 == HIGH && m2 == HIGH && m3 == HIGH && m4 == HIGH) {
    reset();
  } else if (m2 == HIGH && m3 == HIGH) {
    forward(mStrength, mStrength);
  } else if (m1 == HIGH && m2 == HIGH) {
    forward(mStrength, mStrength * 0.9);
  } else if (m3 == HIGH && m4 == HIGH) {
    left(mStrength * 0.9, mStrength);
  } else if (m2 == HIGH) {
    forward(mStrength, mStrength * 0.7);
  } else if (m3 == HIGH) {
    forward(mStrength * 0.7, mStrength);
  } else if (m1 == HIGH) {
    right(mStrength, mStrength);
  } else if (m4 == HIGH) {
    left(mStrength, mStrength);
  }
}

void rotateMode() {
  if (isLeft) {
    left(mStrength, mStrength);
  } else if (isRight) {
    right(mStrength, mStrength);
  }
}

void processMsg(char msg) {
  //控制寻迹开关
  if (msg == 'x') {
    mEnable = true;
    return;
  } else if (msg == 'o') {
    mEnable = false;
    return;
  } else if (msg == 'c') {
    isCross = true;
  } else if (msg == 'l') {
    isLeft = true;
  } else if (msg == 'r') {
    isRight = true;
  }

  //控制
  if (msg == 'w') {
    forward(mStrength, mStrength);
  } else if (msg == 'a') {
    left(mStrength, mStrength);
  } else if (msg == 's') {
    back(mStrength, mStrength);
  } else if (msg == 'd') {
    right(mStrength, mStrength);
  }
  delay(moveDelay);
  reset();
}

void BTSetup() {
  BT.begin(9600);
}

void MHSetup() {
  pinMode(MH_DP1, INPUT);
  pinMode(MH_DP2, INPUT);
  pinMode(MH_DP3, INPUT);
  pinMode(MH_DP4, INPUT);
}

void wheelSetup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void BTWrite(SoftwareSerial BT, String msg) {
  for (int i = 0; i < msg.length(); i++) {
    BT.write(msg[i]);
  }
}

void right(int strengthA, int strengthB) {  //右，左
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, strengthA);
  analogWrite(ENB, strengthB);
}

void left(int strengthA, int strengthB) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, strengthA);
  analogWrite(ENB, strengthB);
}

void back(int strengthA, int strengthB) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, strengthA);
  analogWrite(ENB, strengthB);
}

void forward(int strengthA, int strengthB) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, strengthA);
  analogWrite(ENB, strengthB);
}

void reset() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
