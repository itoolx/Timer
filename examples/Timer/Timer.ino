#include <Timer.h>
#include <Arduino.h>

Timer timer = Timer();
const byte  ledPin1 = 3;     //pwm输出引脚
const byte  ledPin2 = 5;     //pwm输出引脚
const byte  ledPin3 = 6;     //pwm输出引脚
const byte  ledPin4 = 9;     //pwm输出引脚
const byte  ledPin5 = 10;     //pwm输出引脚
const byte  ledPin6 = 11;     //pwm输出引脚
const byte  STEP = 2;
const byte  MIN_VALUE = 0;
const byte  MAX_VALUE = 250;
byte mPWMValue = 0;
bool isUpper = true;
int freq = 1;

void setup() {
//	timer.startTimer(ledPin1, freq);
	timer.startTimer(ledPin2, freq);
	timer.startTimer(ledPin3, freq);
	timer.startTimer(ledPin4, freq);
  timer.startTimer(ledPin5, freq);
  timer.startTimer(ledPin6, freq);
	Serial.begin(9600);
  //Serial.println("the version: " + String(timer.getVersion())); 
}

void loop() {
	if (isUpper) {
		mPWMValue += STEP;
	} else {
		mPWMValue -= STEP;
	}
	if (mPWMValue >= MAX_VALUE) {
		isUpper = false;
		mPWMValue = MAX_VALUE;
	} else if (mPWMValue <= MIN_VALUE) {
		isUpper = true;
		mPWMValue = MIN_VALUE;
	}
  analogWrite(ledPin1, mPWMValue);
  //analogWrite(ledPin2, mPWMValue);
  //analogWrite(ledPin3, mPWMValue);
  //analogWrite(ledPin4, mPWMValue);
  //analogWrite(ledPin5, mPWMValue);
  //analogWrite(ledPin6, mPWMValue);
//	timer.setTimerDuty(ledPin1, mPWMValue);
	timer.setTimerDuty(ledPin2, mPWMValue);
	timer.setTimerDuty(ledPin3, mPWMValue);
	timer.setTimerDuty(ledPin4, mPWMValue);
  timer.setTimerDuty(ledPin5, mPWMValue);
  timer.setTimerDuty(ledPin6, mPWMValue);
	Serial.println(map(mPWMValue, 0, 255, 0, 5));    //使用map函数映射为 0~5v的电压信号
	delay(2000);
}

