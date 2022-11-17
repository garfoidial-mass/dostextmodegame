#define _M_I86
#define _DOSBOX
#include <stdint.h>
#include <stdlib.h>
#include <i86.h>

typedef enum color_t {Black,Blue,Green,Cyan,Red,Magenta,Brown,LightGrey,DarkGrey,LightBlue,LightGreen,LightCyan,LightRed,LightMagenta,Yellow,White} TerminalColor;

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25

#ifdef _DOSBOX
uint16_t* __far terminalbuffer;
uint16_t* __far currentpage;
#else
uint16_t* terminalbuffer;
uint16_t* currentpage;
#endif
union REGS registers;

struct key_t{
    int scancode;
    char ascii;
};

typedef struct key_t key_t;

enum gamestate_t {EXPLORING, CHOOSING, ATTACKING, DEFENDING};
typedef enum gamestate_t gamestate_t;
gamestate_t gamestate;

#define PAGESIZE (SCREEN_WIDTH*SCREEN_HEIGHT*2)
#define PAGE(page) (terminalbuffer + ((PAGESIZE)*(page)))
#define set_page(page) currentpage = PAGE(page)
#define getchar_at(page,x,y) (char)((*(PAGE(page)+(SCREEN_WIDTH*(y)) + (x))))
#define getattrib_at(page,x,y) (char)((*(PAGE(page)+(SCREEN_WIDTH*(y)) + (x)))>>8)

typedef struct attrib_t{
    char fg;
    char bg;
}attrib_t;


#define attrib_to_colors(attrib,out)  out.fg = (attrib & 0x0F); out.bg = ((attrib & 0x70) >> 4);

uint8_t terminal_color;
uint8_t cursor_x;
uint8_t cursor_y;

void set_video_mode(uint8_t mode)
{
    registers.h.ah = 0;
    registers.h.al = mode;
    int86(0x10,&registers,&registers);
    return;
}

void clear_screen()
{
    registers.h.ah = 0x1;
    registers.h.al = 0;
    int86(0x10,&registers,&registers);
}

void set_color(TerminalColor foreground, TerminalColor background)
{
    terminal_color = (background << 4) | (foreground & 0x0F);
    return;
}

void putchar_at(char c, uint8_t x, uint8_t y)
{
    uint16_t vgac = c | (terminal_color << 8);

    *(currentpage + (SCREEN_WIDTH*y) + x) = vgac;
    return;
}

void putchar(char c)
{
    if(cursor_x >= SCREEN_WIDTH)
    {
        cursor_y += 1;
        cursor_x = 0;
    }
    else
    {
        cursor_x+=1;
    }

    putchar_at(c,cursor_x,cursor_y);
    return;
}

void print_string(const char* s)
{   
    int i;
    for(i = 0; s[i] != '\0'; i++)
    {
        putchar(s[i]);
    }
}

void print_hor_line(char c, uint8_t x1, uint8_t y1, uint8_t length)
{
    int x;
    for(x = x1; x <= x1+length; x++)
    {
        putchar_at(c,x,y1);
    }
}

void print_vert_line(char c, uint8_t x1, uint8_t y1, uint8_t height)
{
    int y;
    for(y = y1; y <= y1+height; y++)
    {
        putchar_at(c,x1,y);
    }
}

void print_rect(char c, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    int i;
    for(i = x1; i <= x2; i++)
    {
        putchar_at(c, i, y1);
        putchar_at(c, i, y2);
    }
    for(i = y1+1; i < y2; i++)
    {
        putchar_at(c, x1, i);
        putchar_at(c, x2, i);
    }
}


struct boxstyle_t
{
    char top; 
    char bottom; 
    char left;
    char right;
    char tr;
    char tl;
    char br;
    char bl;
};

typedef struct boxstyle_t boxstyle_t;



void print_fancy_box(boxstyle_t style, uint8_t x1, uint8_t y1, uint8_t cwidth, uint8_t cheight)
{
    uint8_t width = cwidth-1;
    uint8_t height = cheight-1;
    print_hor_line(style.top,x1+1,y1,width-2);
    print_hor_line(style.bottom,x1+1,y1+height,width-2);
    print_vert_line(style.left,x1,y1+1,height-2);
    print_vert_line(style.left,x1+width,y1+1,height-2);
    putchar_at(style.tl,x1,y1);
    putchar_at(style.tr,x1+width,y1);
    putchar_at(style.bl,x1,y1+height);
    putchar_at(style.br,x1+width,y1+height);
}


void print_filled_rect(char c, uint8_t x1, uint8_t y1, uint8_t width, uint8_t height)
{
    int x, y;
    for(y = y1; y < y1+height; y++)
    {
        for(x = x1; x < x1+width; x++)
        {
            putchar_at(c, x, y);
        }
    }
}

void copy_pages()
{
    int i;
    for(i = 0; i < PAGESIZE; i++)
    {
       *(PAGE(0)+i) = *(PAGE(1)+i); 
    }
}

void init_screen()
{
    #ifdef _DOSBOX
    terminalbuffer = (uint16_t *) 0xB8000000;
    #else
    terminalbuffer = (uint16_t *) 0xB8000;
    #endif
    set_page(0);
    set_video_mode(0x01);
    set_color(White,Black);
    return;
}

void reset_screen()
{
    set_video_mode(0x03);
    return;
}

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
    if(getchar_at(4,player_x+move_x,player_y+move_y) == ' ')
    {
        set_color(colors.fg,colors.bg);
        putchar_at(getchar_at(1,player_x,player_y),player_x,player_y);
        player_x += move_x;
        player_y += move_y; 
    }
    attrib = getattrib_at(1,player_x,player_y);
    attrib_to_colors(attrib,colors);
    set_color(White,colors.bg);
    putchar_at('@',player_x,player_y);
    return;
}


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
    enemy_t enemies[10];
    DrawFunc draw;
};

typedef struct screen_t screen_t;


void draw_screen0()
{
    set_color(Brown,Brown);
    print_filled_rect(' ',0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    set_color(Black,Black);
    print_filled_rect(' ',0,0,SCREEN_WIDTH,4);
    print_filled_rect(' ',0,20,SCREEN_WIDTH,5);
    set_page(4);
    print_filled_rect('x',0,0,SCREEN_WIDTH,4);
    print_filled_rect('x',0,20,SCREEN_WIDTH,5);
    set_page(1);
    set_color(LightGrey,LightGrey);
    print_filled_rect(' ',7,7,17,4);
    set_color(Green,Green);
    print_filled_rect(' ',9,7,13,4);
    set_color(Yellow,Green);
    print_filled_rect('&',11,7,3,2);
    return;
}

screen_t screens[10];

void setupscreens()
{
    screen_t screen0;
    screen0.draw = &draw_screen0;
    screens[0] = screen0;
}

#define draw_screen(x) set_page(1); (screens[x].draw)(); copy_pages(); set_page(0); set_color(White,Black);

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
    player_y = 11;
    player_x = 11;
    init_screen();
    draw_screen(0);
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