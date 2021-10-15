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

//Error 1 == No Assigning of response
//Error 2 == No user inpuit found for response

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

int score = 0;

int main(void) {

    start();
    _delay_ms(1000);
    question_cycle();
    



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

void display_answer(int a1, int a2){
    lcd.clear();

    if (a1 != a2){
        lcd.setCursor(5, 0);
        lcd.print("Same!");
        score++;
    }
    else{
        lcd.setCursor(3, 0);
        lcd.print("Different!");
    }
    _delay_ms(2000);
}

void question_cycle(void){

    int i = 0;
    while (i < f_qs_nm){
        display_question(i); // currently always gives question 2???
        display_responder();
        _delay_ms(5000);
        display_response_blink(0, 3, 600);
        _delay_ms(2000);
        display_answer(0, 1);
        i++;
    }

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Better Luck");
    lcd.setCursor(2,1);
    lcd.print("Next Time!");

    _delay_ms(2000);

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Your Score:");
    lcd.setCursor(6, 1);
    char s_score[3];
    itoa(score,s_score,10);
    lcd.print(s_score );
    lcd.print("/3");
}