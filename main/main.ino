#include <TM4C123GH6PM.h>
#include <SD.h>
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

#include "game_classes.h"
#include "ILI9341_spi.h"
#include "helicopter_sprites.h"

//Graficos externos
extern unsigned char pointer[];
extern unsigned char title_screen_bg[];
extern unsigned char helicopter_g_title_screen[];
extern unsigned char helicopter_r_title_screen[];

//Classes
File root;
File txtFile;
Player helicopter1(32, 16, helicopter_g, helicopter_g_hit);
Player helicopter2(32, 16, helicopter_r, helicopter_r_hit);

Menu_pointer m_pointer(20, 16, pointer);

//Game Status
typedef enum GameStateType
{
  MENU = 0,
  PLAYING = 1,
  SCORES = 2,
  HELP = 3,
  CREDITS = 4,
  GAME_OVER = 5
} GameStateType;
GameStateType gameStatus = MENU;

//Player buttons
#define P1Left  38
#define P1Right 37
#define P1Up    36
#define P1Shoot 35
#define P2Left  34
#define P2Right 33
#define P2Up    32
#define P2Shoot 31

//Game vars
int tiempo = 0;
int animation_counter = 0;
int scoreP1 = 0;
int scoreP2 = 0;
int prev_scoreP1 = 1; //Diferente a 0 para mostrar el punteo desde el inicio
int prev_scoreP2 = 1;

//Other vars
int game_bgColor = 0x0000;
int menu_bgColor = 0x3DDF;
int fontColor1 = 0xFC00;
int fontColor2 = 0xFFFF;

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
     Serial.println("In menu");
    draw_menu();
    menu_loop();
  }
  else if (gameStatus == PLAYING) {
    draw_backgroud();
    while (true) {
      execute_game();
    }
  }
  else if (gameStatus == HELP){
    //read_SD();
  }
}

void draw_menu() {
  //LCD_Bitmap(0, 0, 320, 240, title_screen_bg);
  LCD_Print("PLAY", 128, 110, 2, fontColor2, menu_bgColor);
  LCD_Print("HELP", 128, 130, 2, fontColor2, menu_bgColor);
  LCD_Print("CREDITS", 104, 150, 2, fontColor2, menu_bgColor);
  m_pointer.set_to(80, 110, menu_bgColor);
  m_pointer.set_to(80, 110, menu_bgColor);//Repeat to set previous coordinates

}

void menu_loop() {
  while (digitalRead(P1Up)) {
    //Cycle through options
    if (!digitalRead(P1Left)) {
      delay(500);
      m_pointer.y_pos -= 20;
      if (m_pointer.y_pos < 110) m_pointer.y_pos = 150;
      m_pointer.set_to(80, m_pointer.y_pos, menu_bgColor);
    }
    else if (!digitalRead(P1Right)) {
      delay(500);
      m_pointer.y_pos += 20;
      if (m_pointer.y_pos > 150) m_pointer.y_pos = 110;
      m_pointer.set_to(80, m_pointer.y_pos, menu_bgColor);
    }
    animation_counter++;
    m_pointer.update_display(animation_counter);
    LCD_Sprite(15, 80, 32, 20, helicopter_g_title_screen, 4, (animation_counter / 6) % 4, 0, 0);
    LCD_Sprite(120, 80, 32, 20, helicopter_r_title_screen, 4, (animation_counter / 6) % 4, 0, 0);
  }
  //Next page
  switch (m_pointer.y_pos) {
    case 110:
      LCD_Print("PLAY", 128, 110, 2, fontColor1, menu_bgColor);
      delay(200);
      LCD_Print("PLAY", 128, 110, 2, fontColor2, menu_bgColor);
      delay(200);
      LCD_Print("PLAY", 128, 110, 2, fontColor1, menu_bgColor);
      gameStatus = PLAYING;

      for (int x; x < 100; x++) {
        animation_counter++;
        LCD_Sprite(15 + x, 80, 32, 20, helicopter_g_title_screen, 4, (animation_counter / 5) % 4, 0, 0);
        LCD_Sprite(120 + x, 80, 32, 20, helicopter_r_title_screen, 4, (animation_counter / 5) % 4, 0, 0);
        V_line(15 + x, 80, 20, menu_bgColor);
        V_line(120 + x, 80, 20, menu_bgColor);
      }
      break;
    case 130:
      LCD_Print("HELP", 128, 130, 2, fontColor1, menu_bgColor);
      delay(200);
      LCD_Print("HELP", 128, 130, 2, fontColor2, menu_bgColor);
      delay(200);
      LCD_Print("HELP", 128, 130, 2, fontColor1, menu_bgColor);
      gameStatus = HELP;
      break;
    case 150:
      LCD_Print("CREDITS", 104, 150, 2, fontColor1, menu_bgColor);
      delay(200);
      LCD_Print("CREDITS", 104, 150, 2, fontColor2, menu_bgColor);
      delay(200);
      LCD_Print("CREDITS", 104, 150, 2, fontColor1, menu_bgColor);
      //gameStatus = CREDITS;
      gameStatus = MENU;
      break;
    default:
      gameStatus = MENU;
      break;
  }
}

void draw_backgroud() {
  LCD_Clear(0x0000);
  for (int x = 0; x < 319;) {
    LCD_Bitmap(x, 0, 16, 16, tile2);
    LCD_Bitmap(x, 16, 16, 16, tile);
    LCD_Bitmap(x, 207, 16, 16, tile);
    LCD_Bitmap(x, 223, 16, 16, tile2);
    x += 16;
  }
  LCD_Print("P1:", 0, 0, 2, 0x0000, 0xdd55);
  LCD_Print("P2:", 208, 0, 2, 0x0000, 0xdd55);
}

void execute_game() {
  helicopter1.define_bullet(16, 4, bullet, 26, 28, explotion);
  helicopter2.define_bullet(16, 4, bullet, 26, 28, explotion);
  helicopter1.set_to(10, 100);
  helicopter2.set_to(10, 120);
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
      delay(250);
      helicopter1.shoot(tiempo);
    }
    if (!digitalRead(P2Shoot)) {
      delay(250);
      helicopter2.shoot(tiempo);
    }
    update_game();
    update_scores();
    tiempo += 2;
    animation_counter++;
  }
}

void update_game() {
  helicopter1.update_display(tiempo, animation_counter);
  helicopter2.update_display(tiempo, animation_counter);

  if (are_colliding(helicopter2.hitbox_bullet, helicopter1.hitbox_heli)) {
    helicopter2.bullet_state = EXPLOTION;//Forzar explosion del proyectil
    helicopter1.t0_immunity = tiempo;
    scoreP2 += 50;
  }
  if (are_colliding(helicopter2.hitbox_heli, helicopter1.hitbox_bullet)) {
    helicopter1.bullet_state = EXPLOTION;//Forzar explosion del proyectil
    helicopter2.t0_immunity = tiempo;
    scoreP1 += 50;
  }
  if (are_colliding(helicopter1.hitbox_heli, helicopter1.hitbox_bullet)) {
    helicopter1.bullet_state = EXPLOTION;//Forzar explosion del proyectil
    helicopter1.t0_immunity = tiempo;
    scoreP1 -= 100;
  }
  if (are_colliding(helicopter2.hitbox_heli, helicopter2.hitbox_bullet)) {
    helicopter2.bullet_state = EXPLOTION;//Forzar explosion del proyectil
    helicopter2.t0_immunity = tiempo;
    scoreP1 -= 100;
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

int are_colliding(hitbox h1, hitbox h2) {
  return ((h1.x + h1.w > h2.x) && (h1.x < h2.x + h2.w) &&
          (h1.y + h1.h > h2.y) && (h1.y < h2.y + h2.h));
}

/*void read_SD() {
  SPI.setModule(0);

  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);

  if (!SD.begin(12)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.\n");

  root = SD.open("ASCII_~1/");

  getFiles(root);
  Serial.println("done!");  
}

String directory[3];
void getFiles(File root) {
  int i = 0;
   while(true) {
     
     File entry = root.openNextFile();
     if (!entry) {
       // no more files
       break;
     }
     directory[i] = entry.name();
     entry.close();
     i++;
   }
}*/
