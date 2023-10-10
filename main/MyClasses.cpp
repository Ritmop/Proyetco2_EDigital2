#include "MyClasses.h"
#include "ILI9341_spi.h"

const int LCD_min_x = 6;
const int LCD_max_x = 320;
const int LCD_min_y = 33;
const int LCD_max_y = 207;
const int backgroundColor = 0x0000;

const float a = 0.03;
const float vel_leap = 0.5;
const float vel_bullet = 5;

/*********************************************************************************************************
  PLAYER
*********************************************************************************************************/
Player::Player(unsigned int w, unsigned int h, unsigned char* bitmap) {
  _width_heli = w;
  _height_heli = h;
  _sprite_heli = bitmap;
}

void Player::update_display(unsigned int t, unsigned int counter) {
  unsigned int animation = (counter / 5 % 3);
  free_fall(t);
  update_bullet(t);
  if (t - t0_immunity < 200) {
    LCD_Sprite(_x_pos_heli, _y_pos_heli, _width_heli, _height_heli, _sprite_heli, 3, 2, 0, 0);
    hitbox_heli = {0, 0, 0, 0};
  }
  else {
    LCD_Sprite(_x_pos_heli, _y_pos_heli, _width_heli, _height_heli, _sprite_heli, 3, animation, 0, 0);
    hitbox_heli = {_x_pos_heli, _y_pos_heli, _width_heli, _height_heli};
  }
}

void Player::set_to(unsigned int x, unsigned int y) {
  /*FillRect(_x_prev, _y_prev, _width, _height, color);
    _x_prev = _x_pos_heli;
    _y_prev = _y_pos_heli;*/
  _x_pos_heli = x;
  _y_pos_heli = y;
}

void Player::move_right() {
  int _x_prev = _x_pos_heli;
  _x_pos_heli += 5;
  check_bounds();
  FillRect(_x_prev, _y_pos_heli, _x_pos_heli - _x_prev, _height_heli, backgroundColor);
}

void Player::move_left() {
  int _x_prev = _x_pos_heli;
  _x_pos_heli -= 5;
  check_bounds();
  FillRect(_x_pos_heli + _width_heli, _y_pos_heli,  _x_prev - _x_pos_heli, _height_heli , backgroundColor);
}

int Player::check_bounds() {
  if (_y_pos_heli - 1 < LCD_min_y) {
    _y_pos_heli = LCD_min_y;
    if (_x_pos_heli - 1 < LCD_min_x) {
      _x_pos_heli = LCD_min_x;
      return 4; //Supera límite izquierdo y superior
    }
    else if (_x_pos_heli + _width_heli + 1 > LCD_max_x) {
      _x_pos_heli = LCD_max_x - _width_heli;
      return 2; //Supera límite derecho y superior
    }
    else {
      return 3; //Supera límite superior
    }
  }
  else if (_y_pos_heli + _height_heli + 1 > LCD_max_y) {
    _y_pos_heli = LCD_max_y - _height_heli;
    if (_x_pos_heli - 1 < LCD_min_x) {
      _x_pos_heli = LCD_min_x;
      return 6; //Supera límite izquierdo e inferior
    }
    else if (_x_pos_heli + _width_heli + 1 > LCD_max_x) {
      _x_pos_heli = LCD_max_x - _width_heli;
      return 8; //Supera límite derecho e inferior
    }
    else {
      return 7; //Supera límite inferior
    }
  }
  else {
    if (_x_pos_heli - 1 < LCD_min_x) {
      _x_pos_heli = LCD_min_x;
      return 5; //Supera límite izquierdo
    }
    else if (_x_pos_heli + _width_heli + 1 > LCD_max_x) {
      _x_pos_heli = LCD_max_x - _width_heli;
      return 1; //Supera límite derecho
    }
    else {
      return 0; //No supera ningún límite
    }
  }
}

void Player::free_fall(unsigned int t) {
  int y_prev = _y_pos_heli;
  int delta_t = t - _t0_leap;
  _y_pos_heli += - vel_leap * delta_t + (a / 2) * delta_t * delta_t; //Movimiento parabólico
  check_bounds();
  if (_y_pos_heli < y_prev) { //Borrar pixeles cuando está subiendo
    FillRect(_x_pos_heli, _y_pos_heli + _height_heli, _width_heli - 1, y_prev - _y_pos_heli, backgroundColor);
  }
  else { //Borrar pixeles cuando está bajando
    FillRect(_x_pos_heli, y_prev, _width_heli - 1, _y_pos_heli - y_prev, backgroundColor);
  }
}

void Player::leap(unsigned int t) {
  _t0_leap = t;
}

void Player::define_bullet(unsigned int wB, unsigned int hB, unsigned char* bitmapB, unsigned int wE, unsigned int hE, unsigned char* bitmapE) {
  _width_bullet = wB;
  _height_bullet = hB;
  _bitmap_bullet = bitmapB;
  _width_explotion = wE;
  _height_explotion = hE;
  _bitmap_bullet_explotion = bitmapE;
}

void Player::shoot(unsigned int t) {
  if (bullet_state == IDLE) {
    _x_pos_bullet = _x_pos_heli + _width_heli;
    _y_pos_bullet = _y_pos_heli + 8;
    _x0_shoot = _x_pos_bullet;
    _t_explotion = t;
    bullet_state = SHOOTING;
  }
  else {
    bullet_state = EXPLOTION;
  }
}

void Player::update_bullet(unsigned int t) {
  Serial.println(bullet_state);
  switch (bullet_state) {
    case IDLE:
      hitbox_bullet = {0, 0, 0, 0};
      break;
    case SHOOTING:
      _x_prev_bullet = _x_pos_bullet;
      if (_x_pos_bullet - _x0_shoot < 150) {
        _x_pos_bullet += vel_bullet;
        LCD_Bitmap(_x_pos_bullet, _y_pos_bullet, _width_bullet, _height_bullet, _bitmap_bullet);
        FillRect(_x_prev_bullet, _y_pos_bullet, _x_pos_bullet - _x_prev_bullet, _height_bullet, backgroundColor); //Borrar pixeles de la trayectoria
        hitbox_bullet = {_x_pos_bullet, _y_pos_bullet, _width_bullet, _height_bullet};
      }
      else bullet_state = EXPLOTION;
      break;
    case EXPLOTION:
      if (t - _t_explotion < 200) {
        _x_exp = (_x_pos_bullet + _width_explotion >= LCD_max_x) ? LCD_max_x - _width_explotion : _x_pos_bullet;
        _y_exp = (_y_pos_bullet - 10 + _height_explotion > LCD_max_y) ? LCD_max_y - _height_explotion : _y_pos_bullet - 10;
        LCD_Bitmap(_x_exp, _y_exp, _width_explotion, _height_explotion, _bitmap_bullet_explotion);
        hitbox_bullet = {_x_exp, _y_exp, _width_explotion, _height_explotion};
      }
      else {
        bullet_state = REMOVE;
      }
      break;
    case REMOVE:
      FillRect(_x_exp, _y_exp, _width_explotion, _height_explotion, backgroundColor);
      hitbox_bullet = {0, 0, 0, 0};
      bullet_state = IDLE;
      break;
  }
}

Menu_pointer::Menu_pointer(unsigned int w, unsigned int h, unsigned char* bitmap) {
  _width = w;
  _height = h;
  _bitmap = bitmap;
}

void Menu_pointer::set_to(unsigned int x, unsigned int y, unsigned int color) {
  FillRect(_x_prev, _y_prev, _width, _height, color);
  _x_prev = _x_pos;
  _y_prev = y_pos;
  _x_pos = x;
  y_pos = y;
  LCD_Bitmap(_x_pos, y_pos, _width, _height, _bitmap);
}
