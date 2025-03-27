#ifndef OPENPAYGO_TOKEN_CORE_H_
#define OPENPAYGO_TOKEN_CORE_H_

#include <stdint.h>

uint32_t generate_openpaygo_token(uint32_t last_token, uint8_t secret_key[16]);

#endif /* OPENPAYGO_TOKEN_CORE_H_ */