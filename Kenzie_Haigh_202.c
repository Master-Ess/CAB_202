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

char friend_qs[5][64] = {
                         "Better Cook",
                         "Better Driver",
                         "Cleaner"
                         "More on time"
                         "Faster Responder"
                     };




void start(void);
void display_question(void);




int main(void) {

    start();
    _delay_ms(1000);
    display_question(1);

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

    lcd.setCursor(0, 0);
    lcd.print(friend_qs[num]);



}


