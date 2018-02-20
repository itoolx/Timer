#include <Timer.h>
#include <Arduino.h>

byte Timer::getPeriod(int microseconds) {
	long cycles = (F_CPU * microseconds) / 2000000;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
	byte clockSelectBits = 0;
	if(cycles < RESOLUTION)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
	else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11);              // prescale by /8
	else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
	else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12);              // prescale by /256
	else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
	else cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum
	return clockSelectBits;
}

uint32_t Timer::getVersion() {
#if defined (__AVR_ATmega8__)
  return 0x01;
#elif defined(__AVR_ATmega48__)
  return 0x02;
#elif defined(__AVR_ATmega88__)
  return 0x03;
#elif defined(__AVR_ATmega168__)
  return 0x04;
#elif defined(__AVR_ATmega328P__)
  return 0x05;
#elif defined(__AVR_ATmega1280__)
  return 0x06;
#elif defined(__AVR_ATmega2560__)
  return 0x07;
#else
  return 0xFFFFFFFF;
#endif
}

/**
 * 初始化 Timer, 通过对 TCCRnA，TCCRnB 设置波形输出率， 以及 OCR 设置占空比来控制输出内容, 详细查看http://playground.arduino.cc/Main/TimerPWMCheatsheet
 * TOP is defined as 0xFF when WGM2[2:0] = 0x3, and OCR2A when WGM2[2:0] = 0x7
 * @pin 输出pin角
 * @frequency 取值范围为 1~7
 */
void Timer::startTimer(uint8_t pin, uint8_t frequency) {
    // We need to make sure the PWM output is enabled for those pins
    // that support it, as we turn it off when digitally reading or
    // writing with them.  Also, make sure the pin is in output mode
    // for consistenty with Wiring, which doesn't require a pinMode
    // call for the analog output pins.
	// To summarize the register settings, setting the waveform generation mode bits WGM to 011 selects fast PWM
	//
	// TCCRnA & TCCRnB 同时设置频率
	// OCR 设置占空比
    pinMode(pin, OUTPUT);
	switch(digitalPinToTimer(pin)) {
		// XXX fix needed for atmega8
		#if defined(TCCR0) && defined(COM00) && !defined(__AVR_ATmega8__)
		case TIMER0A:
			// connect pwm to pin on timer 0
			sbi(TCCR0, COM00);
			TCCR0 |= _BV(WGM00) | _BV(WGM01);
//			TCCR0 = frequency & 0x5 | _BV(WGM02);
			TCCR0 = frequency & 0x5;
			OCR0 = 0;
			break;
		#endif

		#if defined(TCCR0A) && defined(COM0A1)
		case TIMER0A:
			// connect pwm to pin on timer 0, channel A
			sbi(TCCR0A, COM0A1);// fast PWM, turn on oc0a
			TCCR0A |= _BV(WGM00) | _BV(WGM01);
//			TCCR0B = frequency & 0x5 | _BV(WGM02);
			TCCR0B = frequency & 0x5;
			OCR0A = 0;
			break;
		#endif

		#if defined(TCCR0A) && defined(COM0B1)
		case TIMER0B:
			// connect pwm to pin on timer 0, channel B
			sbi(TCCR0A, COM0B1);// fast PWM, turn on oc0b
			TCCR0A |= _BV(WGM00) | _BV(WGM01);
//			TCCR0B = frequency & 0x5 | _BV(WGM02);
			TCCR0B = frequency & 0x5;
			OCR0B = 0;
			break;
		#endif

		#if defined(TCCR1A) && defined(COM1A1)
		case TIMER1A:
			// connect pwm to pin on timer 1, channel A
			sbi(TCCR1A, COM1A1);// fast PWM, turn on oc1a
			TCCR1A |= _BV(WGM10) | _BV(WGM10);
//			TCCR1B = (frequency & 0x5) | _BV(WGM12) | _BV(WGM13);
			TCCR1B = (frequency & 0x5);
			OCR1A = 0;
			break;
		#endif

		#if defined(TCCR1A) && defined(COM1B1)
		case TIMER1B:
			// connect pwm to pin on timer 1, channel B
			sbi(TCCR1A, COM1B1);// fast PWM, turn on oc1b
			TCCR1A |= _BV(WGM10) | _BV(WGM10);
//			TCCR1B = (frequency & 0x5) | _BV(WGM12) | _BV(WGM13);
			TCCR1B = (frequency & 0x5);
			OCR1B = 0;
			break;
		#endif

		#if defined(TCCR1A) && defined(COM1C1)
		case TIMER1C:
			// connect pwm to pin on timer 1, channel B
			sbi(TCCR1A, COM1C1);// fast PWM, turn on oc1c
			TCCR1A |= _BV(WGM10) | _BV(WGM10);
//			TCCR1B = (frequency & 0x5) | _BV(WGM12) | _BV(WGM13);
			TCCR1B = (frequency & 0x5);
			OCR1C = 0;
			break;
		#endif

		#if defined(TCCR2) && defined(COM21)
		case TIMER2:
			// connect pwm to pin on timer 2
			sbi(TCCR2, COM21);
			break;
		#endif

		#if defined(TCCR2A) && defined(COM2A1)
		case TIMER2A:
			// connect pwm to pin on timer 2, channel A
			sbi(TCCR2A, COM2A1);
			TCCR2A |= _BV(WGM20) | _BV(WGM21);// fast PWM, turn on oc2a
//			TCCR2B = frequency & 0x7 | _BV(WGM22);
			TCCR2B = frequency & 0x7;
			OCR2A = 0;
			break;
		#endif

		#if defined(TCCR2A) && defined(COM2B1)
		case TIMER2B:
			// connect pwm to pin on timer 2, channel B
			sbi(TCCR2A, COM2B1);
			TCCR2A |= _BV(WGM20) | _BV(WGM21);// fast PWM, turn on oc2b
//			TCCR2B = frequency & 0x7 | _BV(WGM22);
			TCCR2B = frequency & 0x7;
			OCR2B = 0;
			break;
		#endif

		#if defined(TCCR3A) && defined(COM3A1)
		case TIMER3A:
			// connect pwm to pin on timer 3, channel A
			sbi(TCCR3A, COM3A1);
			sbi(TCCR3B, _BV(WGM13));
			TCCR3B = (frequency & 0x7);
			OCR3A = 0;
			break;
		#endif

		#if defined(TCCR3A) && defined(COM3B1)
		case TIMER3B:
			// connect pwm to pin on timer 3, channel B
			sbi(TCCR3A, COM3B1);
			sbi(TCCR3B, _BV(WGM13);
			TCCR3B = (frequency & 0x7);
			OCR3B = 0; // set pwm duty
			break;
		#endif

		#if defined(TCCR3A) && defined(COM3C1)
		case TIMER3C:
			// connect pwm to pin on timer 3, channel C
			sbi(TCCR3A, COM3C1);
			sbi(TCCR3B, _BV(WGM13));
			TCCR3B = (frequency & 0x7);
			OCR3C = 0;
			break;
		#endif

		#if defined(TCCR4A)
		case TIMER4A:
			//connect pwm to pin on timer 4, channel A
			sbi(TCCR4A, COM4A1);
			#if defined(COM4A0)        // only used on 32U4
			cbi(TCCR4A, COM4A0);
			#endif
			TCCR4A |= _BV(WGM10);// fast PWM, turn on oc3c
			TCCR4B = (frequency & 0x7) | _BV(WGM12);
			OCR4A = 0;
			break;
		#endif

		#if defined(TCCR4A) && defined(COM4B1)
		case TIMER4B:
			// connect pwm to pin on timer 4, channel B
			sbi(TCCR4A, COM4B1);
			TCCR4A |= _BV(WGM10);// fast PWM, turn on oc3c
			TCCR4B = (frequency & 0x7) | _BV(WGM12);
			OCR4B = 0;
			break;
		#endif

		#if defined(TCCR4A) && defined(COM4C1)
		case TIMER4C:
			// connect pwm to pin on timer 4, channel C
			sbi(TCCR4A, COM4C1);
			TCCR4A |= _BV(WGM10);// fast PWM, turn on oc3c
			TCCR4B = (frequency & 0x7) | _BV(WGM12);
			OCR4C = 0;
			break;
		#endif

		#if defined(TCCR4C) && defined(COM4D1)
		case TIMER4D:
			// connect pwm to pin on timer 4, channel D
			sbi(TCCR4C, COM4D1);
			#if defined(COM4D0)        // only used on 32U4
			cbi(TCCR4C, COM4D0);
			#endif
			TCCR4A |= _BV(WGM10);// fast PWM, turn on oc3c
			TCCR4B = (frequency & 0x7) | _BV(WGM12);
			OCR4D = 0;
			break;
		#endif


		#if defined(TCCR5A) && defined(COM5A1)
		case TIMER5A:
			// connect pwm to pin on timer 5, channel A
			sbi(TCCR5A, COM5A1);
			TCCR5A |= _BV(WGM10);// fast PWM, turn on oc3c
			TCCR5B = (frequency & 0x7) | _BV(WGM12);
			OCR5A = 0;
			break;
		#endif

		#if defined(TCCR5A) && defined(COM5B1)
		case TIMER5B:
			// connect pwm to pin on timer 5, channel B
			sbi(TCCR5A, COM5B1);
			TCCR5A |= _BV(WGM10);// fast PWM, turn on oc3c
			TCCR5B = (frequency & 0x7) | _BV(WGM12);
			OCR5B = 0;
			break;
		#endif

		#if defined(TCCR5A) && defined(COM5C1)
		case TIMER5C:
			// connect pwm to pin on timer 5, channel C
			sbi(TCCR5A, COM5C1);
			TCCR5A |= _BV(WGM10);// fast PWM, turn on oc3c
			TCCR5B = (frequency & 0x7) | _BV(WGM12);
			OCR5C = 0;
			break;
		#endif

		case NOT_ON_TIMER:
		default:
			digitalWrite(pin, HIGH);
	}
}
/**
 * 设置占空比(0~256)
 */
void Timer::setTimerDuty(uint8_t pin, uint8_t duty) {
	// 保留7位
	duty &= 0x7F;
	switch(digitalPinToTimer(pin)) {
		// XXX fix needed for atmega8
		#if defined(TCCR0) && defined(COM00) && !defined(__AVR_ATmega8__)
		case TIMER0A:
			OCR0 = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR0A) && defined(COM0A1)
		case TIMER0A:
			OCR0A = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR0A) && defined(COM0B1)
		case TIMER0B:
			OCR0B = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR1A) && defined(COM1A1)
		case TIMER1A:
			OCR1A = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR1A) && defined(COM1B1)
		case TIMER1B:
			OCR1B = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR1A) && defined(COM1C1)
		case TIMER1C:
			OCR1C = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR2) && defined(COM21)
		case TIMER2:
			OCR2 = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR2A) && defined(COM2A1)
		case TIMER2A:
			OCR2A = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR2A) && defined(COM2B1)
		case TIMER2B:
			OCR2B = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR3A) && defined(COM3A1)
		case TIMER3A:
			OCR3A = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR3A) && defined(COM3B1)
		case TIMER3B:
			OCR3B = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR3A) && defined(COM3C1)
		case TIMER3C:
			OCR3C = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR4A)
		case TIMER4A:
			OCR4A = duty;    // set pwm duty
			break;
		#endif

		#if defined(TCCR4A) && defined(COM4B1)
		case TIMER4B:
			OCR4B = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR4A) && defined(COM4C1)
		case TIMER4C:
			OCR4C = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR4C) && defined(COM4D1)
		case TIMER4D:
			OCR4D = duty;    // set pwm duty
			break;
		#endif


		#if defined(TCCR5A) && defined(COM5A1)
		case TIMER5A:
			OCR5A = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR5A) && defined(COM5B1)
		case TIMER5B:
			OCR5B = duty; // set pwm duty
			break;
		#endif

		#if defined(TCCR5A) && defined(COM5C1)
		case TIMER5C:
			OCR5C = duty; // set pwm duty
			break;
		#endif

		case NOT_ON_TIMER:
		default:
			if (duty < 128) {
				digitalWrite(pin, LOW);
			} else {
				digitalWrite(pin, HIGH);
			}
	}
}
void Timer::stopTimer(uint8_t pin) {
	switch(digitalPinToTimer(pin)) {
		// XXX fix needed for atmega8
		#if defined(TCCR0) && defined(COM00) && !defined(__AVR_ATmega8__)
		case TIMER0A:
			// disconnect pwm to pin on timer 0
			cbi(TCCR0, COM00);
			break;
		#endif

		#if defined(TCCR0A) && defined(COM0A1)
		case TIMER0A:
			// connect pwm to pin on timer 0, channel A
			cbi(TCCR0A, COM0A1);
			break;
		#endif

		#if defined(TCCR0A) && defined(COM0B1)
		case TIMER0B:
			// connect pwm to pin on timer 0, channel B
			cbi(TCCR0A, COM0B1);
			break;
		#endif

		#if defined(TCCR1A) && defined(COM1A1)
		case TIMER1A:
			// connect pwm to pin on timer 1, channel A
			cbi(TCCR1A, COM1A1);
			break;
		#endif

		#if defined(TCCR1A) && defined(COM1B1)
		case TIMER1B:
			// connect pwm to pin on timer 1, channel B
			cbi(TCCR1A, COM1B1);
			break;
		#endif

		#if defined(TCCR1A) && defined(COM1C1)
		case TIMER1C:
			// connect pwm to pin on timer 1, channel B
			cbi(TCCR1A, COM1C1);
			break;
		#endif

		#if defined(TCCR2) && defined(COM21)
		case TIMER2:
			// connect pwm to pin on timer 2
			cbi(TCCR2, COM21);
			break;
		#endif

		#if defined(TCCR2A) && defined(COM2A1)
		case TIMER2A:
			// connect pwm to pin on timer 2, channel A
			cbi(TCCR2A, COM2A1);
			break;
		#endif

		#if defined(TCCR2A) && defined(COM2B1)
		case TIMER2B:
			// connect pwm to pin on timer 2, channel B
			cbi(TCCR2A, COM2B1);
			break;
		#endif

		#if defined(TCCR3A) && defined(COM3A1)
		case TIMER3A:
			// connect pwm to pin on timer 3, channel A
			cbi(TCCR3A, COM3A1);
			break;
		#endif

		#if defined(TCCR3A) && defined(COM3B1)
		case TIMER3B:
			// connect pwm to pin on timer 3, channel B
			cbi(TCCR3A, COM3B1);
			break;
		#endif

		#if defined(TCCR3A) && defined(COM3C1)
		case TIMER3C:
			// connect pwm to pin on timer 3, channel C
			cbi(TCCR3A, COM3C1);
			break;
		#endif

		#if defined(TCCR4A)
		case TIMER4A:
			//connect pwm to pin on timer 4, channel A
			cbi(TCCR4A, COM4A1);
			break;
		#endif

		#if defined(TCCR4A) && defined(COM4B1)
		case TIMER4B:
			// connect pwm to pin on timer 4, channel B
			cbi(TCCR4A, COM4B1);
			break;
		#endif

		#if defined(TCCR4A) && defined(COM4C1)
		case TIMER4C:
			// connect pwm to pin on timer 4, channel C
			cbi(TCCR4A, COM4C1);
			break;
		#endif

		#if defined(TCCR4C) && defined(COM4D1)
		case TIMER4D:
			// connect pwm to pin on timer 4, channel D
			cbi(TCCR4C, COM4D1);
			break;
		#endif

		#if defined(TCCR5A) && defined(COM5A1)
		case TIMER5A:
			// connect pwm to pin on timer 5, channel A
			cbi(TCCR5A, COM5A1);
			break;
		#endif

		#if defined(TCCR5A) && defined(COM5B1)
		case TIMER5B:
			// connect pwm to pin on timer 5, channel B
			cbi(TCCR5A, COM5B1);
			break;
		#endif

		#if defined(TCCR5A) && defined(COM5C1)
		case TIMER5C:
			// connect pwm to pin on timer 5, channel C
			cbi(TCCR5A, COM5C1);
			break;
		#endif

		case NOT_ON_TIMER:
		default:
			digitalWrite(pin, LOW);
	}
}