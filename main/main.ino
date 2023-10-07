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
Player helicopter1(32, 16, helicopter);
Player helicopter2(32, 16, helicopter);

//Game Status
typedef enum GameStateType
{
  MENU = 0,
  PLAYING = 1,
  GAME_OVER = 2
} GameStateType;
GameStateType gameStatus = MENU;

//Player buttons
#define P1Left  18
#define P1Right 17
#define P1Up    38
#define P1Shoot 37
#define P2Left  13
#define P2Right 12
#define P2Up    33
#define P2Shoot 32

//Game vars
int tiempo = 0;
int animation_counter = 0;
int scoreP1 = 1000;
int scoreP2 = 1000;
int prev_scoreP1 = 0;
int prev_scoreP2 = 0;

void setup() {
  //Hardware setup
  pinMode(P1Left, INPUT_PULLUP);
  pinMode(P1Right, INPUT_PULLUP);
  pinMode(P1Up, INPUT_PULLUP);
  pinMode(P1Shoot, INPUT_PULLUP);

  pinMode(P2Left, INPUT_PULLUP);
  pinMode(P2Right, INPUT_PULLUP);
  pinMode(P2Up, INPUT_PULLUP);
  pinMode(P2Shoot, INPUT_PULLUP);

  //LCD Setup
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);
  SPI.setModule(3); //SPI for LCD
  Serial.println("Inicio");
  LCD_Init();
  delay(10);
}

void loop() {
  if (gameStatus == MENU) {
    draw_menu();
    while (true) {
      if (!digitalRead(P1Up)) {
        Serial.print(gameStatus);
        gameStatus = PLAYING;
        break;
      }
    }
  }
  else if (gameStatus == PLAYING) {
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

  LCD_Print("P1:", 0, 0, 2, 0x0000, 0xdd55);
  LCD_Print("P2:", 208, 0, 2, 0x0000, 0xdd55);
}

void execute_game() {
  helicopter1.set_to(20, 40);
  helicopter2.set_to(84, 40);
  delay(1000);

  while (true) {
    if (!digitalRead(P1Left)) {
      helicopter1.move_left();
    }
    if (!digitalRead(P2Left)) {
      helicopter2.move_left();
    }
    if (!digitalRead(P1Right)) {
      helicopter1.move_right();
    }
    if (!digitalRead(P2Right)) {
      helicopter2.move_right();
    }
    if (!digitalRead(P1Up)) {
      helicopter1.leap(tiempo);
    }
    if (!digitalRead(P2Up)) {
      helicopter2.leap(tiempo);
    }
    if (!digitalRead(P1Shoot)) {
      delay(500);
      helicopter1.shoot(tiempo, 16, 4, bullet, explotion);
    }
    if (!digitalRead(P2Shoot)) {
      delay(500);
      helicopter2.shoot(tiempo, 16, 4, bullet, explotion);
    }

    helicopter1.free_fall(tiempo);
    helicopter1.move_proj(tiempo);
    helicopter1.update_display(animation_counter);
    helicopter2.free_fall(tiempo);
    helicopter2.move_proj(tiempo);
    helicopter2.update_display(animation_counter);
    tiempo += 2;
    animation_counter++;

    update_scores();
  }
}

void update_scores() {
  if (scoreP1 != prev_scoreP1) {
    LCD_Print(String(scoreP1), 48, 0, 1, 0x0000, 0xdd55);
    prev_scoreP1 = scoreP1;
  }
  if (scoreP2 != prev_scoreP2) {
    LCD_Print(String(scoreP2), 256, 0, 1, 0x0000, 0xdd55);
    prev_scoreP2 = scoreP2;
  }
}
