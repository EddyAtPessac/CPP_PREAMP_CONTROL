#include <Arduino.h>
#include "LogicPotentiometer.h"


void InitialiseEncoder(int pin_a, int pin_b, int pin_push, 
                        void change_level(double), void select_next(void) );

void ICACHE_RAM_ATTR IsrEncoderPinA_Change(void);
void ICACHE_RAM_ATTR IsrEncoderPin_Push(void);

