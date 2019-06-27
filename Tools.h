#ifndef Tools_h
#define Tools_h

#include <Arduino.h>

class Tools {
public:
    Tools();

    ~Tools();

    static void parseSerialCommand(const char *command, const char *param);

    static char *getUptime();

    static char *getCharArray(char *args[], int len);

    static char *intToChar(int value);

    static char *stringReplace(char *str, char *find, char *replace);

    static bool millisOverflowIsClose();

    static char *getMegaUptime();

    static void megaRestart();
};

#endif /* Tools_h */
