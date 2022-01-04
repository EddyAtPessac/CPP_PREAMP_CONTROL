#include <Arduino.h>
#include <list>
#include <RemoteDebug.h>

#ifndef __PRINT_UTIL__
#define __PRINT_UTIL__

// static permit to use this code in multiple sources
extern char printStr[];
extern char fbuf1[];
extern char fbuf2[];
extern String toPrint;

extern RemoteDebug Debug;
extern std::list<String> msg_pool;
extern String toPrintStr;
extern char toPrintChar[];

void print_loop_handle(void);
void print_pool(String str);


#ifdef totoENABLE_PRINT

// disable Serial output
#define Serial SomeOtherwiseUnusedName
static class {
public:
    void begin(...) {}
    void print(...) {}
    void println(...) {}
} Serial;
#endif


char * fToStr(char *buf, double fVal, int nbd=2);

#endif
