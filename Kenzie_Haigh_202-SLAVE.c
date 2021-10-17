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
    
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(buffer);
    _delay_ms(500);
    for (;;){
    uart_get_string(buffer, 20);
    
  	int compare = atoi(buffer);

    if (compare == 81){
        uart_get_string(buffer,20);

        int dq = atoi(buffer);

        display_question(dq);
        display_responder();
    }

    if (compare == 82){
        uart_get_string(buffer, 20);
        int asw = atoi(buffer);

        display_response(asw);
    }

    if (compare == 83){

        uart_get_string(buffer, 20);
        int a1 = atoi(buffer);


        display_answer(a1);
    }

    if (compare == 84){

        display_win();

    }

    if (compare == 85){
        
        uart_get_string(buffer, 20);
        int score = atoi(buffer);
         
        display_loss();

    }

    _delay_ms(10);
    }
}




void display_question(int num){

    lcd.clear();


    char *question = friend_qs[num];
    int question_start = 7 - (strlen(question)/2); //AUTO CENTER 

    lcd.setCursor(question_start, 0);
    lcd.print(question);
    lcd.print("?");



}


void display_responder(void){
    lcd.setCursor(1,1);
    lcd.print("You");

    lcd.setCursor(8,1);
    lcd.print("/");

    lcd.setCursor(12, 1);
    lcd.print("Me");
}


void display_response(int ans){
    lcd.clear();

    lcd.setCursor(6, 1);
    lcd.print(aswrs[ans]);
    
}

void display_response_blink(int ans, int times, int delay){

    display_response(ans);

    int i = 0;

    while (i <= times){
    _delay_ms(delay);
    lcd.clear();
    _delay_ms(delay);
    display_response(ans);
    i++;
    }
    
}

void display_answer(int a1){
    lcd.clear();

    if (a1 == 1){
        lcd.setCursor(5, 0);
        lcd.print("Same!");
        score++;
    }
    else{
        lcd.setCursor(3, 0);
        lcd.print("Different!");
    }
    
}

void display_loss(void){
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Better Luck");
    lcd.setCursor(2,1);
    lcd.print("Next Time!");

    _delay_ms(2000);

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Your Score:");
    lcd.setCursor(6, 1);
    char s_score[3];
    itoa(score,s_score,10);
    lcd.print(s_score );
    lcd.print("/3");
}

void display_win(void){
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Well Done!");
}


void start(void) {


   

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