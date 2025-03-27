#ifndef OPENPAYGO_TOKEN_SIPHASH24_H_
#define OPENPAYGO_TOKEN_SIPHASH24_H_

#include <stdint.h>

struct siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        uint32_t inlen;
};

void siphash24_init(struct siphash *state, const uint8_t k[static 16]);
void siphash24_compress(const void *in, uint32_t inlen, struct siphash *state);
uint64_t siphash24_finalize(struct siphash *state);
uint64_t siphash24(const void *in, uint32_t inlen, const uint8_t k[static 16]);

#endif /* OPENPAYGO_TOKEN_SIPHASH24_H_ */
