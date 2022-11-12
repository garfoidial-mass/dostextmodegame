#define _M_I86
#define _DOSBOX
#include <stdint.h>
#include <i86.h>

typedef enum color_t {Black,Blue,Green,Cyan,Red,Magenta,Brown,LightGrey,DarkGrey,LightBlue,LightGreen,LightCyan,LightRed,LightMagenta,Yellow,White} TerminalColor;

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#ifdef _DOSBOX
uint16_t* __far terminalbuffer;
uint16_t* __far currentpage;
#else
uint16_t* terminalbuffer;
uint16_t* currentpage;
#endif
union REGS registers;

#define PAGESIZE SCREEN_WIDTH*SCREEN_HEIGHT*2
#define PAGE(page) terminalbuffer + ((PAGESIZE)*(page))
#define set_page(page) currentpage = PAGE(page)

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

void print_filled_rect(char c, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    int x, y;
    for(y = y1; y <= y2; y++)
    {
        for(x = x1; x <= x2; x++)
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
    set_video_mode(0x03);
    set_color(White,Black);
    return;
}

void reset_screen()
{
    set_video_mode(0x03);
    return;
}

//get input

int get_key()
{
    registers.h.ah = 0;
    int86(0x16,&registers,&registers);
    return registers.h.al;
}
//survivial game inside house, you are being frozen over and trying to keep yourself warm, surive until rescue

int main()
{
    int c;
    init_screen();
    cursor_x = 10;
    cursor_y = 10;
    set_page(1); // draw background to page 1, copy to page 0
    print_filled_rect(' ',9,9,16,13);
    set_color(White,Green);
    set_page(0);
    copy_pages();
    putchar_at('@',10,10);
    c = get_key();
    while(c != 'e')
    {
        putchar(c);
        c = get_key();
    }
    set_color(White,Black);
    return 0;
}
