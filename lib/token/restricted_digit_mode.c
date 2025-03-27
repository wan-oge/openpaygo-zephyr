#include "restricted_digit_mode.h"
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(openpaygo_token, CONFIG_OPENPAYGO_TOKEN_LOG_LEVEL);

void store_n_bits_in_array(bool* boolean_array, uint64_t data, uint8_t number_of_bits, uint8_t buffer_start_bit) {
    for (int i = 0; i < number_of_bits; i++) {
        boolean_array[i + buffer_start_bit] = !!(data & (1 << ((number_of_bits - 1 - i))));
        ;
    }
}

uint32_t get_int_from_boolean_array(bool* boolean_array, uint8_t start_position, uint8_t length) {
    uint32_t buffer = 0;
    for (int i = start_position; i < start_position + length; i++) {
        buffer = (buffer << 1) + (boolean_array[i] ? 1 : 0);
    }
    return buffer;
}

uint32_t convert_from_four_digit_token(uint64_t four_digit_token) {
    // This is made for 15 digit long codes converting to 9.
    // For other lengths the code needs to be adapted
    bool token_boolean_array[30] = {0};
    uint8_t this_digit;

    for (int i = 0; i < 15; i++) {
        this_digit = (four_digit_token % 10) - 1;
        store_n_bits_in_array(token_boolean_array, this_digit, 2, (14 - i) * 2);
        four_digit_token /= 10;
    }

    return get_int_from_boolean_array(token_boolean_array, 0, 30);
}