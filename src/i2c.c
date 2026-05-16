#include "i2c.h"

void i2c_init(void) {
    // Setam frecventa SCL la 100 kHz pentru un ceas de 16MHz
    // TWBR = ((F_CPU / F_SCL) - 16) / 2 = ((16000000 / 100000) - 16) / 2 = 72
    TWBR = 72;
    // Activam TWI
    TWCR = (1 << TWEN);
}

void i2c_start(void) {
    // Trimitem conditia de START
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    // Asteptam finalizarea operatiei
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
    // Trimitem conditia de STOP
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void i2c_write(uint8_t data) {
    // Incarcam datele in registrul de date
    TWDR = data;
    // Pormim transmisia
    TWCR = (1 << TWINT) | (1 << TWEN);
    // Asteptam finalizarea transmisiei
    while (!(TWCR & (1 << TWINT)));
}