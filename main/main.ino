#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "MyClasses.h"
#include "ILI9341_spi.h"
#include "bitmaps.h"

//Classes
Player helicopter1(64, 32, prueba);
Player helicopter2(64, 32, prueba);

typedef enum GameStateType
{
  menu = 0,
  playing = 1,
  gameOver = 2
} GameStateType;

GameStateType gameStatus = menu;

int tiempo = 0;


void setup() {
  //Hardware setup
  pinMode(PUSH1, INPUT_PULLUP);
  pinMode(PUSH2, INPUT_PULLUP);

  //LCD Setup
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);
  SPI.setModule(3); //SPI for LCD
  Serial.println("Inicio");
  LCD_Init();
  delay(10);
}

void loop() {
  if (gameStatus == menu) {
    draw_menu();
    while (true) {
      if (!digitalRead(PUSH1)) {
        Serial.print(gameStatus);
        gameStatus = playing;
        break;
      }
    }
  }
  else if (gameStatus == playing) {
    draw_backgroud();
    while (true) {
      execute_game();
    }
  }
}



void draw_menu() {
  LCD_Clear(0x2800);
  LCD_Print("HELICOPTERS", 80, 20, 2, 0xFC08, 0x2800);
  LCD_Print("OPTION 1", 80, 110, 2, 0x0000, 0x2800);
  LCD_Print("OPTION 2", 80, 130, 2, 0x0000, 0x2800);
  LCD_Print("OPTION 3", 80, 150, 2, 0x0000, 0x2800);
}

void draw_backgroud() {
  LCD_Clear(0x0000);
  for (int x = 0; x < 319;) {
    LCD_Bitmap(x, 0, 16, 16, tile2);
    LCD_Bitmap(x, 16, 16, 16, tile);
    LCD_Bitmap(x, 207, 16, 16, tile);
    LCD_Bitmap(x, 223, 16, 16, tile2);
    x += 15;
  }
}

void execute_game() {  
  helicopter1.set_to(15, 100);
  helicopter2.set_to(15, 120);
  delay(1000);

  while (true) {
    if (!digitalRead(PUSH1)) {
      helicopter1.leap(tiempo);
      helicopter2.leap(tiempo);
      //helicopter1.move_right();
    }
    if (!digitalRead(PUSH2)) {
      while (!digitalRead(PUSH2));
      helicopter1.shoot(tiempo, 16, 16, enemy);
      helicopter2.shoot(tiempo, 16, 16, enemy);
    }
    helicopter1.free_fall(tiempo);
    helicopter1.move_proj(tiempo);
    helicopter1.update_display();
    helicopter2.free_fall(tiempo);
    helicopter2.move_proj(tiempo);
    helicopter2.update_display();
    tiempo += 2;
  }
}
