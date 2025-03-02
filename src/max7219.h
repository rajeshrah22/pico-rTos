#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>
#include <stddef.h>

#define MAX7219_CMD(a, b)  (((a) << 8u) | (b))

void init_max7219(void);
void max7219_set_brightness(uint8_t brightness);
void max7219_set_column(uint8_t digit, uint8_t value);
void max7219_set_frame(const uint16_t frame[8]);
void max7219_gen_frame(const uint8_t data[8], uint16_t frame[8]);
void max7219_gen_frame_cir(const uint8_t *data, uint16_t frame[8], size_t start, size_t len);

#endif // MAX7219_H
