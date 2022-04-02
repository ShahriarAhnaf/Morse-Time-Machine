#include "morse.h"
#include <stdlib.h>

#include "ece198.h"

//letters in morse from A - Z
static const char MORSE_LETTER[26][6] = {
    {".-"}, {"-..."}, {"-.-."}, {"-.."}, {"."},
    {"..-."}, {"--."}, {"...."}, {".."}, {".---"},
    {"-.-"}, {".-.."}, {"--"}, {"-."}, {"---"},
    {".--."}, {"--.-"}, {".-."}, {"..."}, {"-"},
    {"..-"}, {"...-"}, {".--"}, {"-..-"}, {"-.--"}, {"--.."}
};

static const char MORSE_DIGIT[10][6] = {
    {"-----"},{".----"},{"..---"},{"...--"},{"....-"},
    {"....."},{"-...."},{"--..."},{"---.."},{"----."}
};


//appends string literal to char array, automatically increments index var
void str_append(char str[], const char suffix[], unsigned short* index) {
    for(unsigned short i = 0; suffix[i] != '\0'; ++i) {
        str[(*index)++] = suffix[i];
    }

    str[(*index)++] = ' ';
}


struct morseString *char_to_morse(char message[], unsigned short size) {
    //Create an array with enough space to fit array of largest morse character
    struct morseString *str = malloc(sizeof(struct morseString) 
                            + sizeof(char) * size * 6);
    (*str).length = size*6;

    unsigned short index = 0;
    
    for(unsigned short i = 0; message[i] != '\0'; ++i) {
        char c = message[i];
        //letters
        if (c >= 65 && c <= 90) {
            str_append((*str).text, MORSE_LETTER[c - 65], &index);
        }
        //numbers
        else if(c >= 48 && c <= 57) {
            str_append((*str).text, MORSE_DIGIT[c - 48], &index);
        }
    }

    return str;

}

void blink_LED(unsigned int time, uint16_t LED_pin) {
    HAL_GPIO_WritePin(GPIOA, LED_pin, 1);
    HAL_Delay(time);
    HAL_GPIO_WritePin(GPIOA, LED_pin, 0);
}

void print_morse(struct morseString* message, uint16_t LED_pin) {
    for (int i = 0; i < (*message).length; ++i) {
        switch((*message).text[i]) {
            case '.':
                blink_LED(MORSE_TIME_UNIT, LED_pin);
                break;
            case '-':
                blink_LED(MORSE_TIME_UNIT*3, LED_pin); 
                break;
            case ' ':
                HAL_GPIO_WritePin(GPIOA, LED_pin, 0);
                HAL_Delay(MORSE_TIME_UNIT * 3);
                break;
            default:
                break;
        }
        HAL_Delay(MORSE_TIME_UNIT);
    }
}

int get_user_input(struct morseString* required_input, uint16_t button_pin, uint16_t buzzer_pin) {
    for(int i = 0; i < (*required_input).length; ++i) {
        char input;
        unsigned int start_time;

        //Error handling for invalid input
        if ((*required_input).text[i] == '.' || (*required_input).text[i] == '-') {
            //wait for user to press button
            while(!HAL_GPIO_ReadPin(GPIOB, button_pin)) {}
            start_time = HAL_GetTick();
            //buzz THE LED WHILE PRESSING
            HAL_GPIO_WritePin(GPIOB, p1_buzzer, 1);
            
            //wait for user to stop pressing button
            while (HAL_GPIO_ReadPin(GPIOB, button_pin)) {}
            
            HAL_GPIO_WritePin(GPIOB, p1_buzzer, 0);

            unsigned int time = HAL_GetTick() - start_time;
            
            SerialPuts("loop\n");

            // check for dot or dash
            if(time >= MORSE_TIME_UNIT * 2) {
                input = '-';
            }
            else {
                input = '.';
            }

            
            // if the input no exist
            if((*required_input).text[i] != '.' && (*required_input).text[i] != '-'){
                SerialPutc('1');
                return 1;
            }
            if ((*required_input).text[i] != input) {
                SerialPutc('f');
                return 0;
            } 
        }
    }
    return 1;
}

//player = 0 => Player 1
//player = 1 => Player 2
void ask_question(int player, struct morseString* question, struct morseString* answer) {
    struct morseString *FAIL_MESSAGE = char_to_morse("F", 1);

    uint16_t green_led = player ? p2_green_led : p1_green_led;
    uint16_t red_led = player ? p2_red_led : p1_red_led;
    uint16_t button = player ? p2_button : p1_button;
    uint16_t buzzer = player ? p2_buzzer : p1_buzzer;

    //only print the question if its player 1 who needs the question
    if(!player){
    print_morse(question, green_led);
    }
    while(!get_user_input(answer, button, buzzer)) {
        print_morse(FAIL_MESSAGE, red_led);
    }
}

