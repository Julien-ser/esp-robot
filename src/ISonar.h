#ifndef ISONAR_H
#define ISONAR_H

#include <stdint.h>

class ISonar {
public:
    virtual ~ISonar() = default;
    virtual uint16_t ping() = 0;
    virtual uint16_t convert_cm(uint16_t microseconds) = 0;
    virtual uint16_t convert_in(uint16_t microseconds) = 0;
};

#endif // ISONAR_H
