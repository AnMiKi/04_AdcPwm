#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "uart.h"

/******************************* CONSTANTS & ENUMS *******************************/

// Define LED and Button Pins
#define LED_PIN     PD2
#define BUTTON_PIN  PD3

// Define states
typedef enum {
    STATE_OFF = 0,
    STATE_ON,
    STATE_SLOW_BLINK,
    STATE_FAST_BLINK,
    STATE_RESET,
    STATE_COUNT // Helper to loop states
} State;

/******************************* GLOBAL VARIABLES *******************************/

volatile State currentState = STATE_OFF;
volatile uint8_t buttonLocked = 0;     // For debouncing
volatile int buttonPressCount = 0;
int debugMode = 0;

/******************************* FUNCTION DECLARATIONS **************************/

void print_state(const char* message);
char* int_to_string(int number);

/******************************* MAIN *******************************/

int main(void) {
    uart_init();
    uart_print("Debug Start\n");

    // LED pin as output
    DDRD |= (1 << LED_PIN);
    // Button pin as input
    DDRD &= ~(1 << BUTTON_PIN);

    // Configure INT1 (PD3 / D3) on rising edge
    EICRA |= (1 << ISC11) | (1 << ISC10);
    EIMSK |= (1 << INT1);
    sei(); // Enable global interrupts

    while (1) {
        switch (currentState) {
            case STATE_OFF:
                print_state("State: OFF\n");
                PORTD |= (1 << LED_PIN); 
                PORTD &= ~(1 << LED_PIN);
                _delay_ms(10);
                buttonLocked = 0;
                break;

            case STATE_ON:
                print_state("State: ON\n");
                PORTD |= (1 << LED_PIN);
                _delay_ms(10);
                buttonLocked = 0;
                break;

            case STATE_SLOW_BLINK:
                print_state("State: SLOW BLINK\n");
                PORTD ^= (1 << LED_PIN);
                _delay_ms(2000);
                buttonLocked = 0;
                break;

            case STATE_FAST_BLINK:
                print_state("State: FAST BLINK\n");
                PORTD ^= (1 << LED_PIN);
                _delay_ms(100);
                buttonLocked = 0;
                break;

            case STATE_RESET:
                print_state("State: RESET → OFF\n");
                PORTD &= ~(1 << LED_PIN);
                currentState = STATE_OFF;
                _delay_ms(100);
                buttonLocked = 0;
                break;

            default:
                currentState = STATE_OFF;
                break;
        }

        // Optional: Debug override
        if (debugMode) {
            PORTD |= (1 << LED_PIN);
            _delay_ms(100);
            buttonLocked = 0;
        }
    }
}

/******************************* FUNCTION IMPLEMENTATIONS ************************/

void print_state(const char* message) {
    if (buttonLocked) {
        uart_print(message);
    }
}

char* int_to_string(int number) {
    static char buffer[10];
    itoa(number, buffer, 10);
    return buffer;
}

/******************************* INTERRUPTS *******************************/

ISR(INT1_vect) {
    if (!buttonLocked) {
        buttonLocked = 1;
        currentState++;
        buttonPressCount++;

        //uart_print("Button Pressed\n");
        //uart_print("Total Presses: ");
        //uart_print(int_to_string(buttonPressCount));
        //uart_print("\n");

        if (currentState >= STATE_COUNT) {
            currentState = STATE_OFF;
        }
    }
}
