
// Class LogicPotentiometer and his utilities functions 
#define COLOR_UINT 0  // Set df_RingColor as uint

#include <Arduino.h>
#include "PotRing.h"

#include <RemoteDebug.h>
extern RemoteDebug Debug;


#ifndef _LOGIC_POTENTIOMETER_
#define _LOGIC_POTENTIOMETER_
//#include <IRremote.h>


#define POT_PHYSIQUE_MAX_LEVEL  1024
#define PWM_FREQUENCY  40000
#define TIME_TO_DEFAULT 10000L // Time to return to Volume display
#define TIME_TO_SHUT_DOWN 15000L // Time to turn ring off
#define LOGIC_POTENTIOMETER_BASE_INCREMENT   ((4.0l * 1.0l) / POT_PHYSIQUE_MAX_LEVEL)

void ChangeSelectedPotLevel(double inc);
void SelectNextPotentiometer(void);
void CheckPotLoop(void);
void CheckPotentiometerIRData(uint32_t ir_code, bool is_repeat);
void ShutdownSelectedRing(void);

class LogicPotentiometer {
    public:
        LogicPotentiometer(int _out_pin, df_RingColor _color, String _name, PotRing *ringRef, double _level = 0.1);
        void ChangeLevel(double inc);
        void AttachIRCode(uint32_t up, uint32_t down);
        int  CheckRecevedIR(uint32_t ir_rx);
        void SelectMe(void);
        void ShutdownRing(void);
        
        uint32_t ir_up;
        uint32_t ir_down;
        String name;

    private:
        uint8_t potNum;
        uint8_t outPin;
        df_RingColor ledColor;
        double level;
        int16_t pwm;
        PotRing *ledRing;

};

#endif // _LOGIC_POTENTIOMETER_