#ifndef OPENPAYGO_TOKEN_H_
#define OPENPAYGO_TOKEN_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup openpaygo_token OpenPAYGO-Token
    * @ingroup lib
    * @{
    * 
 */

#if CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS > 16
    typedef uint32_t used_counts_t;
    #define USED_COUNTS_MAX UINT32_MAX
#else
    typedef uint16_t used_counts_t;
    #define USED_COUNTS_MAX UINT16_MAX
#endif

/**
 * @defgroup openpaygo_token_constants OpenPAYGO-Token Constants
 * @{
 */
#define OPENPAYGO_TOKEN_COUNTER_SYNC_VALUE 999 /**< OpenPAYGO-Token Counter Sync value */
#define OPENPAYGO_TOKEN_PAYG_DISABLE_VALUE 998 /**< OpenPAYGO-Token PAYG Disable value */
#define OPENPAYGO_TOKEN_MAX_ACTIVATION_VALUE 995 /**< Max activation value */

#define OPENPAYGO_TOKEN_OLDER_TOKEN_VALID -2 /**< Error code if token is valid but it is older*/
#define OPENPAYGO_TOKEN_TOKEN_INVALID -1 /**< Error code if token is invalid*/

/** @} */

/**
 * @defgroup openpaygo_token_structs OpenPAYGO-Token Structures
 * @{
 */
/**
 * @brief OpenPAYGO token configuration
 * 
 */
struct openpaygo_token_config{
    const uint8_t secret_key[16]; /**< 16 bytes Secret key */
    uint32_t starting_code; /**< 9 digit Starting code */
};

/**
 * @brief OpenPAYGO-Token state
 * 
 */
struct openpaygo_token_state{
    uint16_t count; /**< Internal count of the token decoder */
#if CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS > 0
    used_counts_t used_counts; /**< Bitfield of used counts */
#endif
};

/** @} */

/**
 * @defgroup openpaygo_token_api OpenPAYGO-Token API
 * @{
 */
/**
 * @brief Decode a token and return the value
 * 
 * @param conf OpenPAYGO-Token configuration
 * @param state OpenPAYGO-Token state
 * @param input_token Input token to be decoded
 * @return int16_t Value of the token
 * @return OPENPAYGO_TOKEN_TOKEN_INVALID if the token is invalid
 * @return OPENPAYGO_TOKEN_OLDER_TOKEN_VALID if the token is valid but already used
 */
int16_t openpaygo_token_decode(const struct openpaygo_token_config * conf, struct openpaygo_token_state * state, uint64_t input_token);

/** @} */
/** @} */


#endif /* OPENPAYGO_TOKEN_H_ */
