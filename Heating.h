#ifndef Heating_h
#define Heating_h

#include <Arduino.h>

struct HeatingIntVariable {
    int *var;
    const char *key;

    HeatingIntVariable() {}

    HeatingIntVariable(int *_var, const char *_key) : var(_var), key(_key) {}
};

struct HeatingStringVariable {
    String *var;
    const char *key;

    HeatingStringVariable() {}

    HeatingStringVariable(String *_var, const char *_key) : var(_var), key(_key) {}
};

class Heating {
public:
    Heating();

    ~Heating();

    static void setVariable(int *var, const char *key);

    static void setVariable(String *var, const char *key);

    static int &getIntVariable(const char *key);

    static String &getStringVariable(const char *key);

    static void check();

    static void stop();
};

#endif /* Heating_h */
