#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#include <i86.h>
#include <stdint.h>
union REGS registers;

enum gamestate_t {EXPLORING, TALKING, CHOOSING, ATTACKING, DEFENDING};
typedef enum gamestate_t gamestate_t;
gamestate_t gamestate;

int player_x;
int player_y;

struct key_t{
    int scancode;
    char ascii;
};
typedef struct key_t key_t;

uint8_t check_key()
{
    registers.h.ah = 1;
    int86(0x16,&registers,&registers);
    return registers.w.ax;
}

key_t get_key()
{
    key_t key;
    registers.h.ah = 0;
    int86(0x16,&registers,&registers);
    key.ascii = registers.h.al;
    key.scancode = registers.h.ah;
    return key;
}

typedef struct enemy_t{
    uint8_t x;
    uint8_t y;
    char name[20];
    uint32_t health;
    unsigned int attack;
    int defense;
    int xp_reward;
    int gold_reward;
} enemy_t;

typedef void (*TriggerFunc)();

typedef void (*DrawFunc)();

typedef struct screen_t
{
    uint8_t startx;
    uint8_t starty;
    DrawFunc draw;
    TriggerFunc triggers[5];
} screen_t;

screen_t screens[10];  // i am using the color attribute for screen transition characters as the index into this sometimes

screen_t* current_screen;

#endif