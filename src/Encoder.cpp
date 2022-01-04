#include "LogicPotentiometer.h"
#include "Encoder.h"
#include "PrintUtil.h"


double base_increment = LOGIC_POTENTIOMETER_BASE_INCREMENT;
const long debound_time = 2;
const long speed_Fast_1 = 10;
const long speed_Fast_2 = 40;
int pin_A;
int pin_B;
int pin_Push;
int debug_pin;
bool last_a_state;
bool a_state;
long temps_A;
long push_time;
void (*change_Level)(double);
void (*select_Next)(void);

void InitialiseEncoder(int pin_a, int pin_b, int pin_push, void change_level(double), void select_next(void) ) {
    pin_A = pin_a;
    pin_B = pin_b;
    pin_Push = pin_push;
    pinMode (pin_A, INPUT_PULLUP);
    pinMode (pin_B, INPUT_PULLUP);
    pinMode (pin_Push, INPUT_PULLUP);
    pinMode (debug_pin, OUTPUT);
    digitalWrite(debug_pin, 1);     // Montre fin d'IT
    delay(10);
    digitalWrite(debug_pin, 0);     // Montre fin d'IT
    last_a_state = digitalRead(pin_A);
    push_time = temps_A = millis();
    change_Level = change_level;
    select_Next = select_next;
    attachInterrupt(digitalPinToInterrupt(pin_A), IsrEncoderPinA_Change, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pin_Push), IsrEncoderPin_Push, RISING );  // On push release
    Serial.println("Base increment: "+ String(fToStr(fbuf1, base_increment, 4)));
}


void ICACHE_RAM_ATTR IsrEncoderPin_Push(void){
    ulong  dtime = millis()-push_time; 
    Serial.println("ISR Push");
    if (dtime < debound_time ) {
        return;
    }
    Serial.println("Push");
    select_Next();
    push_time =  millis();
}


void ICACHE_RAM_ATTR IsrEncoderPinA_Change() {
    double inc = 1.0l;

    digitalWrite(debug_pin, 1);     // Enter in the interrupt 
    ulong  dtime = millis()-temps_A; 
    if (dtime < debound_time ) {
        digitalWrite(debug_pin, 0); 
        return;
    }
    //Serial.println("DTime: "+ String(dtime));
    if (dtime < speed_Fast_1) {
        inc = 10.0l;
    }
    else if (dtime < speed_Fast_2) {
        inc = 5.0l;
    }
    // on mesure A
    a_state = digitalRead(pin_A);
    // Si B different de l'ancien état de A alors
    if(digitalRead(pin_B) != last_a_state){
        inc *= -1.0;
    }

    //Serial.println("Inc int before " + String(int(inc)));
    inc *= base_increment;
    //Serial.println("Inc x100000 int after " + String(int(inc*100000l)));
    temps_A = millis();  // memorisation du temps pour A    
    last_a_state = a_state ; // memorisation de l'état de A
    change_Level(inc);
    digitalWrite(debug_pin, 0);     // Montre fin d'IT
    //Serial.println("ISR end");  // Evite les printf dans les interrupt !!!
}   // IsrEncoderPinA_Change

