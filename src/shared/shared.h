#include <stdint.h>

/* Shared structure */
struct Message {
	uint16_t x;
	uint16_t y;
	uint16_t colour;
};

extern const int messageSize;

extern const int screenMinX;
extern const int screenMaxX;
extern const int screenMinY;
extern const int screenMaxY;