#include "siphash24.h"
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(openpaygo_token, CONFIG_OPENPAYGO_TOKEN_LOG_LEVEL);

uint32_t extract_bits(uint32_t source, uint32_t from, uint32_t to){
    uint32_t mask = ((1 << (to - from + 1)) - 1) << from;
    return (source & mask) >> from;
}

uint32_t convert_hash_to_token(uint64_t hash){
    uint32_t hi_hash = hash >> 32;
    uint32_t lo_hash = hash & 0xFFFFFFFF;

    hi_hash ^= lo_hash;

    uint32_t result = extract_bits(hi_hash, 2, 32);

    if(result > 999999999){
        result -= 73741825;
    }
    return result;
}

uint32_t generate_openpaygo_token(uint32_t last_token, uint8_t secret_key[16]){
    uint8_t a[8];
    
    for(int i = 0; i < 4; i++){
        uint8_t value = (last_token >> ((3 - i) * 8));
        a[i] = value;
        a[i + 4] = value;
    }

    uint64_t hash = siphash24(a, 8, secret_key);

    return convert_hash_to_token(hash);
}