#include "MyClasses.h"
#include "ILI9341_spi.h"

const int LCD_min_x = 6;
const int LCD_max_x = 320;
const int LCD_min_y = 33;
const int LCD_max_y = 207;

const int backGroundColor = 0x0000;

const float a = 0.05;
const float vel_leap = 0.7;
const float vel_bullet = 10;

/*********************************************************************************************************
  PLAYER
*********************************************************************************************************/
Player::Player(unsigned int w, unsigned int h, unsigned char* bitmap) {
  width_heli = w;
  height_heli = h;
  _bitmap_heli = bitmap;
}

void Player::update_display() {
  LCD_Bitmap(x_pos_heli, y_pos_heli, width_heli, height_heli, _bitmap_heli);
  LCD_Bitmap(x_pos_bullet, y_pos_bullet, width_bullet, height_bullet, _bitmap_bullet);
}

void Player::set_to(unsigned int x, unsigned int y) {
  x_pos_heli = x;
  y_pos_heli = y;
}

void Player::move_right() {
  int x_prev = x_pos_heli;
  x_pos_heli += 5;
  check_bounds();
  FillRect(x_prev, y_pos_heli, x_pos_heli - x_prev, height_heli, backGroundColor);
}

void Player::move_left() {
  int x_prev = x_pos_heli;
  x_pos_heli -= 5;
  check_bounds();
  FillRect(x_pos_heli + width_heli, y_pos_heli,  x_prev - x_pos_heli, height_heli , backGroundColor);
}

/* Player::move_up() {
  y_pos_heli--;
  check_bounds();
  H_line(x_pos, y_pos_heli + height_heli, width_heli - 1, backGroundColor);
  }

  void Player::move_down() {
  y_pos_heli++;
  check_bounds();
  H_line(x_pos, y_pos_heli - 1, width_heli - 1, backGroundColor);
  }*/

int Player::check_bounds() {
  if (y_pos_heli - 1 < LCD_min_y) {
    y_pos_heli = LCD_min_y;
    if (x_pos_heli - 1 < LCD_min_x) {
      x_pos_heli = LCD_min_x;
      //LCD_Print("4", 200, 100, 2, 0xffff, backGroundColor);
      return 4; //Supera límite izquierdo y superior
    }
    else if (x_pos_heli + width_heli + 1 > LCD_max_x) {
      x_pos_heli = LCD_max_x - width_heli;
      //LCD_Print("2", 200, 100, 2, 0xffff, backGroundColor);
      return 2; //Supera límite derecho y superior
    }
    else {
      //LCD_Print("3", 200, 100, 2, 0xffff, backGroundColor);
      return 3; //Supera límite superior
    }
  }
  else if (y_pos_heli + height_heli + 1 > LCD_max_y) {
    y_pos_heli = LCD_max_y - height_heli;
    if (x_pos_heli - 1 < LCD_min_x) {
      x_pos_heli = LCD_min_x;
      //LCD_Print("6", 200, 100, 2, 0x0f0f, backGroundColor);
      return 6; //Supera límite izquierdo e inferior
    }
    else if (x_pos_heli + width_heli + 1 > LCD_max_x) {
      x_pos_heli = LCD_max_x - width_heli;
      //LCD_Print("8", 200, 100, 2, 0x0ff0, backGroundColor);
      return 8; //Supera límite derecho e inferior
    }
    else {
      ////LCD_Print("7", 200, 100, 2, 0xf00f, backGroundColor);
      return 7; //Supera límite inferior
    }
  }
  else {
    if (x_pos_heli - 1 < LCD_min_x) {
      x_pos_heli = LCD_min_x;
      //LCD_Print("5", 200, 100, 2, 0xff00, backGroundColor);
      return 5; //Supera límite izquierdo
    }
    else if (x_pos_heli + width_heli + 1 > LCD_max_x) {
      x_pos_heli = LCD_max_x - width_heli;
      //LCD_Print("1", 200, 100, 2, 0x00ff, backGroundColor);
      return 1; //Supera límite derecho
    }
    else {
      //LCD_Print("0", 200, 100, 2, 0xffff, backGroundColor);
      return 0; //No supera ningún límite
    }
  }
}

void Player::free_fall(unsigned int t) {
  int y_prev = y_pos_heli;
  int delta_t = t - _t0_leap;
  y_pos_heli += - vel_leap * delta_t + (a / 2) * delta_t * delta_t; //Movimiento parabólico
  check_bounds();
  if (y_pos_heli < y_prev) { //Borrar pixeles cuando está subiendo
    FillRect(x_pos_heli, y_pos_heli + height_heli, width_heli - 1, y_prev - y_pos_heli, backGroundColor);
  }
  else { //Borrar pixeles cuando está bajando
    FillRect(x_pos_heli, y_prev, width_heli - 1, y_pos_heli - y_prev, backGroundColor);
  }
}

void Player::leap(unsigned int t) {
  _t0_leap = t;
}

void Player::shoot(unsigned int t, unsigned int w, unsigned int h, unsigned char* bitmap) {
  if (_shooting == 0) { //Shooting spam control
    _shooting = 1;
    _t0_shoot = t;
    width_bullet = w;
    height_bullet = h;
    _bitmap_bullet = bitmap;
    x_pos_bullet = x_pos_heli + 30;
    y_pos_bullet = y_pos_heli + 20;
    _x0_shoot = x_pos_bullet;
  }
  else {//Explodes* bullet
    _shooting = 0;
  }
}

void Player::move_proj(unsigned int t) {
  int x_prev = x_pos_bullet;
  int delta_t = t - _t0_shoot;
  if ((x_pos_bullet - _x0_shoot < 180) && (_shooting == 1)) {
    x_pos_bullet += vel_bullet;
    FillRect(x_prev, y_pos_bullet, x_pos_bullet - x_prev, height_bullet, backGroundColor); //Borrar pixeles de la trayectoria
  }
  else {
    FillRect(x_pos_bullet, y_pos_bullet, width_bullet - 1, height_bullet, 0xff00); //Borrar pixeles después de la distancia máxima o al explotar
    width_bullet = 0;
    height_bullet = 0;
    _shooting = 0;
  }
}