#ifndef _POT_RING_
#define _POT_RING_

#include <Arduino.h>
#include "Adafruit_neopixel.h"
//#include "LogicPotentiometer.h"     // For df_pot_color

//  Header for the function that drive Neoring to show level in various color.

typedef struct RingColor {
    uint8_t r;
    uint8_t v;
    uint8_t b;
} df_RingColor;

extern df_RingColor RC_COLOR_RED;
extern df_RingColor RC_COLOR_MAGENTA;
extern df_RingColor RC_COLOR_BLUE;
extern df_RingColor RC_COLOR_GREEN;
extern df_RingColor RC_COLOR_BLACK;

class PotRing : public Adafruit_NeoPixel {

    public:
    //static const int MODE_ONE_POINT;
    enum ring_mode {
        undef = 0,
        one_point
    };

    static void lowBrightness(df_RingColor &color, uint shift);
    static bool isSameColor(df_RingColor colorA, df_RingColor colorB);
    static void copyColor(df_RingColor colorA, df_RingColor colorB);

    PotRing(int pin, int led_count);

    void setRing(double level, ring_mode mode, df_RingColor color);
    void attachLedRing(ring_mode mode, df_RingColor color);
    void UpdateRing();

    private:
    uint led_count;
    df_RingColor last_colour;
    uint last_led_indx;
    
    // Last request 
    double level;
    ring_mode mode;
    df_RingColor color;

    //PotRing *ledRing;
    void setRingOnePointMode(double level, df_RingColor color);
    bool isRingChanged(int led_idx, df_RingColor color);

};

#endif