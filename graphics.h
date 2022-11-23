#include "common.h"

#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

typedef enum color_t {Black,Blue,Green,Cyan,Red,Magenta,Brown,LightGrey,DarkGrey,LightBlue,LightGreen,LightCyan,LightRed,LightMagenta,Yellow,White} TerminalColor;

uint16_t* __far terminalbuffer;
uint16_t* __far currentpage;

enum gamestate_t {EXPLORING, CONVERSING, CHOOSING, ATTACKING, DEFENDING};
typedef enum gamestate_t gamestate_t;
gamestate_t gamestate;

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25

#define PAGESIZE ((SCREEN_WIDTH*SCREEN_HEIGHT))
#define PAGE(page) (terminalbuffer + ((PAGESIZE)*(page)))
#define set_page(page) currentpage = PAGE(page)
#define getchar_at(page,x,y) (char)((*(PAGE(page)+(SCREEN_WIDTH*(y)) + (x))))
#define getattrib_at(page,x,y) (char)((*(PAGE(page)+(SCREEN_WIDTH*(y)) + (x)))>>8)
#define set_transitionindex(index) terminal_color = index

typedef struct attrib_t{
    char fg;
    char bg;
}attrib_t;


#define attrib_to_colors(attrib,out)  out.fg = (attrib & 0x0F); out.bg = ((attrib & 0xF0) >> 4);

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

inline void set_color(TerminalColor foreground, TerminalColor background)
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
    for(x = x1; x < x1+length; x++)
    {
        putchar_at(c,x,y1);
    }
}

void print_vert_line(char c, uint8_t x1, uint8_t y1, uint8_t height)
{
    int y;
    for(y = y1; y < y1+height; y++)
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
    for(i = y1+1; i <= y2; i++)
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

//the one thing i've stolen for this lol i need to get better at assembly
void set_controller_bits();
#pragma aux set_controller_bits = \
"mov ax,40H" \
"mov es,ax" \
"mov dx,es:[063H]" \
"add dx,4" \
"mov al,es:[065H]" \
"and al,0dfH" \
"out dx,al" \
"mov es:[065H],al" \
modify [ ax es dx ];

#define clear_screen() set_video_mode(0x03); set_video_mode(0x01); set_controller_bits()

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

    set_controller_bits();
    set_color(White,Black);
    return;
}

void reset_screen()
{
    set_video_mode(0x03);
    return;
}
#endif