#include <stdint.h>
#include <stddef.h>

#include "protocall.h"

#define MARQUEE_SOF 'F'
#define ANIMATE_SOF 'A'
#define BRIGHTNESS_SOF 'B'
#define SPEED_SOF 'S'
#define BUF_SIZE 0x300

static uint16_t length = 0;
static uint8_t *framebuf_ptr = NULL;
static uint8_t brightness = 0xff;
static uint16_t speed = 0xffff;

static int marquee_append(uint8_t data, uint16_t *pos);
static int animate_append(uint8_t data, uint16_t *pos);
static int brightness_append(uint8_t data, uint16_t *pos);
static int speed_append(uint8_t data, uint16_t *pos);

int protocall_appended(uint8_t data){
	static uint8_t type = 0;
	static uint16_t pos = 0;

	if(pos == 0){
		type = data;
		pos++;
		switch(data){
			case MARQUEE_SOF:
			case ANIMATE_SOF:
			case BRIGHTNESS_SOF:
			case SPEED_SOF:
				return PROTOCALL_NOT_VALID;
			default:
				pos = 0;
				type = 0;
				return PROTOCALL_INVALID;
		}
	}

	switch(type){
		case MARQUEE_SOF:
			return marquee_append(data, &pos);
		case ANIMATE_SOF:
			return animate_append(data, &pos);
		case BRIGHTNESS_SOF:
			return brightness_append(data, &pos);
		case SPEED_SOF:
			return speed_append(data, &pos);
		default:
			return PROTOCALL_INVALID;
	}
}

static int marquee_append(uint8_t data, uint16_t *pos){
	static uint16_t expected_len = 0;
	static uint8_t rolling_checksum = 0;
	static uint8_t index = 0;
	static uint8_t framebuf[2][BUF_SIZE] = {0};
	int protocall_valid = PROTOCALL_NOT_VALID;

	if(index > 1)
		index = 0;

	if (*pos >= (expected_len + 4)) {
		*pos = 0;
		rolling_checksum = (1 + (~rolling_checksum));
		if (data == rolling_checksum) {
			framebuf_ptr = framebuf[index];
			// why is this +1?
			length = expected_len + 1;
			index++;
			rolling_checksum = 0;
			expected_len = 0;
			return PROTOCALL_VALID;
		} else {
			expected_len = 0;
			return PROTOCALL_INVALID;
		}
	}

	rolling_checksum += data;

	if (*pos == 1){
		expected_len = data << 8;
		(*pos)++;
		return protocall_valid;
	}

	if (*pos == 2){
		expected_len |= data;
		if(expected_len > BUF_SIZE) {
			expected_len = 0;
			rolling_checksum = 0;
			protocall_valid = PROTOCALL_INVALID;
			*pos = 0;
		} else {
			(*pos)++;
		}

		return protocall_valid;
	}

	framebuf[index][(*pos) - 3] = data;
	(*pos)++;

	return protocall_valid;
}

static int animate_append(uint8_t data, uint16_t *pos){
	(void)data;

	(*pos) = 0;
	return PROTOCALL_INVALID;
}

static int brightness_append(uint8_t data, uint16_t *pos){
	brightness = data;
	(*pos) = 0;
	return PROTOCALL_VALID;
}

static int speed_append(uint8_t data, uint16_t *pos){
	static uint8_t temp_speed;

	if (*pos == 1){
		temp_speed = data;
		(*pos)++;
		return PROTOCALL_NOT_VALID;
	}

	if (*pos == 2){
		speed = (temp_speed << 8) | data;
		*pos = 0;
		return PROTOCALL_VALID;
	}

	*pos = 0;
	return PROTOCALL_INVALID;
}

uint8_t protocall_getbrightness(void){
	return brightness;
}

uint16_t protocall_getspeed(void){
	return speed;
}

uint8_t* protocall_getdata(size_t *len){
	*len = length;
	return framebuf_ptr;
}
