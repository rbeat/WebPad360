/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby
 */

#pragma once

#include <stdint.h>

#define XINPUT_ENDPOINT_SIZE 20

// Buttons 1 (8 bits)
#define XBOX_MASK_UP    (1u << 0)
#define XBOX_MASK_DOWN  (1u << 1)
#define XBOX_MASK_LEFT  (1u << 2)
#define XBOX_MASK_RIGHT (1u << 3)
#define XBOX_MASK_START (1u << 4)
#define XBOX_MASK_BACK  (1u << 5)
#define XBOX_MASK_LS    (1u << 6)
#define XBOX_MASK_RS    (1u << 7)

// Buttons 2 (8 bits)
#define XBOX_MASK_LB    (1u << 0)
#define XBOX_MASK_RB    (1u << 1)
#define XBOX_MASK_HOME  (1u << 2)
#define XBOX_MASK_A     (1u << 4)
#define XBOX_MASK_B     (1u << 5)
#define XBOX_MASK_X     (1u << 6)
#define XBOX_MASK_Y     (1u << 7)

typedef struct __attribute__((packed, aligned(1)))
{
    uint8_t report_id;
    uint8_t report_size;
    uint8_t buttons1;
    uint8_t buttons2;
    uint8_t lt;
    uint8_t rt;
    int16_t lx;
    int16_t ly;
    int16_t rx;
    int16_t ry;
    uint8_t _reserved[6];
} XInputReport;

static const uint8_t xinput_string_language[]    = { 0x09, 0x04 };
static const uint8_t xinput_string_manfacturer[] = "Microsoft";
static const uint8_t xinput_string_product[]     = "XInput STANDARD GAMEPAD";
static const uint8_t xinput_string_version[]     = "1.0";

static const uint8_t *xinput_string_descriptors[] =
{
    xinput_string_language,
    xinput_string_manfacturer,
    xinput_string_product,
    xinput_string_version
};

static const uint8_t xinput_device_descriptor[] =
{
    0x12, 0x01, 0x00, 0x02,
    0xFF, 0xFF, 0xFF,
    0x40,
    0x5E, 0x04,
    0x8E, 0x02,
    0x14, 0x01,
    0x01, 0x02, 0x03,
    0x01,
};

static const uint8_t xinput_configuration_descriptor[] =
{
    0x09, 0x02, 0x30, 0x00, 0x01, 0x01, 0x00, 0x80, 0xFA,

    0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x5D, 0x01, 0x00,

    0x10, 0x21, 0x10, 0x01, 0x01, 0x24,
    0x81, 0x14, 0x03,
    0x00, 0x03, 0x13,
    0x01, 0x00, 0x03,
    0x00,

    0x07, 0x05, 0x81, 0x03, 0x20, 0x00, 0x01,
    0x07, 0x05, 0x01, 0x03, 0x20, 0x00, 0x08,
};
