#include "tusb.h"
#include "XInputDescriptors.h"

uint8_t const *tud_descriptor_device_cb(void) {
    return xinput_device_descriptor;
}

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return xinput_configuration_descriptor;
}

static uint16_t _desc_str[32];

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;

    if (index >= TU_ARRAY_SIZE(xinput_string_descriptors)) {
        return NULL;
    }

    uint8_t chr_count = 0;

    if (index == 0) {
        _desc_str[1] = xinput_string_language[0] | (uint16_t)(xinput_string_language[1] << 8);
        chr_count = 1;
    } else {
        const char *str = (const char *)xinput_string_descriptors[index];
        chr_count = (uint8_t)strlen(str);
        if (chr_count > (TU_ARRAY_SIZE(_desc_str) - 1)) {
            chr_count = (TU_ARRAY_SIZE(_desc_str) - 1);
        }
        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));
    return _desc_str;
}
