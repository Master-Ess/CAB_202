#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdarg.h>
#include <EEPROM.h>


// LiquidCrystal
#include <LiquidCrystal.h>
#define B1 9
#define B0 8
#define D4 4
#define D5 2
#define D6 3
#define D7 7



// general defines
#define SET_BIT(reg, pin)           (reg) |= (1 << (pin))
#define CLEAR_BIT(reg, pin)         (reg) &= ~(1 << (pin))
#define WRITE_BIT(reg, pin, value)  (reg)=(((reg) & ~(1 << (pin))) | ((value) << (pin)))
#define BIT_VALUE(reg, pin)         (((reg) >> (pin)) & 1)
#define BIT_IS_SET(reg, pin)        (BIT_VALUE((reg),(pin))==1)


//////////////////////////
//CODE FOR MASTER
//////////////////////////

#define FREQ (16000000.0)
#define PRESCALE (1024.0)

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
int obj = 3; 


uint8_t counter = 0;
uint8_t is_pressed = 0;

volatile int overflow_counter = 0;
 
ISR(TIMER2_OVF_vect) {
    overflow_counter ++;
}

int main(void) {


    void uart_init(void);
    void uart_put_string(char s[]);

    pwm_init(256);
    start();
    _delay_ms(1000);


    uart_init();
    
   
    
  	//PL2---------
    uart_put_string("Player 2");

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Player 1");

    _delay_ms(1000);

     analog_init(256);
  	
  	uint16_t divider = 256;
	pwm_init(divider);


     question_cycle(); //Comment out if you want to test the osscolosope first -- wow spelling
    
   
	for (;;) {
        char temp_buf[64];
        uint16_t dpot = (analog_read(4) / 3.8);
        itoa(dpot, (char*)temp_buf, 10);
    	Serial.println();
		Serial.print(temp_buf);
      	int duty_cycle = (dpot);
		pwm_write(duty_cycle);
        _delay_ms(250);
       	dpot = (analog_read(4));

        Serial.println();
        Serial.print(overflow_counter);
	}
  
   

}


void __init__(void){ //Not Initalisation but the british way of say isnt it

CLEAR_BIT(DDRC, 0); //Set pin A0 through A3 to input
CLEAR_BIT(DDRC, 1);
CLEAR_BIT(DDRC, 2);
CLEAR_BIT(DDRC, 3);
CLEAR_BIT(DDRC, 4); //PINS for period and duty cycle change
CLEAR_BIT(DDRC, 5); //ISR test


SET_BIT(DDRB, 5); // PIN for LED - IDK its an afterthought 

TCCR2A = 0;
TCCR2B = 4;
TIMSK2 = 1;


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

void display_question(int num){

    void uart_put_string(char s[]);

    lcd.clear();
    
    uart_put_string("81"); //Display Question
	
    char payload[5];
    itoa(num,payload,10); //Covert Int to char for transmission

    uart_put_string(payload);


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

    void uart_put_string(char s[]);

    lcd.clear();
    if (a1 == 3){
        lcd.setCursor(5, 0);
        lcd.print("Time!");
        uart_put_string("3");
    }
    else if (a1 != a2){
        lcd.setCursor(5, 0);
        lcd.print("Same!");
        uart_put_string("1");

        SET_BIT(PORTB, PB5); //LED

        score++;

        pwm_write(score * 86);
    }
    else{
        lcd.setCursor(3, 0);
        lcd.print("Different!");
        uart_put_string("2");
    }
    _delay_ms(2000);
}

void display_loss(void){

    void uart_put_string(char s[]);

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
    uart_put_string("85");
    uart_put_string(s_score);
    lcd.print(s_score );
    lcd.print("/3");
}

void display_win(void){
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Well Done!");
}

void question_cycle(void){

    void uart_put_string(char s[]);

   

    int i = 0;
    while (i < f_qs_nm){
        CLEAR_BIT(PORTB, PB5);
        display_question(i);
        display_responder();
      

        int response_a = 2; //Set to garbage, just for my peace of mind lol
        int response_b = 2;
        overflow_counter = 0;
        for (;;){



            if (response_a == 2){
            if (BIT_IS_SET(PINC, 0)){
                response_a = 0;
                display_response(response_a);

            }
            if (BIT_IS_SET(PINC, 1)){
                response_a = 1;
                display_response(response_a);

            }
            }

            if (response_b == 2){
            if (BIT_IS_SET(PINC, 3)){
                response_b = 0;
                uart_put_string("82");
                uart_put_string("0");

            }
            if (BIT_IS_SET(PINC, 2)){
                response_b = 1;
                uart_put_string("82");
                uart_put_string("1");

            }
            }
            double time = ( overflow_counter * 256.0 + TCNT0 ) * PRESCALE  / FREQ;

            if (time > 130){ //~10 seconds
                response_a = 3;
                response_b = 3;
                break;

            }

            if (response_a != 2 && response_b != 2){
                break;
            }



            _delay_ms(10); //Make it chug less but could also mean that it misses button presses
        }

        
        _delay_ms(1000);

        uart_put_string("83");
        
        display_answer(response_a, response_b);

        if (score == obj){
            display_win();
            uart_put_string("84");
            break;
        }
        if (i == (f_qs_nm - 1)){
            display_loss();
            
            break;
        }

        i++;
    }
}

//ADC --> Anyone else alawys think ACDC?

void analog_init(uint8_t division_factor) {
	// INSERT CODE

	ADMUX = 0b01000000;

	ADCSRA = 0b00000000;

	ADCSRA = ((1 << ADEN)); //EDEN

	CLEAR_BIT(ADCSRA, ADPS0);
	CLEAR_BIT(ADCSRA, ADPS1);
	CLEAR_BIT(ADCSRA, ADPS2);

	switch (division_factor)
	{
	case 2:
		SET_BIT(ADCSRA, ADPS0);
		CLEAR_BIT(ADCSRA, ADPS1);
		CLEAR_BIT(ADCSRA, ADPS2);
		break;
	case 4:
		CLEAR_BIT(ADCSRA, ADPS0);
		SET_BIT(ADCSRA, ADPS1);
		CLEAR_BIT(ADCSRA, ADPS2);
		break;
	case 8:
		SET_BIT(ADCSRA, ADPS0);
		SET_BIT(ADCSRA, ADPS1);
		CLEAR_BIT(ADCSRA, ADPS2);
		break;
	case 16:
		CLEAR_BIT(ADCSRA, ADPS0);
		CLEAR_BIT(ADCSRA, ADPS1);
		SET_BIT(ADCSRA, ADPS2);
		break;
	case 32:
		SET_BIT(ADCSRA, ADPS0);
		CLEAR_BIT(ADCSRA, ADPS1);
		SET_BIT(ADCSRA, ADPS2);
		break;
	case 64:
		CLEAR_BIT(ADCSRA, ADPS0);
		SET_BIT(ADCSRA, ADPS1);
		SET_BIT(ADCSRA, ADPS2);
		break;
	case 128:
		SET_BIT(ADCSRA, ADPS0);
		SET_BIT(ADCSRA, ADPS1);
		SET_BIT(ADCSRA, ADPS2);
		break;
	default:
		CLEAR_BIT(ADCSRA, ADPS0);
		CLEAR_BIT(ADCSRA, ADPS1);
		CLEAR_BIT(ADCSRA, ADPS2);
		break;
	}

	


}

uint16_t analog_read(uint8_t channel) {
	ADMUX = ADMUX ^ channel;

	
	ADCSRA |= (1 << ADSC);

	
#if !__AMS__
	while (ADCSRA & (1 << ADSC));
#endif

	return (ADC);
}



void ADC_display(void){

uint16_t dpot = analog_read(4); //duty cycle
uint16_t ppot = analog_read(5); //period

 int duty_per = 1000*dpot/1024;
 int freq = 1000/ppot;  //NOT NEEDED

 int high = duty_per*ppot/1024;
 int low = ppot - high;

for (int t; t < 20 ; t++){

    SET_BIT(PIND,2);
    _delay_ms(high);
    CLEAR_BIT(PIND,2);
    _delay_ms(low);


    Serial.println();
    Serial.print("frequency ");
    Serial.print(freq);
    Serial.println(" hertz");
    Serial.print("period ");
    Serial.print(ppot);
    Serial.println(" miliseconds");
    Serial.print("duty % ");
    Serial.println(duty_per);
}

}

//PWM FUNCTIONS

void pwm_init(uint16_t division_factor) {

	//  Configure the digital I/O pin corresponding to OCR0A for output. 
	//  Modify at most one pin in the DDR. DDRD 6
	SET_BIT(DDRD, 6);
	//  Update the value of TCCR0A so that register OC0A will clear on compare 
	//  match. //TCCR0A = 0b10000000;
	TCCR0A = 0b10000000;
	//  Update TCCR0B to disable Force Output Compare functionality.TCCR0B = 0b11000000;
	TCCR0B = 0b11000000;
	//  Update TCCR0B to ensure that the clock pre-scaler matches the 
	//  designated division factor.
	CLEAR_BIT(TCCR0B, CS00);
	CLEAR_BIT(TCCR0B, CS01);
	CLEAR_BIT(TCCR0B, CS02);

	//TCCR0B = (division_factor << CS02) | (division_factor << CS01) | (division_factor << CS00);

	switch (division_factor)
	{
	case 1:
		SET_BIT(TCCR0B, CS00);
		CLEAR_BIT(TCCR0B, CS01);
		CLEAR_BIT(TCCR0B, CS02);
		break;
	case 8:
		CLEAR_BIT(TCCR0B, CS00);
		SET_BIT(TCCR0B, CS01);
		CLEAR_BIT(TCCR0B, CS02);
		break;
	case 64:
		SET_BIT(TCCR0B, CS00);
		SET_BIT(TCCR0B, CS01);
		CLEAR_BIT(TCCR0B, CS02);
		break;
	case 256:
		CLEAR_BIT(TCCR0B, CS00);
		CLEAR_BIT(TCCR0B, CS01);
		SET_BIT(TCCR0B, CS02);
		break;
	case 1024:
		SET_BIT(TCCR0B, CS00);
		CLEAR_BIT(TCCR0B, CS01);
		SET_BIT(TCCR0B, CS02);
		break;
	default:
		CLEAR_BIT(TCCR0B, CS00);
		CLEAR_BIT(TCCR0B, CS01);
		CLEAR_BIT(TCCR0B, CS02);
		break;
	}
	
	SET_BIT(TCCR0A, WGM00);
	SET_BIT(TCCR0A, COM0A1);
	SET_BIT(TCCR0A, WGM01);
	CLEAR_BIT(TCCR0A, COM0A0);



	CLEAR_BIT(TCCR0B, FOC0B);
	CLEAR_BIT(TCCR0B, FOC0A);
	CLEAR_BIT(TCCR0B, WGM02);

}
void pwm_write(uint8_t duration) {
	OCR0A = duration;
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


//ISR CODE --> NEED CHECK if __AMS__ for TIM/TIMER compatoblity. Nice spelling



