#include "Arduino.h"
#include "PrintUtil.h"
#include <list>


char printStr[128]="";
char fbuf1[16]="";
char fbuf2[16]="";

std::list<String> msg_pool = {};
String toPrintStr;
char toPrintChar[256];

void print_pool(String str) {
    msg_pool.push_front(str);
}


void print_loop_handle(void) {
    String msg;
    if (msg_pool.size() > 0) {
        msg = msg_pool.front();
        msg_pool.pop_front();
        Serial.print(msg);
        Debug.print(msg);
    }
}


// prints a number with 2 digits following the decimal place
// creates the string backwards, before printing it character-by-character from
// the end to the start
//
// Usage: sToStr(buf, 270.458l, 2)  return "270.46"
char * fToStr(char *buf, double fVal, int nbd)
{
    int dVal, dec;
    int digit = 0;
    int decf = 1;
    // Adjust the decimal part
    for (int d=nbd; d>0; d-- ) {decf *= 10;}    // decf = pow10(nbd) without math.h
    fVal += (double) 0.5 / decf;   // Rounding to the requested decimal
    // Prepare the entiere part
    dVal = fVal;
#if 0   // 0:With or 1: without printf
    // Doesnt works
    int nbe =0; // Number of entier decimal (nb of digits before the dot)
    for (int ent=1; ent<dVal; ent*=10) nbe++; // Calculate the number of digit left to the dot
    while (nbe > 0) {   // fill the buffer
        buf[digit+nbe-1] = (dVal % 10) + '0';
        dVal /= 10;
        nbe --;
    }
#else
    digit = sprintf(buf,"%d", dVal);
#endif
    buf[digit] = '.'; // Remove also the 0 terminated string added by sprintf
    dec = (int)(fVal * decf) % (int) decf;  // dec is the decimal part of fVal (digit after comma)
    buf[digit+nbd+1]='\0';  // The end of the result
    while (nbd > 0)
    {
        buf[digit+nbd] = (dec % 10) + '0';
        dec /= 10;
        nbd --;
    }
    return(buf);
}
