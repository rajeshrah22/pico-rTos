#include <stdint.h>
#include <stddef.h>

#include "max7219.h"
#include "spi.h"

static uint8_t tx_buf_ind = 0;
static const uint16_t *tx_buf = NULL;

static void max7219_callback(void) {
	if(tx_buf_ind >= 8)
		tx_buf_ind = 0;
	if(tx_buf == NULL)
		return;

	tx_buf_ind += spi_fill_tx_fifo(tx_buf + tx_buf_ind, 8 - tx_buf_ind);
}

void init_max7219(void) {
	static const unsigned max7219_init[] = {
		MAX7219_CMD(0xc, 0),
		MAX7219_CMD(0xf, 0),
		MAX7219_CMD(0xb, 7),
		MAX7219_CMD(0x9, 0),
		0
	};

	for(const unsigned* p = max7219_init; *p; p++) {
		spi_transfer(*p);
	}

	for(unsigned i = 1; i < 9; i++) {
		spi_transfer(MAX7219_CMD(i, 0));
	}

	spi_transfer(MAX7219_CMD(0xc, 1));
	max7219_set_brightness(0xf);


	spi_set_callback(max7219_callback);
//	spi_asign_tx_int(1);
	spi_transfer(0); // MAX7219 nop. trigger the interrupt
}

void max7219_set_frame(const uint16_t frame[8]) {
	tx_buf = frame;
}

void max7219_set_brightness(uint8_t brightness) {
	spi_transfer(MAX7219_CMD(0xa, brightness & 0xf));
}

void max7219_set_column(uint8_t digit, uint8_t value) {
	if(digit < 1 || digit > 8)
		return;
	spi_transfer(MAX7219_CMD(digit + 1, value));
}

void max7219_gen_frame(const uint8_t data[8], uint16_t frame[8]) {
	for(int i = 0; i < 8; i++) {
		frame[i] = MAX7219_CMD(i + 1, data[i]);
	}
}

void max7219_gen_frame_cir(const uint8_t *data, uint16_t frame[8], size_t start, size_t len) {
	size_t offset;

	for(size_t i = 0; i < 8; i++) {
		offset = start + i;

		if(offset >= len)
			offset -= len;

		frame[i] = MAX7219_CMD(i + 1, data[offset]);
	}
}
