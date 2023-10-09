#ifndef _MY_CLASSES_H_
#define _MY_CLASSES_H_

struct hitbox {
  unsigned int x;
  unsigned int y;
  unsigned int w;
  unsigned int h;
};

class Player {
  public:
    Player(unsigned int w, unsigned int h, unsigned char* bitmap);

    void set_to(unsigned int x, unsigned int y);
    void update_display(unsigned int counter);
    void move_left();
    void move_right();
    //void move_up();
    //void move_down();
    int check_bounds();
    void free_fall(unsigned int t);
    void leap(unsigned int t);
    void shoot(unsigned int t, unsigned int w, unsigned int h, unsigned char* bitmapB, unsigned char* bitmapE);
    void move_proj(unsigned int t);

    hitbox hitbox_heli;
    hitbox hitbox_bullet;    
    unsigned int _shooting = 0;

  private:
    unsigned int x_pos_heli;
    unsigned int y_pos_heli;
    unsigned int width_heli;
    unsigned int height_heli;
    unsigned int x_pos_bullet = 320;
    unsigned int y_pos_bullet = 0;
    unsigned int width_bullet;
    unsigned int height_bullet;
    unsigned char* _sprite_heli;
    unsigned char* _bitmap_bullet;
    unsigned char* _bitmap_bullet_explotion;
    unsigned int _t0_leap;
    unsigned int _t0_shoot;
    unsigned int _x0_shoot;
    unsigned int _x_exp;
    unsigned int _y_exp;
    unsigned int _t_explotion = 0;
};

class Menu_pointer {
  public:
    Menu_pointer(unsigned int w, unsigned int h, unsigned char* bitmap);

    void set_to(unsigned int x, unsigned int y, unsigned int color);

    unsigned int x_pos;
    unsigned int y_pos;

  private:
    unsigned char* _bitmap;
    unsigned int _width;
    unsigned int _height;
    unsigned int _x_prev;
    unsigned int _y_prev;
};

#endif
