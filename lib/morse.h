#include "ece198.h"


//hardware pin variables 
#define p1_red_led GPIO_PIN_6
#define p1_green_led GPIO_PIN_7
#define p1_button GPIO_PIN_8
#define p1_button_1_port GPIOB
#define p1_buzzer GPIO_PIN_10
#define p2_red_led GPIO_PIN_0
#define p2_green_led GPIO_PIN_1
#define p2_button GPIO_PIN_9
#define p2_buzzer GPIO_PIN_0

#define MORSE_TIME_UNIT 250

struct morseString {
    unsigned short length;
    char text[];
};

void str_append(char str[], const char suffix[], unsigned short* index);

struct morseString *char_to_morse(char message[], unsigned short size);

void blink_LED(unsigned int time, uint16_t LED_pin);

void print_morse(struct morseString* message, uint16_t LED_pin);

int get_user_input(struct morseString* required_input, uint16_t button_pin, uint16_t buzzer_pin);