#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include "framework.h"

void ftoa(float n, char *res, int res_size, int afterpoint);
void encodeFloat(uint8_t *buf, float f);
float decodeFloat(uint8_t *buf);

#endif