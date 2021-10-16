#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdarg.h>

// LiquidCrystal
#include <LiquidCrystal.h>
#define B1 9
#define B0 8
#define D4 4
#define D5 5
#define D6 6
#define D7 7

// general defines
#define SET_BIT(reg, pin)           (reg) |= (1 << (pin))
#define CLEAR_BIT(reg, pin)         (reg) &= ~(1 << (pin))
#define WRITE_BIT(reg, pin, value)  (reg)=(((reg) & ~(1 << (pin))) | ((value) << (pin)))
#define BIT_VALUE(reg, pin)         (((reg) >> (pin)) & 1)
#define BIT_IS_SET(reg, pin)        (BIT_VALUE((reg),(pin))==1)

//////////////////////////
//CODE FOR SLAVE 
//////////////////////////

// initialize the library by associating any needed LCD interface pin
// with the Arduino pin number it is connected to
uint8_t rs = B1, en = B0;

LiquidCrystal lcd(rs, en, D4, D5, D6, D7);


const int f_qs_nm = 5; //Number of questions in friends_questions

char friend_qs[f_qs_nm][64] = {
                         "Better Cook",
                         "Better Driver",
                         "Cleaner",
                         "More on time",
                         "Faster Texter"
                     };


char aswrs[2][8] = {
                         "You",
                         " Me"
                     };



void start(void);
void display_question(void);
void display_responder(void);
void display_response(int ans);
void display_response_blink(int ans, int times, int delay);
void question_cycle(void);
void display_answer(int a1, int a2);
void display_loss(void);
void display_win(void);
void __init__(void);

int score = 0;
int obj = 3; //Currenly hard coded, need to be dynamic later

uint8_t counter_you = 0;
uint8_t counter_me = 0;
uint8_t is_pressed_you = 0; //A0
uint8_t is_pressed_me = 0; //A1

int main(void) {

    start();
    _delay_ms(1000);

    uart_init();
    
    char buffer[64];
    uart_get_string(buffer, 20);
    uart_put_string(buffer);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(buffer);



    while (1) {

        //loop_ch();
        _delay_ms(500);


    }
}


void __init__(void){ //Not Initalisation but the british way of say isnt it

CLEAR_BIT(DDRC, 0); //Set pin A0 and A1 to input
CLEAR_BIT(DDRC, 1);

TCCR0A = 0;
TCCR0B = 4;
TIMSK0 = 1;


sei(); //Interupts

}


void start(void) {


    __init__();

    // Initialises LCD
    lcd.begin(16, 2);


    // Print a message to the LCD 
    lcd.print("WELCOME TO:");
    _delay_ms(1000);

    lcd.clear();

  

    lcd.setCursor(5, 0);
    lcd.print("You or");
    lcd.setCursor(7, 1);
    lcd.print("Me?");

}

//UART FUNCTIONS

void uart_init(void) {

    UBRR0 = F_CPU / 16 / 9600 - 1;
    UCSR0A = 0;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (3 << UCSZ00);
}


//BYTE GRAB AND RECEIVE

void uart_putbyte(unsigned char data) {
   
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

int uart_getbyte(unsigned char* buffer) {
    // If receive buffer contains data...
    if (UCSR0A & (1 << RXC0)) {
        // Copy received byte from UDR0 into memory location (*buffer)
        *buffer = UDR0;
        // 
        return 1;
    }
    else {
        return 0;
    }
}

//STRING CONSTRUCT AND DECONSTRUCT
void uart_put_string(char s[]) {
    int i = 0;
    while (s[i] != 0) {
        uart_putbyte(s[i]);
        i++;
    }
    uart_putbyte(0);
}



void uart_get_string(char buff[], int buff_len) {
    int i = 0;
    unsigned char ch;
  	
    for (;;) {
        while (!uart_getbyte(&ch)) {}
        if (ch == 0) {
            break;
        }
        if (i < (buff_len - 1)) { // < > look for inequalitiies 
            buff[i] = ch;
            i++;
        }
    }
    buff[i] = 0;

}