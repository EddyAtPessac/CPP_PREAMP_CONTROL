#include "PotRing.h"


df_RingColor RC_COLOR_RED = {(uint8_t)255, (uint8_t)0, (uint8_t)0};
df_RingColor RC_COLOR_MAGENTA ={(uint8_t)(199>>1), (uint8_t)(21>>1), (uint8_t)(133>>1)};
df_RingColor RC_COLOR_BLUE = {(uint8_t)0, (uint8_t)0, (uint8_t)255};
df_RingColor RC_COLOR_GREEN = {(uint8_t)0, (uint8_t)255, (uint8_t)0};
df_RingColor RC_COLOR_BLACK = {(uint8_t)0, (uint8_t)0, (uint8_t)0};


// Argument 1 = Nombre de Pixel dans le Ring
// Argument 2 = Wemos Pin
// Argument 3 = Pixel type
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Led cablé selon GRB (most NeoPixel products)
//   NEO_GRBW    Led cablé selon GRB + W (most NeoPixel products) + Led blanche
//   NEO_RGB     Led cablé selon RGB (v1 FLORA pixels, not v2)
//   NEO_RGBW   led cablé selon  RGBW (NeoPixel RGBW products)






// Ici ca fait planter
PotRing::PotRing(int pin, int _led_count) 
        : Adafruit_NeoPixel(pin, _led_count, NEO_GRB + NEO_KHZ800) 
{

    led_count = _led_count;
    last_colour = RC_COLOR_BLACK;
    last_led_indx = 0;
    #if 0
    this->begin();
    //this->setBrightness(10); // Set BRIGHTNESS to about 1/5 (max = 255)
    this->fill(this->Color(0,0,255),0,22);   // allume la led 2,3 et 4  en bleu
    this->show();
    #endif
 }

bool PotRing::isSameColor(df_RingColor colorA, df_RingColor colorB) {
    bool ret = true;
    if (colorA.b != colorB.b) ret = false;
    if (colorA.r != colorB.r) ret = false;
    if (colorA.v != colorB.v) ret = false;
    return (ret);
}


bool PotRing::isRingChanged(int _led_idx, df_RingColor _color) {
    bool ret = false;
    if (_led_idx != last_led_indx)
        ret = true;
    if (! isSameColor(_color, last_colour))
        ret = true;
    last_led_indx = _led_idx;
    last_colour = _color;
    return(ret);
}

/*!
    return the color with all terms (r,g,b) shifted of shift to lower value
*/
void PotRing::lowBrightness(df_RingColor &color, uint shift) {
    color.r >>= shift;
    color.v >>= shift;
    color.b >>= shift;
}

void PotRing::copyColor(df_RingColor colorA, df_RingColor colorB) {
    ;
}

// Called in main loop, this subroutine update ring if it is changed
void PotRing::setRingOnePointMode(double level, df_RingColor color) {
    String led_display = String("..[..................]..");
    df_RingColor low_color= color; 
    lowBrightness(low_color, 2);
    //Serial.printf( "color, low_color: %06X , %06X \n", Color(color.r, color.v, color.b), Color(low_color.r, low_color.v, low_color.b));
    int idxmin = led_display.indexOf("[")+1;
    int idxmax = led_display.indexOf("]");
    int full_scalle = idxmax-idxmin-1;
    int led_to_light = round(full_scalle * level);
    df_RingColor led_pix = RC_COLOR_BLACK;
    led_display.setCharAt(idxmin + led_to_light,'*');
    //Serial.println("setRingOnePointMode");
    if (isRingChanged(led_to_light, color)) {
        Serial.println("Change display to: " + led_display);
        this->clear();
        this->begin();
        int i=0;
        for (char const  &c: led_display) {

            led_pix = (c=='.' ? RC_COLOR_BLACK : (c=='*' ? color : low_color ));
            this->setPixelColor(i++, led_pix.r, led_pix.v, led_pix.b);
        }
        //Serial.println("Show");
        this->show();
    }
}


/* Note the mode, level and color requested. As we are here in an interuption, 
    we only note the request. This is UpdateRing() called in the 
    main loop that update physicaly the ring.
*/
void PotRing::setRing(double _level, ring_mode _mode, df_RingColor _color) {
    level = _level;
    mode = _mode;
    color = _color;
}

void PotRing::UpdateRing() {
    switch (mode) {
        case one_point:
            setRingOnePointMode(level, color);
            break;
        default:
            break;
    }
}