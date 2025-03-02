#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "rp2040-map.h"
#include "helpers.h"
#include "gpio.h"
#include "spi.h"
#include "max7219.h"
#include "resets.h"
#include "clocks.h"
#include "uart.h"
#include "protocall.h"

#define LED_PIN (25)

static inline void set_spi0_pinmux(void);
static inline void set_uart0_pinmux(void);

int main(void) {
	init_xosc();
	set_perif_clock_xosc();

	set_reset(RESETS_PLL_USB, 0);
	set_reset(RESETS_PLL_SYS, 0);
	set_usb_pll();
	set_sys_pll();

	set_sys_clock_pll_sys();

	set_reset(RESETS_IO_BANK0, 0);
	set_reset(RESETS_UART0, 0);

	set_uart0_pinmux();

	set_pinfunc(LED_PIN, 5);

	SIO_BASE[9] = 1 << LED_PIN; // OE_SET

	uart_init();

	uart_rx_int_en();

	while(1) {
		delay(1000);
		uart_puts("hello!\r\n");
	}

	return 0;
}

static inline void set_uart0_pinmux(void) {
	set_pinfunc(0, GPIO_FUNC_UART); /* TX, PICO PIN 1 */
	set_pinfunc(1, GPIO_FUNC_UART); /* RX, PICO PIN 2 */
}

static inline void set_spi0_pinmux(void) {
	set_pinfunc(2, GPIO_FUNC_SPI); /* SCK, PICO PIN 4*/
	set_pinfunc(3, GPIO_FUNC_SPI); /* TX,  PICO PIN 5*/
	set_pinfunc(4, GPIO_FUNC_SPI); /* RX,  PICO PIN 6*/
	set_pinfunc(5, GPIO_FUNC_SPI); /* CS,  PICO PIN 7*/
}
