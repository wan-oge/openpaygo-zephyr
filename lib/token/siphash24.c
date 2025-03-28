#include "siphash24.h"
#include <assert.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(openpaygo_token, CONFIG_OPENPAYGO_TOKEN_LOG_LEVEL);

static inline uint64_t unaligned_read_le64(const void *_u) {
        const struct __attribute__((__packed__, __may_alias__)) { uint64_t x; } *u = _u;

        return sys_le64_to_cpu(u->x);
}

static uint64_t rotate_left(uint64_t x, uint8_t b)
{
    assert(b < 64);
    return (x << b) | (x >> (64 - b));
}

static void sipround(struct siphash *state){
        assert(state);

    state->v0 += state->v1;
    state->v1 = rotate_left(state->v1, 13);
    state->v1 ^= state->v0;
    state->v0 = rotate_left(state->v0, 32);
    state->v2 += state->v3;
    state->v3 = rotate_left(state->v3, 16);
    state->v3 ^= state->v2;
    state->v0 += state->v3;
    state->v3 = rotate_left(state->v3, 21);
    state->v3 ^= state->v0;
    state->v2 += state->v1;
    state->v1 = rotate_left(state->v1, 17);
    state->v1 ^= state->v2;
    state->v2 = rotate_left(state->v2, 32);
}

void siphash24_init(struct siphash *state, const uint8_t k[static 16]) {
        uint64_t k0, k1;

        assert(state);
        assert(k);

        k0 = unaligned_read_le64(k);
        k1 = unaligned_read_le64(k + 8);

        *state = (struct siphash) {
                /* "somepseudorandomlygeneratedbytes" */
                .v0 = 0x736f6d6570736575ULL ^ k0,
                .v1 = 0x646f72616e646f6dULL ^ k1,
                .v2 = 0x6c7967656e657261ULL ^ k0,
                .v3 = 0x7465646279746573ULL ^ k1,
                .padding = 0,
                .inlen = 0,
        };
}

void siphash24_compress(const void *_in, uint32_t inlen, struct siphash *state) {
        assert(_in);
        
        const uint8_t *in = (const uint8_t *)_in;
        const uint8_t *end = in + inlen;
        uint32_t left = state->inlen & 7;
        uint64_t m;

        assert(state);

        /* Update total length */
        state->inlen += inlen;

        /* If padding exists, fill it out */
        if (left > 0) {
                for ( ; in < end && left < 8; in ++, left ++)
                        state->padding |= ((uint64_t) *in) << (left * 8);

                if (in == end && left < 8)
                        /* We did not have enough input to fill out the padding completely */
                        return;

                state->v3 ^= state->padding;
                sipround(state);
                sipround(state);
                state->v0 ^= state->padding;

                state->padding = 0;
        }

        end -= (state->inlen % sizeof(uint64_t));

        for ( ; in < end; in += 8) {
                m = unaligned_read_le64(in);

                state->v3 ^= m;
                sipround(state);
                sipround(state);
                state->v0 ^= m;
        }

        left = state->inlen & 7;
        switch (left) {
                case 7:
                        state->padding |= ((uint64_t) in[6]) << 48;
                case 6:
                        state->padding |= ((uint64_t) in[5]) << 40;
                case 5:
                        state->padding |= ((uint64_t) in[4]) << 32;
                case 4:
                        state->padding |= ((uint64_t) in[3]) << 24;
                case 3:
                        state->padding |= ((uint64_t) in[2]) << 16;
                case 2:
                        state->padding |= ((uint64_t) in[1]) <<  8;
                case 1:
                        state->padding |= ((uint64_t) in[0]);
                case 0:
                        break;
        }
}

uint64_t siphash24_finalize(struct siphash *state) {
        uint64_t b;

        assert(state);

        b = state->padding | (((uint64_t) state->inlen) << 56);

        state->v3 ^= b;
        sipround(state);
        sipround(state);
        state->v0 ^= b;

        state->v2 ^= 0xff;

        sipround(state);
        sipround(state);
        sipround(state);
        sipround(state);

        return state->v0 ^ state->v1 ^ state->v2  ^ state->v3;
}

uint64_t siphash24(const void *in, uint32_t inlen, const uint8_t k[static 16]) {
        struct siphash state;

        assert(in);
        assert(k);

        siphash24_init(&state, k);
        siphash24_compress(in, inlen, &state);

        return siphash24_finalize(&state);
}