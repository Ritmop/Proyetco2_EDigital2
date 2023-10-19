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

//Graficos externos
extern unsigned char pointer[];
extern unsigned char helicopter_g_title_screen[];
extern unsigned char helicopter_r_title_screen[];

extern unsigned char helicopter_g[];
extern unsigned char helicopter_g_hit[];
extern unsigned char helicopter_r[];
extern unsigned char helicopter_r_hit[];
extern unsigned char bullet[];
extern unsigned char explotion[];

extern unsigned char pico64d[];
extern unsigned char pico50d[];
extern unsigned char pico32d[];
extern unsigned char pico64u[];
extern unsigned char pico50u[];
extern unsigned char pico32u[];

extern unsigned char topBlock[];
extern unsigned char bottomBlock[];
extern unsigned char internalBlock[];

extern unsigned char hearts[];

//Objects
Menu_pointer m_pointer(20, 16, pointer);

Player helicopter1(32, 16, helicopter_g, helicopter_g_hit);
Player helicopter2(32, 16, helicopter_r, helicopter_r_hit);

Obstacle Obs1(1, 0, 32, 32, pico32d);
Obstacle Obs2(81, 1, 32, 32, pico32u);
Obstacle Obs3(161, 1, 32, 32, pico32u);

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
GameStateType gameStatus = PLAYING;

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
int livesP1 = 3;
int livesP2 = 3;
int prev_livesP1 = 4; //Diferente al inicial para mostrar el punteo desde el inicio
int prev_livesP2 = 4;

//Other vars
const int game_bgColor = 0x0000;
const int menu_bgColor = 0x3DDF;
const int fontColor1 = 0xFC00;
const int fontColor2 = 0xFFFF;

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

  pinMode(LCD_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);

  //LCD Setup
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);

  SPI.setModule(3); //SPI for LCD
  digitalWrite(LCD_CS, LOW);    //Select LCD
  digitalWrite(SD_CS, HIGH);
  Serial.println("SPI init");
  SD_init();
  delay(10);
  LCD_Init();
  delay(10);
  Serial.println("SD & LDC init");
  LCD_Clear(0x0000);
}


void loop() {
  if (gameStatus == MENU) {
    draw_menu();
    menu_loop();
  }
  else if (gameStatus == PLAYING) {
    draw_backgroud();
    while (true) {
      execute_game();
    }
  }
  else if (gameStatus == HELP) {
    FillRect(0, 0, 64, 240, 0xffff);
    FillRect(64, 0, 64, 240, 0xffff);
    FillRect(128, 0, 64, 240, 0xffff);
    FillRect(192, 0, 64, 240, 0xffff);
    FillRect(256, 0, 64, 240, 0xffff);
    gameStatus = MENU;
  }
}

void draw_menu() {
  imageFromSD("COPTER~1/TITLES~1.TXT");
  LCD_Print("PLAY", 128, 110, 2, fontColor2, menu_bgColor);
  LCD_Print("HELP", 128, 130, 2, fontColor2, menu_bgColor);
  LCD_Print("CREDITS", 104, 150, 2, fontColor2, menu_bgColor);
  m_pointer.set_to(80, 110, menu_bgColor);
  m_pointer.set_to(80, 110, menu_bgColor);//Repeat to set previous coordinates

}

void menu_loop() {
  while (digitalRead(P1Up) && digitalRead(P2Up)) {
    //Cycle through options
    if (!digitalRead(P1Left) || !digitalRead(P2Left)) {
      delay(500);
      m_pointer.y_pos -= 20;
      if (m_pointer.y_pos < 110) m_pointer.y_pos = 150;
      m_pointer.set_to(80, m_pointer.y_pos, menu_bgColor);
    }
    else if (!digitalRead(P1Right) || !digitalRead(P2Right)) {
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
  helicopter1.set_to(10, 100);//Set initial values for helicopters
  helicopter2.set_to(10, 120);
  LCD_Clear(0x0000);
  for (int x = 0; x < 319;) {
    LCD_Bitmap(x, 0, 16, 16, internalBlock);
    LCD_Bitmap(x, 16, 16, 16, topBlock);
    LCD_Bitmap(x, 207, 16, 16, bottomBlock);
    LCD_Bitmap(x, 223, 16, 16, internalBlock);
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
    //Read player commands
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
      helicopter1.immunity = 0;
      helicopter1.leap(tiempo);
    }
    if (!digitalRead(P2Up)) {
      helicopter2.immunity = 0;
      helicopter2.leap(tiempo);
    }
    if (!digitalRead(P1Shoot) && !helicopter1.immunity) {
      delay(250);
      helicopter1.shoot(tiempo);
    }
    if (!digitalRead(P2Shoot) && !helicopter2.immunity) {
      delay(250);
      helicopter2.shoot(tiempo);
    }
    //Update game
    update_game();
    update_lives();
    tiempo += 2;
    animation_counter++;
  }
}

void update_game() {
  //Update players & obstacles
  helicopter1.update_display(tiempo, animation_counter);
  helicopter2.update_display(tiempo, animation_counter);

  Obs1.update_display();
  Obs2.update_display();
  Obs3.update_display();

  //Check collitions
  //with bullets
  if (are_colliding(helicopter2.hitbox_bullet, helicopter1.hitbox_heli)) {
    helicopter2.bullet_state = EXPLOTION;//Forzar explosion del proyectil
    helicopter1.immunity = 1;
    livesP1--;
  }
  if (are_colliding(helicopter2.hitbox_heli, helicopter1.hitbox_bullet)) {
    helicopter1.bullet_state = EXPLOTION;//Forzar explosion del proyectil
    helicopter2.immunity = 1;
    livesP2--;
  }
  if (are_colliding(helicopter1.hitbox_heli, helicopter1.hitbox_bullet)) {
    helicopter1.bullet_state = EXPLOTION;//Forzar explosion del proyectil
    helicopter1.immunity = 1;
    livesP1--;
  }
  if (are_colliding(helicopter2.hitbox_heli, helicopter2.hitbox_bullet)) {
    helicopter2.bullet_state = EXPLOTION;//Forzar explosion del proyectil
    helicopter2.immunity = 1;
    livesP2--;
  }
  //with obstacles
  if (are_colliding(helicopter1.hitbox_heli, Obs1.hitbox_obstacle) || are_colliding(helicopter1.hitbox_heli, Obs2.hitbox_obstacle) || are_colliding(helicopter1.hitbox_heli, Obs3.hitbox_obstacle)) {
    helicopter1.immunity = 1;
    livesP1--;
  }
  if (are_colliding(helicopter2.hitbox_heli, Obs1.hitbox_obstacle) || are_colliding(helicopter2.hitbox_heli, Obs2.hitbox_obstacle) || are_colliding(helicopter2.hitbox_heli, Obs3.hitbox_obstacle)) {
    helicopter2.immunity = 1;
    livesP2--;
  }

  //Check obstacles
  if (Obs1.x_pos <= 0) {
    unsigned int randNum = random(0, 6);
    switch (randNum) {
      case 0:
        //Small down
        Obs1.restart(0, 32, 32, pico32d);
        break;
      case 1:
        //Medium down
        Obs1.restart(0, 32, 50, pico50d);
        break;
      case 2:
        //Big down
        Obs1.restart(0, 40, 64, pico64d);
        break;
      case 3:
        //Small up
        Obs1.restart(1, 32, 32, pico32u);
        break;
      case 4:
        //Medium up
        Obs1.restart(1, 32, 50, pico50u);
        break;
      case 5:
        //Big up
        Obs1.restart(1, 40, 64, pico64u);
        break;
      default:
        //Small down
        Obs1.restart(0, 32, 32, pico32d);
        break;
    }
  }
  if (Obs2.x_pos <= 0) {
    unsigned int randNum = random(0, 6);
    switch (randNum) {
      case 0:
        //Small down
        Obs2.restart(0, 32, 32, pico32d);
        break;
      case 1:
        //Medium down
        Obs2.restart(0, 32, 50, pico50d);
        break;
      case 2:
        //Big down
        Obs2.restart(0, 40, 64, pico64d);
        break;
      case 3:
        //Small up
        Obs2.restart(1, 32, 32, pico32u);
        break;
      case 4:
        //Medium up
        Obs2.restart(1, 32, 50, pico50u);
        break;
      case 5:
        //Big up
        Obs2.restart(1, 40, 64, pico64u);
        break;
      default:
        //Small down
        Obs2.restart(0, 32, 32, pico32d);
        break;
    }
  }
  if (Obs3.x_pos <= 0) {
    unsigned int randNum = random(0, 6);
    switch (randNum) {
      case 0:
        //Small down
        Obs3.restart(0, 32, 32, pico32d);
        break;
      case 1:
        //Medium down
        Obs3.restart(0, 32, 50, pico50d);
        break;
      case 2:
        //Big down
        Obs3.restart(0, 40, 64, pico64d);
        break;
      case 3:
        //Small up
        Obs3.restart(1, 32, 32, pico32u);
        break;
      case 4:
        //Medium up
        Obs3.restart(1, 32, 50, pico50u);
        break;
      case 5:
        //Big up
        Obs3.restart(1, 40, 64, pico64u);
        break;
      default:
        //Small down
        Obs3.restart(0, 32, 32, pico32d);
        break;
    }
  }

}

void update_lives() {
  if (livesP1 != prev_livesP1) {
    LCD_Sprite(48, 0, 60, 16, hearts,4, livesP1, 0, 0);
    prev_livesP1 = livesP1;
  }
  if (livesP2 != prev_livesP2) {
    LCD_Sprite(256, 0, 60, 16, hearts,4, livesP2, 0, 0);
    prev_livesP2 = livesP2;
  }
}

int are_colliding(hitbox h1, hitbox h2) {
  return ((h1.x + h1.w > h2.x) && (h1.x < h2.x + h2.w) &&
          (h1.y + h1.h > h2.y) && (h1.y < h2.y + h2.h));
}



File root;
File txtFile;
#define array_size 9600
//Hacer 16 corridas de 15px de alto cada una para crear la pantalla desde la SD.
//320*15*2 = 9600 bytes
unsigned char temp_image_data[array_size];

void SD_init() {
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.\n");
  digitalWrite(SD_CS, HIGH);//Deselect SD
}

//Las imagenes están almacenadas en formato ASCII
//Cada caracter representa un nibble del número hexadecimal
//de cada par el primero es el nibble alto y el segundo el nibble bajo
void imageFromSD(String archivo_ASCII) {
  txtFile = SD.open(archivo_ASCII.c_str(), FILE_READ);
  if (txtFile) {
    int y_offset = 0;
    char highNibble = 0;
    char lowNibble = 0;
    // read from the file until there's nothing else in it:
    while (txtFile.available()) {
      for (int i = 0; i < array_size; i++) {
        highNibble = txtFile.read() - '0';
        lowNibble = txtFile.read() - '0';
        temp_image_data[i] = highNibble * 16 + lowNibble; //Unir los nibbles
        //Serial.println(temp_image_data[i], HEX);
      }
      digitalWrite(SD_CS, HIGH);//Deselect SD
      LCD_Bitmap(0, y_offset, 320, 15, temp_image_data);
      y_offset += 15;
    }
    // close the file:
    txtFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error while opening file.");
  }
}
