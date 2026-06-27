#ifndef BAROMETER_H
#define BAROMETER_H

bool barometer_init();
void barometer_read(float* temp, float* pressure);

#endif
