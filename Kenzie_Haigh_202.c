#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/// include the library code:
#include <LiquidCrystal.h>
#define B1 9
#define B0 8
#define D4 4
#define D5 5
#define D6 6
#define D7 7

// initialize the library by associating any needed LCD interface pin
// with the Arduino pin number it is connected to
uint8_t rs = B1, en = B0;

LiquidCrystal lcd(rs, en, D4, D5, D6, D7);


int f_qs_nm = 5 //Number of questions in friends_questions

char friend_qs[f_qs_nm][64] = {
                         "Better Cook",
                         "Better Driver",
                         "Cleaner"
                         "More on time"
                         "Faster Texter"
                     };




void start(void);
void display_question(void);
void display_responder(void);




int main(void) {

    start();
    _delay_ms(1000);
    display_question(rand() % f_qs_nm);
    display_responder();

    while (1) {

        //loop_ch();
        _delay_ms(500);


    }
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

void display_question(int num){

    lcd.clear();

    lcd.setCursor(1, 0);
    lcd.print(friend_qs[num]);
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