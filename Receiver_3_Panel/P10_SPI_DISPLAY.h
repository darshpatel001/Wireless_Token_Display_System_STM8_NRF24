#ifndef P10_SPI_DISPLAY_H
#define P10_SPI_DISPLAY_H

#include <stdint.h>
#include <SPI.h>

// Panel Configuration - CORRECTED
#define NUM_PANELS   3
#define PANEL_WIDTH  32  // 32 pixels wide per panel
#define PANEL_HEIGHT 16  // 16 pixels high per panel
#define P10_WIDTH    (PANEL_WIDTH * NUM_PANELS)  // 96 pixels total width
#define P10_HEIGHT   PANEL_HEIGHT                // 16 pixels total height

// Pin Definitions
#define P10_ENABLE  15  // PC4
#define P10_A       13  // PD3  
#define P10_B       14  // PD2
#define P10_LAT     12  // PC3

// Function Prototypes
void P10_init();
void P10_clear();
void P10_refresh();
void P10_draw_digit(int digit, int x, int y);
void P10_display_number(int number);
void P10_display_number_zone(uint8_t panel, int number);
void setPixel(uint8_t x, uint8_t y, uint8_t state);

#endif