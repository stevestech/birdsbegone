#include "stringToUint.h"


bool stringToUint(const char *string, uint16_t *result) {
    // Set to the next character in string after the numerical value, hopefully this is the null terminator
    char *end;
    
    // Used to detect overflow errors while running strtol
    errno = 0;
    
    // First, convert the string to a long
    long longResult = strtol(string, &end, BASE_TEN);
    
    // Only proceed if the string is a valid uint16_t
    if (errno ||                        // Set in the event of an overflow condition
        end == string ||                // No conversion was performed
        *end != ASCII_NUL ||            // The entire string was not converted
        longResult < 0 ||               // The value is not in the valid range for a uint16_t
        longResult >= UINT16_MAX) {
            
            return false;
    }
    
    *result = (uint16_t) longResult;
    return true;
}


bool stringToUint(const char *string, uint8_t *result) {
    // Set to the next character in string after the numerical value, hopefully this is the null terminator
    char *end;
    
    // Used to detect overflow errors while running strtol
    errno = 0;
    
    // First, convert the string to a long
    long longResult = strtol(string, &end, BASE_TEN);
    
    // Only proceed if the string is a valid uint16_t
    if (errno ||                        // Set in the event of an overflow condition
        end == string ||                // No conversion was performed
        *end != ASCII_NUL ||            // The entire string was not converted
        longResult < 0 ||               // The value is not in the valid range for a uint16_t
        longResult >= UINT8_MAX) {
            
            return false;
    }
    
    *result = (uint8_t) longResult;
    return true;
}

