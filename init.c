#define _M_IX86
#define _DOSBOX
#include <stdlib.h>
#include "graphics.h"
#include "common.h"
//#include <conio.h>


struct key_t{
    int scancode;
    char ascii;
};
typedef struct key_t key_t;



//get input

key_t get_key()
{
    key_t key;
    registers.h.ah = 0;
    int86(0x16,&registers,&registers);
    key.ascii = registers.h.al;
    key.scancode = registers.h.ah;
    return key;
}

int player_x;
int player_y;
int c;
char buf[10];

struct enemy_t{
    uint8_t x;
    uint8_t y;
    char sprite;
    char name[20];
    uint32_t health;
    unsigned int attack;
    int defense;
    int xp_reward;
    int gold_reward;
};
typedef struct enemy_t enemy_t;


typedef void (*DrawFunc)();

struct screen_t
{
    uint8_t startx;
    uint8_t starty;
    enemy_t enemies[10]; // change to npcs
    DrawFunc draw;
};

typedef struct screen_t screen_t;

screen_t screens[10];  // i am using the color attribute for screen transition characters as the index into this sometimes

#define draw_screen(x) set_page(1); (screens[x].draw)(); copy_pages(); set_page(0); set_color(White,Black)

void change_screen(int index)
{
    screen_t screen = screens[index];
    player_x = screen.startx;
    player_y = screen.starty;
    clear_screen();
    draw_screen(index);
    return;
}

void move_player()
{
    // make sure the current page is 0 when  calling this
    int move_x;
    int move_y;
    char attrib;
    attrib_t colors;
    key_t key;
    move_x = 0;
    move_y = 0;
    key = get_key();
    switch (key.ascii)
    {
    case 'e':
        c=0;
        return;
        break;
    break;
    default:
        break;
    }
    switch (key.scancode)
    {
        case 0x48: // up
            move_y = -1;
            break;
        case 0x4B: // left
            move_x = -1;
            break;
        case 0x4D: // right
            move_x = 1;
            break;
        case 0x50: // down
            move_y = 1;
            break;
        default:
            break;
    }
    attrib = getattrib_at(1,player_x,player_y);
    attrib_to_colors(attrib,colors);
    //print_string(itoa(colors.bg,buf,10));
    //char 21 is screen transition char
    switch(getchar_at(4,player_x+move_x,player_y+move_y))
    {
        case 21:
            change_screen(getattrib_at(4,player_x+move_x,player_y+move_y));
        break;

        case ' ':
            set_color(colors.fg,colors.bg);
            putchar_at(getchar_at(1,player_x,player_y),player_x,player_y);
            if((player_x+move_x >= 0 && player_x+move_x < SCREEN_WIDTH)&&((player_y+move_y >= 0 && player_y+move_y < SCREEN_HEIGHT)))
            {
                player_x += move_x;
                player_y += move_y;
            }
        break;
        default:
        break;
    }

    attrib = getattrib_at(1,player_x,player_y);
    attrib_to_colors(attrib,colors);
    set_color(White,colors.bg);
    putchar_at('@',player_x,player_y);
    return;
}


//219 is full block char

void draw_screen0()
{
    set_color(DarkGrey,DarkGrey);
    print_filled_rect(' ',2,5,SCREEN_WIDTH-4,15);
    print_filled_rect(' ',SCREEN_WIDTH-2,15,2,5);
    set_page(4);
    print_filled_rect('x',SCREEN_WIDTH-2,5,2,10);
    print_filled_rect('x',0,0,SCREEN_WIDTH,5);
    print_filled_rect('x',0,0,2,SCREEN_HEIGHT);
    print_filled_rect('x',0,20,SCREEN_WIDTH,5);
    set_transitionindex(1);
    print_vert_line(21,SCREEN_WIDTH,15,5);
    set_page(1);
    set_color(LightGrey,LightGrey);
    print_filled_rect(' ',12,10,16,5);
    set_color(Green,Green);
    print_filled_rect(' ',14,10,12,5);
    set_color(Yellow,Green);
    print_filled_rect('&',18,11,4,3);
    return;
}

void draw_screen1()
{
    set_page(4);
    print_filled_rect('x',0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    print_filled_rect(' ',0,15,SCREEN_WIDTH-5,4);
    set_transitionindex(0);
    print_vert_line(21,0,15,5);
    set_page(1);
    set_color(DarkGrey,DarkGrey);
    print_filled_rect(' ',0,15,SCREEN_WIDTH-5,4);
    return;
}

void setupscreens()
{
    screen_t screen0;
    screen_t screen1;
    screen0.draw = &draw_screen0;
    screen0.startx = 11;
    screen0.starty = 11;
    screens[0] = screen0;

    screen1.draw = &draw_screen1;
    screen1.startx = 1;
    screen1.starty = 16;
    screens[1] = screen1;
}

int main()
{
    boxstyle_t consoleboxstyle;
    consoleboxstyle.top = 205;
    consoleboxstyle.bottom = 205;
    consoleboxstyle.left = 186;
    consoleboxstyle.right = 186;
    consoleboxstyle.tl = 201;
    consoleboxstyle.tr = 187;
    consoleboxstyle.bl = 200;
    consoleboxstyle.br = 188;

    setupscreens();

    gamestate = EXPLORING;

    c = 1;
    init_screen();

    change_screen(0);
    while(c == 1)
    {
        switch(gamestate)
        {
            case EXPLORING:
                move_player();
            break;
        }
    }
    set_color(White,Black);
    reset_screen();
    return 0;
}