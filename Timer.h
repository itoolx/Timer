#ifndef TIMER_h
#define TIMER_h

#include <inttypes.h>

#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))

class Timer
{
  public:
	uint32_t getVersion();
    void startTimer(uint8_t pin, uint8_t frequency);
    void setTimerDuty(uint8_t pin, uint8_t duty);
    void stopTimer(uint8_t pin);
  private:
    byte getPeriod(int microseconds);
};

#endif