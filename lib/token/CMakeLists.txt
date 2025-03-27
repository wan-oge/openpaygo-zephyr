# Copyright (c) 2024 Off-Grid Europe GmbH

zephyr_library()
zephyr_library_sources(
    token.c
    core.c
    value_utils.c
    siphash24.c
)

if(CONFIG_OPENPAYGO_TOKEN_RESTRICTED_DIGIT_SET_MODE)
    zephyr_library_sources(restricted_digit_mode.c)
endif()

if(CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS GREATER 0)
    zephyr_library_sources(unused_older_tokens.c)
endif()
