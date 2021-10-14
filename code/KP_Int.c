#include <mega32.h>
#include <alcd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>


// Declare your global variables here
flash char Pattern[4] = {0xFE, 0xFD, 0xFB, 0xF7};
flash char key_number [4][4]={'7', '8', '9', '/',
                              '4', '5', '6', '*',
                              '1', '2', '3', '-',
                              'C', '0', '=', '+'};   
char ch;
char  buffer[4];
char num_holder[4];
char nuller[4];                              
                                                                                                   
bool  new_game = true;
bool active_session = false;

int target;
int current_number = 0;
int num_index = 0;
int no_of_digits = 2;
int rand_range = 6;
int seed_time;
int num_counter = 0;

float  error = 0.0;



// External Interrupt 2 service routine
interrupt [EXT_INT2] void ext_int2_isr(void)
{
// Place your code here
    char i, column = 4;           
    for (i=0; i<4; i++)
    {
       PORTD = Pattern[i]; 
       
       if (PIND.4 == 0)
       {
           column = 0;
           break;
       } 

       if (PIND.5 == 0)
       {
           column = 1;
           break;
       } 

       if (PIND.6 == 0)
       {
           column = 2;
           break;
       } 

       if (PIND.7 == 0)
       {
           column = 3;
           break;
       }      
    }
    
    ch = key_number[i][column];  

    if (new_game==false && ch == '+') { 
            ch = '';   
            lcd_clear();    
            lcd_gotoxy(0,1); 
            lcd_puts("Restart Game ...");
            delay_ms(1500);
            num_index = 0;
            new_game = true;
            active_session = false;
            lcd_clear();
    }       

 
     if (ch != 'b' && active_session) {
            if (ch == '0' || ch == '1' ||  ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9') {     
                    lcd_gotoxy(num_index,0);
                    lcd_putchar(ch);     
                    
                    num_holder[num_index] = ch;       
                    num_counter++;                 
                    num_index++;     
                    
                    current_number = atoi(num_holder);     // convert to int                                                                                                                                                     
                                                  
                    if (target >= current_number)  error = target -   current_number;
                    else  error = current_number - target;       
                                        
                    if (error == 0) {  
                        lcd_gotoxy(0,1); 
                        lcd_puts("You Got It!     ");  
                        lcd_gotoxy(0,2); 
                        lcd_puts("----------------");
                        lcd_gotoxy(0,3); 
                        sprintf(buffer, "Answer Is: %d", target);                                                                                    
                        lcd_puts(buffer);                                                                                               
                    }       
                    else if (error <= 3) {    
                        lcd_gotoxy(0,1); 
                        lcd_puts("So Close!     ");                                   
                    }
                   else if ( 3 < error &&  error <= 10 ) {       
                        lcd_gotoxy(0,1); 
                        lcd_puts("Close!       ");
                   }    
                   if (10 < error &&  error <= 20) {
                        lcd_gotoxy(0,1); 
                        lcd_puts("Far!        ");                   
                   }
                   else if (20 < error) {
                        lcd_gotoxy(0,1); 
                        lcd_puts("So Far!        ");                   
                   }                                                                                                              
            }
            ch = 'b';
     }                               
            
                     
    if (key_number[i][column] == 'C')   {  
        lcd_clear();  
        lcd_gotoxy(0,0);                                                 
        error = 0;       
        num_index = 0;  
        num_holder[0] = nuller[0];    
        num_holder[1] = nuller[1];
        num_holder[2] = nuller[2];    
        num_holder[3] = nuller[3];

        buffer[0] = nuller[0];    
        buffer[1] = nuller[1];
        buffer[2] = nuller[2];    
        buffer[3] = nuller[3];                    
        num_counter = 0;
        //current_number = 0;                                                                                                                                                                                                                                                                                 
    }          
    
    PORTD = 0xF0;

}


void main(void) {
    // Ports
    DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
    PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);
    DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
    PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (1<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);
    DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
    PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);
    DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
    PORTD=(1<<PORTD7) | (1<<PORTD6) | (1<<PORTD5) | (1<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

    // INT0: Off , INT1: Off,  INT2: On  Mode: Falling Edge
    GICR|=(0<<INT1) | (0<<INT0) | (1<<INT2);
    MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
    MCUCSR=(0<<ISC2);
    GIFR=(0<<INTF1) | (0<<INTF0) | (1<<INTF2);
    lcd_init(16);
    #asm("sei")

    while (1) {    
        if  (num_index >= no_of_digits)  {    
            ch = ch;
            num_index = 0;   
            error = 0;      
            delay_ms(1500);
            lcd_clear();  
            lcd_gotoxy(0,0);   
            num_holder[0] = nuller[0];    
            num_holder[1] = nuller[1];
            num_holder[2] = nuller[2];    
            buffer[0] = nuller[0];    
            buffer[1] = nuller[1];
            buffer[2] = nuller[2];    
            buffer[3] = nuller[3];                            
            num_counter = 0;        
            //current_number = NULL;;                                                                                                                                                                                                                                                                                                           
        }
        
        if (new_game) {  
                lcd_gotoxy(3,0); 
                lcd_puts("Welcome To");
                lcd_gotoxy(3,1); 
                lcd_puts("Guess Game");
                lcd_gotoxy(0,2); 
                lcd_puts("Press = to start");
        }  
        
        if (ch == '=' && !active_session ) {     
            lcd_clear();                   
            new_game = false;
            active_session = true;
            ch = '';  
            srand(++seed_time);
            target =  rand() % rand_range;
            lcd_gotoxy(0,1); 
            lcd_puts("Number Generated");
            delay_ms(500);    
            lcd_clear();        
        }                 
        }
}








//                    itoa(intpart, buffer);         // convert to string
//                    lcd_gotoxy(0,3); 
//                    lcd_puts(buffer);   