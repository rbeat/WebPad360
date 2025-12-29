#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/httpd.h"
#include "xinput.h"
#include "pico/stdio.h"
#include "pico/error.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "tusb.h"
#include "bsp/board_api.h"
#include "xinput_driver.h"
#include "XInputDescriptors.h"


xinput_gamepad_t gamepad = {0};
static XInputReport xinput_report = {
    .report_id = 0,
    .report_size = XINPUT_ENDPOINT_SIZE,
};
static bool button_release_pending = false;
static absolute_time_t button_release_time;

extern const unsigned char index_html[];
extern const unsigned int index_html_len;

static void blink_led(void) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(100);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

static void send_gamepad_state(void) {
    uint8_t buttons1 = 0;
    uint8_t buttons2 = 0;

    if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) buttons1 |= XBOX_MASK_UP;
    if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) buttons1 |= XBOX_MASK_DOWN;
    if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) buttons1 |= XBOX_MASK_LEFT;
    if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) buttons1 |= XBOX_MASK_RIGHT;
    if (gamepad.wButtons & XINPUT_GAMEPAD_START) buttons1 |= XBOX_MASK_START;
    if (gamepad.wButtons & XINPUT_GAMEPAD_BACK) buttons1 |= XBOX_MASK_BACK;
    if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) buttons1 |= XBOX_MASK_LS;
    if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) buttons1 |= XBOX_MASK_RS;

    if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) buttons2 |= XBOX_MASK_LB;
    if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) buttons2 |= XBOX_MASK_RB;
    if (gamepad.wButtons & XINPUT_GAMEPAD_GUIDE) buttons2 |= XBOX_MASK_HOME;
    if (gamepad.wButtons & XINPUT_GAMEPAD_A) buttons2 |= XBOX_MASK_A;
    if (gamepad.wButtons & XINPUT_GAMEPAD_B) buttons2 |= XBOX_MASK_B;
    if (gamepad.wButtons & XINPUT_GAMEPAD_X) buttons2 |= XBOX_MASK_X;
    if (gamepad.wButtons & XINPUT_GAMEPAD_Y) buttons2 |= XBOX_MASK_Y;

    xinput_report.buttons1 = buttons1;
    xinput_report.buttons2 = buttons2;
    xinput_report.lt = gamepad.bLeftTrigger;
    xinput_report.rt = gamepad.bRightTrigger;
    xinput_report.lx = gamepad.sThumbLX;
    xinput_report.ly = gamepad.sThumbLY;
    xinput_report.rx = gamepad.sThumbRX;
    xinput_report.ry = gamepad.sThumbRY;

    send_report(&xinput_report, sizeof(xinput_report));
}

static void usb_service(void) {
    tud_task();
    receive_xinput_report();
    if (button_release_pending && time_reached(button_release_time)) {
        button_release_pending = false;
        gamepad.wButtons = 0;
        gamepad.bLeftTrigger = 0;
        gamepad.bRightTrigger = 0;
        gamepad.sThumbLX = 0;
        gamepad.sThumbLY = 0;
        gamepad.sThumbRX = 0;
        gamepad.sThumbRY = 0;
        send_gamepad_state();
    }
}

const char* cgi_index_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    return "/index.html";
}

const char* cgi_btn_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    const char* btn = NULL;
    for (int i = 0; i < iNumParams; i++) {
        if (strcmp(pcParam[i], "code") == 0) {
            btn = pcValue[i];
            break;
        }
    }

    gamepad.wButtons = 0;

    if (!btn) {
        printf("No button\n");
        return "/index.html";
    }

    if      (strcmp(btn, "A") == 0)      gamepad.wButtons |= XINPUT_GAMEPAD_A;
    else if (strcmp(btn, "B") == 0)      gamepad.wButtons |= XINPUT_GAMEPAD_B;
    else if (strcmp(btn, "X") == 0)      gamepad.wButtons |= XINPUT_GAMEPAD_X;
    else if (strcmp(btn, "Y") == 0)      gamepad.wButtons |= XINPUT_GAMEPAD_Y;
    else if (strcmp(btn, "GUIDE") == 0)  gamepad.wButtons |= XINPUT_GAMEPAD_GUIDE;
    else if (strcmp(btn, "START") == 0)  gamepad.wButtons |= XINPUT_GAMEPAD_START;
    else if (strcmp(btn, "BACK") == 0)   gamepad.wButtons |= XINPUT_GAMEPAD_BACK;
    else if (strcmp(btn, "LB") == 0)     gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_SHOULDER;
    else if (strcmp(btn, "RB") == 0)     gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_SHOULDER;
    else if (strcmp(btn, "L_THUMB") == 0)gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_THUMB;
    else if (strcmp(btn, "R_THUMB") == 0)gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_THUMB;
    else if (strcmp(btn, "DPAD_UP") == 0)gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
    else if (strcmp(btn, "DPAD_DOWN") == 0)gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
    else if (strcmp(btn, "DPAD_LEFT") == 0)gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
    else if (strcmp(btn, "DPAD_RIGHT") == 0)gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
    else {
        printf("Unknown: %s\n", btn);
        return "/index.html";
    }

    printf("Pressed: %s (0x%04X)\n", btn, gamepad.wButtons);
    send_gamepad_state();
    blink_led();
    button_release_pending = true;
    button_release_time = make_timeout_time_ms(100);
    return "/index.html";
}

const tCGI cgi_handlers[] = {
    {"/", cgi_index_handler},
    {"/btn", cgi_btn_handler},
};

static void configure_static_ip(void) {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gateway;
    IP4_ADDR(&ip, 192, 168, 0, 115);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gateway, 192, 168, 0, 1);

    cyw43_arch_lwip_begin();
    struct netif *netif = &cyw43_state.netif[CYW43_ITF_STA];
#if LWIP_DHCP
    dhcp_stop(netif);
#endif
    netif_set_addr(netif, &ip, &netmask, &gateway);
    netif_set_default(netif);
    netif_set_up(netif);

#if LWIP_DNS
    ip_addr_t dns;
    ip_addr_copy_from_ip4(dns, gateway);
    dns_setserver(0, &dns);
#endif
    cyw43_arch_lwip_end();

    printf("Static IP configured: %s\n", ip4addr_ntoa(&ip));
}

static const char* wifi_error_str(int err) {
    switch (err) {
        case PICO_ERROR_TIMEOUT: return "timeout waiting for IP";
        case PICO_ERROR_BADAUTH: return "authentication failed";
        case PICO_ERROR_CONNECT_FAILED: return "access point not found";
        default: return "unexpected error";
    }
}

void serve_web_page() {
    http_set_cgi_handlers(cgi_handlers, sizeof(cgi_handlers)/sizeof(tCGI));
    httpd_init();
    printf("HTTP server started\n");
}

int main() {
    stdio_init_all();
    board_init();
    tusb_init();
    send_gamepad_state();

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_WORLDWIDE)) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();
    configure_static_ip();

    // Your Wi-Fi credentials go here
    const char *ssid = "SSID_NAME";
    const char *password = "WIFI_PASSWORD";

    int err;
    int attempt = 0;
    const int max_attempts = 5;
    do {
        attempt++;
        printf("Connecting to Wi-Fi (attempt %d)...\n", attempt);
        err = cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000);
        if (err) {
            printf("Wi-Fi failed (%s, code %d)\n", wifi_error_str(err), err);
            for (int i = 0; i < 2000; i++) {
                usb_service();
                sleep_ms(1);
            }
        }
    } while (err && attempt < max_attempts);

    if (err) {
        printf("Wi-Fi failed after %d attempts, giving up\n", attempt);
        return err;
    }

    printf("Connected!\n");
    serve_web_page();

    while (true) {
        usb_service();
        sleep_ms(1);
    }
}
