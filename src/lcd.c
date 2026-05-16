#include "lcd.h"
#include "i2c.h"
#include <util/delay.h>

static void lcd_send(uint8_t value, uint8_t mode) {
    uint8_t high_nibble = value & 0xF0;
    uint8_t low_nibble = (value << 4) & 0xF0;
    
    // mode = 1 pt Date (RS=1), mode = 0 pt Comanda (RS=0)
    // Bitul 3 este lumina de fundal (Backlight = 1)
    uint8_t data_flags = mode | 0x08; 

    i2c_start();
    i2c_write(LCD_ADDR << 1); // Adresa de scriere (bit 0 = 0)
    
    // Trimitem High Nibble
    i2c_write(high_nibble | data_flags | 0x04); // EN = 1
    i2c_write(high_nibble | data_flags);        // EN = 0
    
    // Trimitem Low Nibble
    i2c_write(low_nibble | data_flags | 0x04);  // EN = 1
    i2c_write(low_nibble | data_flags);         // EN = 0
    
    i2c_stop();
}

void lcd_send_cmd(uint8_t cmd) {
    lcd_send(cmd, 0);
}

void lcd_send_data(uint8_t data) {
    lcd_send(data, 1);
}

void lcd_init(void) {
    _delay_ms(50);
    lcd_send_cmd(0x33);
    lcd_send_cmd(0x32);
    lcd_send_cmd(0x28); // 4-bit mode, 2 lines, 5x8 font
    lcd_send_cmd(0x0C); // Display ON, Cursor OFF
    lcd_send_cmd(0x01); // Clear display
    _delay_ms(2);
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    _delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_send_cmd(0x80 | (col + row_offsets[row]));
}

void lcd_print(const char* str) {
    while (*str) {
        lcd_send_data((uint8_t)(*str));
        str++;
    }
}