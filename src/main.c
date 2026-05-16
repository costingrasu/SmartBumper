#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "i2c.h"
#include "lcd.h"

#define PULS_INITIAL_US  500  // Stare initiala servomotor
#define PULS_RIDICAT_US  800  // Stare finala servomotor
#define SERVO_PIN   PB1
#define BUZZER_PIN  PB4

volatile uint32_t systicks = 0;       
volatile uint32_t t_start = 0;
volatile uint32_t t_stop = 0;
volatile uint8_t measure_complete = 0;

#define DISTANCE_M 1  
#define LIMIT_SPEED_KMH 30.0

void GPIO_init(void) {
    // Configurare Buzzer ca Iesire
    DDRB |= (1 << BUZZER_PIN);
    PORTB &= ~(1 << BUZZER_PIN); 

    // Configurare Servo ca Iesire
    DDRB |= (1 << SERVO_PIN);
    PORTB &= ~(1 << SERVO_PIN);
}

void Interrupts_init(void) {
    // Configuram INT0 si INT1 pe front descrescator
    EICRA |= (1 << ISC01) | (1 << ISC11);
    EICRA &= ~((1 << ISC00) | (1 << ISC10));

    // Activam intreruperile hardware
    EIMSK |= (1 << INT0) | (1 << INT1);
}

void Timer1_init(void) {
    TCCR1A = 0;
    TCCR1B = (1 << WGM12); 
    TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 64
    OCR1A = 249; // Intrerupere la fiecare 1ms
    TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
    systicks++;
}

ISR(INT0_vect) {
    t_start = systicks;
    measure_complete = 0;
}

ISR(INT1_vect) {
    if (t_start > 0 && measure_complete == 0) { 
        t_stop = systicks;
        measure_complete = 1;
    }
}

// Trimite o serie de pulsuri pentru a forta servomotorul sa se deplaseze
void set_servo_angle(uint16_t pulse_us) {
    // Trimitem 40 de pulsuri (aprox. 800ms) pentru a asigura tranzitia mecanica completa
    for (int i = 0; i < 40; i++) {
        PORTB |= (1 << SERVO_PIN);   // Pin HIGH
        
        // Generam delay-ul variabil in microsecunde
        uint16_t delay_left = pulse_us;
        while (delay_left > 10) {
            _delay_us(10);
            delay_left -= 10;
        }
        
        PORTB &= ~(1 << SERVO_PIN);  // Pin LOW
        _delay_ms(19);               // Restul ferestrei de pana la 20ms
    }
}

int main(void) {
    GPIO_init();
    Timer1_init();
    Interrupts_init();
    i2c_init();
    lcd_init();

    sei(); // Activare intreruperi globale

    lcd_print("  Smart Bumper  ");
    lcd_set_cursor(1, 0);
    lcd_print("Sistem pornit...");
    
    // Aduce servomotorul in pozitia initiala la pornire
    set_servo_angle(PULS_INITIAL_US); 
    _delay_ms(1000);
    
    lcd_clear();
    lcd_print("Asteptare...");

    char buffer[16];

    while (1) {
        if (measure_complete) {
            uint32_t delta_t_ms = t_stop - t_start;
            
            if (delta_t_ms > 0) {
                float time_s = (float)delta_t_ms / 1000.0;
                float speed_ms = DISTANCE_M / time_s;
                float speed_kmh = speed_ms * 3.6;

                lcd_clear();
                lcd_print("Viteza:");
                lcd_set_cursor(1, 0);
                
                int vit_int = (int)speed_kmh;
                int vit_dec = (int)((speed_kmh - vit_int) * 10);
                sprintf(buffer, "%d.%d km/h", vit_int, vit_dec);
                lcd_print(buffer);

                if (speed_kmh > LIMIT_SPEED_KMH) {
                    lcd_clear();
                    lcd_print("LIMITA DEPASITA!");
                    lcd_set_cursor(1, 0);
                    lcd_print(buffer);

                    PORTB |= (1 << BUZZER_PIN); // Porneste buzzer
                    
                    // Ridicam bumperul
                    set_servo_angle(PULS_RIDICAT_US);
                    
                    _delay_ms(2000); // Mentinem pozitia ridicata 2 secunde
                    PORTB &= ~(1 << BUZZER_PIN); // Opreste sunetul
                } else {
                    _delay_ms(3000); // Daca e regulamentar, lasam doar afisajul pe ecran
                }
            }
            
            // Resetam flag-urile si timpii
            t_start = 0;
            t_stop = 0;
            measure_complete = 0;
            
            lcd_clear();
            lcd_print("Asteptare...");

            // Fortam servomotorul sa revina la pozitia initiala
            set_servo_angle(PULS_INITIAL_US);
            
        } else {
            PORTB |= (1 << SERVO_PIN);
            
            uint16_t delay_left = PULS_INITIAL_US;
            while (delay_left > 10) {
                _delay_us(10);
                delay_left -= 10;
            }
            
            PORTB &= ~(1 << SERVO_PIN);
            _delay_ms(19);
        }
    }

    return 0;
}