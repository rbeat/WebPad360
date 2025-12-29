#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_TUSB_OS               OPT_OS_PICO
#define BOARD_TUD_RHPORT          0
#define CFG_TUSB_RHPORT0_MODE     (OPT_MODE_DEVICE)
#define CFG_TUSB_RHPORT1_MODE     OPT_MODE_NONE

#define CFG_TUD_ENABLED           1
#define CFG_TUD_ENDPOINT0_SIZE    64

#define CFG_TUD_CDC               0
#define CFG_TUD_MSC               0
#define CFG_TUD_HID               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_VENDOR            0
#define CFG_TUD_ECM_RNDIS         0

#define CFG_TUH_ENABLED           0

#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN        __attribute__ ((aligned(4)))
#endif

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
