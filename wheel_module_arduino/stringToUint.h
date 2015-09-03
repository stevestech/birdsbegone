#ifndef STRINGTOUINT_H
#define STRINGTOUINT_H

#include <stdint.h>

#define BASE_TEN                        10

bool stringToUint(const char *string, uint16_t *result);
bool stringToUint(const char *string, uint8_t *result);

#endif
