#ifndef NEWPING_ADAPTER_H
#define NEWPING_ADAPTER_H

#include "ISonar.h"
#include <NewPing.h>

class NewPingAdapter : public ISonar {
private:
    NewPing _sonar;
public:
    NewPingAdapter(uint8_t triggerPin, uint8_t echoPin, uint16_t maxDistance = 500);
    ~NewPingAdapter() override = default;
    uint16_t ping() override;
    uint16_t convert_cm(uint16_t microseconds) override;
    uint16_t convert_in(uint16_t microseconds) override;
};

#endif // NEWPING_ADAPTER_H
