#ifndef STRINGTOUINT_H
#define STRINGTOUINT_H

#include <stdint.h>

#ifndef ASCII_NUL
#define ASCII_NUL                       '\0'
#endif

#define UINT16_MAX                      ((uint32_t)-1)
#define UINT8_MAX                       ((uint8_t)-1)

#define BASE_TEN                        10

bool stringToUint(const char *string, uint16_t *result);
bool stringToUint(const char *string, uint8_t *result);

#endif
