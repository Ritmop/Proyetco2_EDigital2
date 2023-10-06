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

int tiempo = 0;

int global_y = 100;
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

  //Background
  LCD_Clear(0x0000);
  for (int x = 0; x < 319;) {
    LCD_Bitmap(x, 1, 16, 16, tile2);
    LCD_Bitmap(x, 17, 16, 16, tile);

    LCD_Bitmap(x, 207, 16, 16, tile);
    LCD_Bitmap(x, 223, 16, 16, tile);
    x += 16;
  }
  //Player init
  helicopter1.set_to(10, 100);
  helicopter2.set_to(80, 100);
  //helicopter3.set_to(160, 50);



  delay(1000);
}

void loop() {
  if (!digitalRead(PUSH1)) {
    helicopter1.leap(tiempo);
    helicopter2.leap(tiempo);
    //helicopter1.move_right();
  }
  if (!digitalRead(PUSH2)) {
    while(!digitalRead(PUSH2));
    helicopter1.shoot(tiempo, 16,16,enemy);
    helicopter2.shoot(tiempo, 16,16,enemy);
  }
  helicopter1.free_fall(tiempo);
  helicopter1.move_proj(tiempo);
  helicopter1.update_display();  
  helicopter2.free_fall(tiempo);
  helicopter2.move_proj(tiempo);
  helicopter2.update_display();
  tiempo += 2;
}
