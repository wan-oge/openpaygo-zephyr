#include <openpaygo/token.h>
#include <zephyr/ztest.h>

void insert_token(const struct openpaygo_token_config *config,
                                 struct openpaygo_token_state *state,
                                 uint32_t *days_active, int *ret,
                                 uint64_t token) {
  *ret = openpaygo_token_decode(config, state, token);
  switch (*ret) {
  case OPENPAYGO_TOKEN_COUNTER_SYNC_VALUE:
    break;
  case OPENPAYGO_TOKEN_PAYG_DISABLE_VALUE:
    *days_active = UINT32_MAX;
    break;
  case 0 ... OPENPAYGO_TOKEN_MAX_ACTIVATION_VALUE:
    if (state->count % 2) {
      *days_active = *ret;
    } else {
      *days_active = *days_active + *ret;
    }
    break;
  default:
    break;
  }
}

ZTEST(openpaygo_token_lib, test_decode_token) {
  const struct openpaygo_token_config config = {
      .secret_key = {0xa2, 0x9a, 0xb8, 0x2e, 0xdc, 0x5f, 0xbb, 0xc4, 0x1e, 0xc9,
                     0x53, 0x0f, 0x6d, 0xac, 0x86, 0xb1},
      .starting_code = 123456789,
  };
  struct openpaygo_token_state state = {
      .count = 0,
      .used_counts = USED_COUNTS_MAX,
  };

  int ret = 0;
  uint32_t days_active = 0;
  // G1 We enter an invalid token
  insert_token(&config, &state, &days_active, &ret, 123456789);
  zexpect_equal(ret, OPENPAYGO_TOKEN_TOKEN_INVALID,
                "G1 We enter an invalid token");

  // G2 We enter a valid token for setting one day
  insert_token(&config, &state, &days_active, &ret, 932267790);
  zexpect_equal(days_active, 1,
                "G2 We enter a valid token for setting one day");

  // G3 We enter a valid token for adding one day
  insert_token(&config, &state, &days_active, &ret, 953796790);
  zexpect_equal(days_active, 2, "G3 We enter a valid token for adding one day");

  // G4A We enter the same Add Time token for 1 day, the days should not be
  // added and the device should signal that the token was already used
  insert_token(&config, &state, &days_active, &ret, 953796790);
  zexpect_equal(
      days_active, 2,
      "G4A We enter the same Add Time token for 1 day, the days should not be "
      "added and the device should signal that the token was already used");

  // G4B We enter the older Set Time token for 1 day, the days should not change
  // and the device should signal that the token was already used
  insert_token(&config, &state, &days_active, &ret, 932267790);
  zexpect_equal(
      days_active, 2,
      "G4B We enter the older Set Time token for 1 day, the days should not "
      "change and the device should signal that the token was already used");

  // G5 We enter a valid token for setting 30 days and ensures it sets and does
  // not add to the existing
  insert_token(&config, &state, &days_active, &ret, 382662819);
  zexpect_equal(days_active, 30,
                "G5 We enter a valid token for setting 30 days and ensures it "
                "sets and does not add to the existing");

  // G6 We enter a valid token for setting 0 days and ensures the device is
  // inactive with the outputs disabled immediately
  insert_token(&config, &state, &days_active, &ret, 295647789);
  zexpect_equal(days_active, 0,
                "G6 We enter a valid token for setting 0 days and ensures the "
                "device is inactive with the outputs disabled immediately");

  // G7A We enter 3 consecutive Add Time tokens with the maximum amount of days
  // and ensure that they cumulate properly
  insert_token(&config, &state, &days_active, &ret, 699630784);
  zexpect_equal(days_active, 995.0,
                "G7A We enter 3 consecutive Add Time tokens with the maximum "
                "amount of days and ensure that they cumulate properly");

  // G7B
  insert_token(&config, &state, &days_active, &ret, 941068784);
  zexpect_equal(days_active, 1990.0, "G7B ");

  // G7C
  insert_token(&config, &state, &days_active, &ret, 286149784);
  zexpect_equal(days_active, 2985.0, "G7C ");

  // G8A We enter 21 consecutive Set Time tokens for 1, 2, 3, … 21 days each
  // with a count 30 higher than the other. The validation of the token should
  // not take more than 5 seconds
  insert_token(&config, &state, &days_active, &ret, 4144790);
  zexpect_equal(days_active, 1,
                "G8A We enter 21 consecutive Set Time tokens for 1, 2, 3, … 21 "
                "days each with a count 30 higher than the other. The "
                "validation of the token should not take more than 5 seconds");

  // G8B
  insert_token(&config, &state, &days_active, &ret, 970979791);
  zexpect_equal(days_active, 2, "G8B ");

  // G8C
  insert_token(&config, &state, &days_active, &ret, 866961792);
  zexpect_equal(days_active, 3, "G8C ");

  // G8D
  insert_token(&config, &state, &days_active, &ret, 198396793);
  zexpect_equal(days_active, 4, "G8D ");

  // G8E
  insert_token(&config, &state, &days_active, &ret, 421670794);
  zexpect_equal(days_active, 5, "G8E ");

  // G8F
  insert_token(&config, &state, &days_active, &ret, 708258795);
  zexpect_equal(days_active, 6, "G8F ");

  // G8G
  insert_token(&config, &state, &days_active, &ret, 529941796);
  zexpect_equal(days_active, 7, "G8G ");

  // G8H
  insert_token(&config, &state, &days_active, &ret, 917596797);
  zexpect_equal(days_active, 8, "G8H ");

  // G8I
  insert_token(&config, &state, &days_active, &ret, 999247798);
  zexpect_equal(days_active, 9, "G8I ");

  // G8J
  insert_token(&config, &state, &days_active, &ret, 821420799);
  zexpect_equal(days_active, 10, "G8J ");

  // G8K
  insert_token(&config, &state, &days_active, &ret, 206093800);
  zexpect_equal(days_active, 11, "G8K ");

  // G8L
  insert_token(&config, &state, &days_active, &ret, 295991801);
  zexpect_equal(days_active, 12, "G8L ");

  // G8M
  insert_token(&config, &state, &days_active, &ret, 195518802);
  zexpect_equal(days_active, 13, "G8M ");

  // G8N
  insert_token(&config, &state, &days_active, &ret, 892490803);
  zexpect_equal(days_active, 14, "G8N ");

  // G8O
  insert_token(&config, &state, &days_active, &ret, 820548804);
  zexpect_equal(days_active, 15, "G8O ");

  // G8P
  insert_token(&config, &state, &days_active, &ret, 367849805);
  zexpect_equal(days_active, 16, "G8P ");

  // G8Q
  insert_token(&config, &state, &days_active, &ret, 673717806);
  zexpect_equal(days_active, 17, "G8Q ");

  // G8R
  insert_token(&config, &state, &days_active, &ret, 981332807);
  zexpect_equal(days_active, 18, "G8R ");

  // G8S
  insert_token(&config, &state, &days_active, &ret, 955229808);
  zexpect_equal(days_active, 19, "G8S ");

  // G8T
  insert_token(&config, &state, &days_active, &ret, 312682809);
  zexpect_equal(days_active, 20, "G8T ");

  // G8U
  insert_token(&config, &state, &days_active, &ret, 890729810);
  zexpect_equal(days_active, 21, "G8U ");

  // G9 We enter a PAYG Disable token into the device
  insert_token(&config, &state, &days_active, &ret, 32850787);
  zexpect_equal(days_active, UINT32_MAX,
                "G9 We enter a PAYG Disable token into the device");

  // G10 We enter a Set Time token for 0 day, it should relock the device
  insert_token(&config, &state, &days_active, &ret, 36370789);
  zexpect_equal(
      days_active, 0,
      "G10 We enter a Set Time token for 0 day, it should relock the device");

  // G11A We enter a PAYG Disable token to relock the device, then enter a Add
  // Time token with 0 day, it should NOT relock the device (Optional)
  insert_token(&config, &state, &days_active, &ret, 528558787);
  zexpect_equal(
      days_active, UINT32_MAX,
      "G11A We enter a PAYG Disable token to relock the device, then enter a "
      "Add Time token with 0 day, it should NOT relock the device (Optional)");

  // G11B
  insert_token(&config, &state, &days_active, &ret, 707590789);
  zexpect_equal(days_active, UINT32_MAX, "G11B ");

  // G12A We deactivate the device with a Set Time of 0 days. We then wait 48
  // hours before entering a Add Time of 1 day and ensuring that the days late
  // are not considered in the activation time
  insert_token(&config, &state, &days_active, &ret, 475425789);
  zexpect_equal(
      days_active, 0,
      "G12A We deactivate the device with a Set Time of 0 days. We then wait "
      "48 hours before entering a Add Time of 1 day and ensuring that the days "
      "late are not considered in the activation time");

  // G12B
  insert_token(&config, &state, &days_active, &ret, 259057790);
  zexpect_equal(days_active, 1, "G12B ");

  // U1A We generate 3 Add Time tokens, then enter the 3rd, then first, then
  // second and ensure the days are added properly
  insert_token(&config, &state, &days_active, &ret, 46580849);
  zexpect_equal(days_active, 60,
                "U1A We generate 3 Add Time tokens, then enter the 3rd, then "
                "first, then second and ensure the days are added properly");

  // U1B
  insert_token(&config, &state, &days_active, &ret, 794230790);
  zexpect_equal(days_active, 61, "U1B ");

  // U1C
  insert_token(&config, &state, &days_active, &ret, 383775799);
  zexpect_equal(days_active, 71, "U1C ");

  // U1D
  insert_token(&config, &state, &days_active, &ret, 692453794);
  zexpect_equal(days_active, 76, "U1D ");

  // U2A We generate an Add Time, a Set Time and another Add Time token. We
  // enter the set time and ensure that the older Add Time does not work but the
  // newer does
  insert_token(&config, &state, &days_active, &ret, 436894799);
  zexpect_equal(days_active, 10,
                "U2A We generate an Add Time, a Set Time and another Add Time "
                "token. We enter the set time and ensure that the older Add "
                "Time does not work but the newer does");

  // U2B
  insert_token(&config, &state, &days_active, &ret, 936402794);
  zexpect_equal(ret, OPENPAYGO_TOKEN_TOKEN_INVALID, "U2B ");

  // U2C
  insert_token(&config, &state, &days_active, &ret, 773008792);
  zexpect_equal(days_active, 13, "U2C ");

  // U3A We generate an Add Time token and a Disable PAYG token, we enter the
  // Disable PAYG token and then the Add Time token should be refused
  insert_token(&config, &state, &days_active, &ret, 494064787);
  zexpect_equal(
      days_active, UINT32_MAX,
      "U3A We generate an Add Time token and a Disable PAYG token, we enter "
      "the Disable PAYG token and then the Add Time token should be refused");

  // U3B
  insert_token(&config, &state, &days_active, &ret, 816702790);
  zexpect_equal(ret, OPENPAYGO_TOKEN_TOKEN_INVALID, "U3B ");
}

ZTEST_SUITE(openpaygo_token_lib, NULL, NULL, NULL, NULL, NULL);