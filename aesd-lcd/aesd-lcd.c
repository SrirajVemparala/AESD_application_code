/*
Name: lcd.c
Description:  client code for lcd integration
Author: Isha Sharma
Reference: 
	https://www.circuitbasics.com/raspberry-pi-lcd-set-up-and-programming-in-c-with-wiringpi/
   	https://us.rs-online.com/m/d/4252b1ecd92888dbb9d8a39b536e7bf2.pdf
Course: AESD
Date: April 9, 2024
*/

#include <wiringPi.h>
#include <lcd.h>

#define LCD_RS  25               //Register select pin
#define LCD_E   24               //Enable Pin

#define LCD_D4  23               //Data pin D4
#define LCD_D5  22               //Data pin D5
#define LCD_D6  21               //Data pin D6
#define LCD_D7  14               //Data pin D7



int main()
{
    int lcd;     
    float temp =25.0;         
    wiringPiSetup();        
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
    
    lcdPosition(lcd, 2, 1); 
    lcdPuts(lcd, "AESD PROJECT");   
    delay(1000);
    lcdClear(lcd);  
	delay(1000);
	
	lcdPosition(lcd, 2, 0); 
	lcdPuts(lcd, "TEMP SYSTEM"); 
	delay(1000);
    lcdClear(lcd); 
    
    lcdPosition(lcd, 0, 0); 
    lcdPrintf(lcd, "Temp: %.2f degC",temp );
    
}
