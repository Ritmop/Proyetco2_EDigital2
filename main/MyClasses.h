#ifndef _MY_CLASSES_H_
#define _MY_CLASSES_H_

struct hitbox {
  unsigned int x;
  unsigned int y;
  unsigned int w;
  unsigned int h;
};

typedef enum bulletState
{
  IDLE = 0,
  SHOOTING = 1,
  EXPLOTION = 2,
  REMOVE = 3
} bulletState;

class Player {
  public:
    Player(unsigned int w, unsigned int h, unsigned char* bitmap);

    //Helicopter functions
    void set_to(unsigned int x, unsigned int y);
    void update_display(unsigned int t, unsigned int counter);
    void move_left();
    void move_right();
    int check_bounds();
    void free_fall(unsigned int t);
    void leap(unsigned int t);

    //Helicopter vars
    hitbox hitbox_heli;
    unsigned int t0_immunity = 0;

    //Bullet functions
    void define_bullet(unsigned int wB, unsigned int hB, unsigned char* bitmapB, unsigned int wE, unsigned int hE, unsigned char* bitmapE);
    void shoot(unsigned int t);
    void update_bullet(unsigned int t);

    //Bullet vars
    hitbox hitbox_bullet;
    bulletState bullet_state = IDLE;

  private:
    //Helicopter vars
    unsigned int _x_pos_heli;
    unsigned int _y_pos_heli;
    unsigned int _width_heli;
    unsigned int _height_heli;

    unsigned int _t0_leap;
    unsigned char* _sprite_heli;

    //Bullet vars
    unsigned int _x_prev_bullet;
    unsigned int _x_pos_bullet = 320;
    unsigned int _y_pos_bullet = 0;
    unsigned int _width_bullet;
    unsigned int _height_bullet;

    unsigned int _x_exp;
    unsigned int _y_exp;    
    unsigned int _width_explotion;
    unsigned int _height_explotion;
    
    unsigned int _x0_shoot;
    unsigned int _t_explotion = 0;
    unsigned char* _bitmap_bullet;
    unsigned char* _bitmap_bullet_explotion;
};

class Menu_pointer {
  public:
    Menu_pointer(unsigned int w, unsigned int h, unsigned char* bitmap);

    void set_to(unsigned int x, unsigned int y, unsigned int color);

    unsigned int _x_pos;
    unsigned int y_pos;

  private:
    unsigned char* _bitmap;
    unsigned int _width;
    unsigned int _height;
    unsigned int _x_prev;
    unsigned int _y_prev;
};

#endif
