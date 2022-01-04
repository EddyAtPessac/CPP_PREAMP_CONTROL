#include <Arduino.h>
#include <vector>
#include <Ticker.h>
#include "LogicPotentiometer.h"
#include "PinDefinition.h"
#include "PotRing.h"

using namespace std;

#define ENABLE_PRINT
#include "PrintUtil.h"

#include <RemoteDebug.h>

// Register the potentiometer instances 
std::vector<LogicPotentiometer *> listPot;
int selPotIndx = 0;
const int IR_REPEAT_TRIG = 10;   // Every IR_REPEAT_TRIG, we add the 
int ir_direction = 0;   // Last IR code direction  1: up, -1 down
double ir_base_increment = LOGIC_POTENTIOMETER_BASE_INCREMENT;
double ir_increment = ir_base_increment;
int ir_repeat_nb = 0;   // nb of repeat code receveid
const double fast_increment = ir_increment * 5;

//int pwmChan = 0;
const int pwmFreq = 20000;
const int pwmResol = 10;
//const double pwmMaxVal = 1024.0;
const double pwmMaxVal = 768.0;   // The CI max is 3.8V, not 5V

void ReturnToDefaultSelection(void);
Ticker PotTimer(ReturnToDefaultSelection, TIME_TO_DEFAULT, MILLIS); 
Ticker PotTimerLedOff(ShutdownSelectedRing, TIME_TO_SHUT_DOWN, MILLIS); 

// Tranlate from PotNum (1 to 4) to PWM chanel (0 to 15 available)
#define PotNum2PwmChanel(n) (2*n)

// Entry point from the encoder interrupt. 
// This function is registered at main level with InitialiseEncoder() function
void SelectNextPotentiometer(void) {
    selPotIndx ++;
    if (selPotIndx >= listPot.size()) {
        selPotIndx = 0;
    }

    toPrintStr =  "Pot " + listPot[selPotIndx]->name + " is selected\n";
    print_pool(toPrintStr);
    ChangeSelectedPotLevel(0.0);    // Change ring color with new selected 
}


// Entry point from the encoder interrupt. 
// This function is registered at main level with InitialiseEncoder() function
void ChangeSelectedPotLevel(double inc) {
    if (listPot.size()>0) {
        listPot[selPotIndx]->ChangeLevel(inc);
    }
}

// Change color of the selected potentiometer to black
void ShutdownSelectedRing() {
    toPrintStr =  "Shutdown ring is called \n";
    print_pool(toPrintStr);

    if (listPot.size()>0) {
        listPot[selPotIndx]->ShutdownRing();
    }
}

// Select the 1st instance of the potentiometer list 
// This function is called by Ticker PotTimer instance after no event elapsed time
void ReturnToDefaultSelection(void) {

    if (listPot.size()>0) {
        selPotIndx = 0;
        toPrintStr =  "Pot " + listPot[0]->name + " is selected\n";
        print_pool(toPrintStr);
    }
    ChangeSelectedPotLevel(0.0);    // Change ring color with new selected one
}



void CheckPotentiometerIRData(uint32_t ir_code, bool is_repeat) {
    ir_code &= 0xFFFF;  // Keeps the 32 lsb
    sprintf(printStr, "CheckPotentiometerIRData: Check for ir code  %04X - %d. \n", ir_code, is_repeat );
    print_pool(printStr);
    //Serial.printf(printStr);
    is_repeat = (ir_code == 0xFFFF);  // is_repeat flag doesnt work. We check if FFFF is received 
    if (is_repeat) {    // First, check if it is a repeat of last command
    // If we receive a repeat code, we consider that it is the selected potentiometer 
    // whitch have receive the initial code
        if ( ir_repeat_nb > 1 && ((ir_repeat_nb % IR_REPEAT_TRIG) == 0 ) ) {
            Serial.printf("Repeat up increment %f\n", ir_increment);
            ir_increment += fast_increment;  // Every IR_REPEAT_TRIG we grow up ir_increment
        }
        ir_repeat_nb ++;
        ChangeSelectedPotLevel(ir_increment * ir_direction);
    }
    else {
        for (auto pot : listPot) {  // Search in potentiometers instances list
            int dir = pot->CheckRecevedIR(ir_code);
            if ( dir != 0) {    // Code reconised for this potentiometer
                Serial.printf("Dir set to %d \n", dir);
                pot->SelectMe();
                ir_repeat_nb = 0;
                ir_direction = dir;
                ir_increment = ir_base_increment;
                ChangeSelectedPotLevel(ir_increment * ir_direction);
                break;
            }
        }
    } // if is_repeat
}



/* Function to call in loop 
    update the timer to return to the default selection
*/
void CheckPotLoop(void) {
    PotTimer.update();      // Check time, call the callback if time elapsed
    PotTimerLedOff.update();

}

// Reload timer before return to default selection
void ReinitTimer(void) {
    PotTimer.interval(TIME_TO_DEFAULT);
    PotTimer.start();
    PotTimerLedOff.interval(TIME_TO_SHUT_DOWN);
    PotTimerLedOff.start();
}

#define shift_test 4
// LogicPotentiometer methods 
LogicPotentiometer::LogicPotentiometer(int _out_pin, df_RingColor _color, String _name, PotRing *ringRef, double _level) {
    listPot.push_back(this);    // Add this instance to the object list
    potNum = listPot.size();
    if (potNum == 1) {  // Start PotTimer on the 1st instance of LogicPotentiometer
        PotTimer.start();
    } 
    name = _name;
    //outPin = _out_pin;
    ledRing = ringRef;
    level = _level;
    ledcSetup(PotNum2PwmChanel(potNum), pwmFreq, pwmResol);   // Initialise this PWM channel
    ledcAttachPin(_out_pin, PotNum2PwmChanel(potNum));    // PotNum give also the PWM channel 
    //sprintf(printStr,""  %s ", _name);
    memcpy(&ledColor, &_color, sizeof(df_RingColor));
    Serial.println("Create potentiometer " + name) ;
    ChangeLevel(0.0);   // Update pwm output
} //Constructor

void LogicPotentiometer::ChangeLevel(double inc) {
    level += inc;
    if (level > 1.0) level = 1.0;
    if (level < 0.0) level = 0.0;
    if (this != listPot[0]) {       // If we are not on the default potentiometer, reload timer 
        ReinitTimer();
    }
    ledRing->setRing(level, PotRing::one_point, ledColor);
    int pwmLevel = int(round(level * pwmMaxVal));
    ledcWrite(PotNum2PwmChanel(potNum), pwmLevel);
    toPrintStr = "Pot " + String(name) + " add " + String(inc,4) + " inc level= " + 
        String(level) + " Chan "+ String(potNum) + " level= " + pwmLevel + "\n";

    // msg_pool.push_front(toPrint);
    print_pool(toPrintStr);

}   

// Provided to turn off led ring after a certain time
void LogicPotentiometer::ShutdownRing(void) {
    ledRing->setRing(0.0, PotRing::one_point, RC_COLOR_BLACK);
}

// Set this intance of potentiometer as selected potentiometer
void LogicPotentiometer::SelectMe(void) {
    int potPos = 0;
    for (auto pot: listPot) {
        if (pot == this) {
           selPotIndx = potPos; // We are the selected instance
        }
        potPos ++;
    }
    Serial.println("Pot " + name+ " is selected");
}


void LogicPotentiometer::AttachIRCode(uint32_t up, uint32_t down) {
    ir_up = up;
    ir_down = down;
}

// Return Ir direction (-1/1) if reconised 0 else
int LogicPotentiometer::CheckRecevedIR(uint32_t ir_rx) {
    int ret = 0;
    if (ir_rx == ir_up) {
        ret = 1;
    }
    else if (ir_rx == ir_down) {
        ret = -1;
    }
    return(ret);    
}
