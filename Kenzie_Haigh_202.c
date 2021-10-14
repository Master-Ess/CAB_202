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


//=================================================
//=================================================
//=================================================
//                Example code
//=================================================
//=================================================
//=================================================


uint8_t bmp0[8] = { 0b00010010,
                    0b00001010,
                    0b00001001,
                    0b00000100,
                    0b00000100,
                    0b00010010,
                    0b00001010,
                    0b00001001 };
uint8_t bmp1[8] = { 0b00010101,
                    0b00010101,
                    0b00010101,
                    0b00010101,
                    0b00010101,
                    0b00010101,
                    0b00010101,
                    0b00010101 };
uint8_t bmp2[8] = { 0b00001001,
                    0b00001010,
                    0b00010010,
                    0b00000100,
                    0b00000100,
                    0b00001001,
                    0b00001010,
                    0b00010010 };
uint8_t bmp3[8] = { 0b00000000,
                    0b00011111,
                    0b00000000,
                    0b00011111,
                    0b00000000,
                    0b00011111,
                    0b00000000,
                    0b00011111 };


void setup_lcd(void);
void loop(void);


int main(void) {

    setup_lcd();

    while (1) {

        //loop_ch();
        _delay_ms(500);


    }
}


void setup_lcd(void) {

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

    //register 4 new character bitmaps as character codes 0-3
  
    //lcd.blink();

}


void loop_ch(void) {
    static uint8_t frame = 0;

    //write the custom character bitmaps one at a time to make an animation
    if (frame == 0) {
        lcd.setCursor(1, 0);
        lcd.write((unsigned char)0);
        lcd.write((unsigned char)1);
        lcd.setCursor(1, 1);
        lcd.write((unsigned char)2);
        lcd.write((unsigned char)3);
    }
    else if (frame == 1) {
        lcd.setCursor(1, 0);
        lcd.write((unsigned char)1);
        lcd.write((unsigned char)2);
        lcd.setCursor(1, 1);
        lcd.write((unsigned char)3);
        lcd.write((unsigned char)0);
    }
    else if (frame == 2) {
        lcd.setCursor(1, 0);
        lcd.write((unsigned char)2);
        lcd.write((unsigned char)3);
        lcd.setCursor(1, 1);
        lcd.write((unsigned char)0);
        lcd.write((unsigned char)1);
    }
    else if (frame == 3) {
        lcd.setCursor(1, 0);
        lcd.write((unsigned char)3);
        lcd.write((unsigned char)0);
        lcd.setCursor(1, 1);
        lcd.write((unsigned char)1);
        lcd.write((unsigned char)2);
    }

    if (frame % 2 == 0) {
        lcd.scrollDisplayLeft();
    }
    else {
        lcd.scrollDisplayRight();
    }
    frame = (frame + 1) % 4;
}