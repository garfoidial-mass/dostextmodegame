#define _M_IX86
#define _DOSBOX
#include <stdlib.h>
#include "graphics.h"
#include "common.h"
#include "dialog.h"

int c;
char buf[10];

//screens:
//0 - draw screen/ output screen
//1 - backgrounds
//4 - collision

#define draw_screen(x) set_page(1); (screens[x].draw)(); copy_pages(); set_page(0); set_color(White,Black)

void change_screen(int index)
{
    screen_t screen = screens[index];
    player_x = screen.startx;
    player_y = screen.starty;
    clear_screen();
    draw_screen(index);
    current_screen = &(screens[index]);
    registers.h.ah = 0x01;
    registers.h.ch = 0x3F;
    int86(0x10,&registers,&registers);
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
    // make smiley face npc trigger char
    switch(getchar_at(4,player_x+move_x,player_y+move_y))
    {
        case 21:
            change_screen(getattrib_at(4,player_x+move_x,player_y+move_y));
        break;

        case 1:
            //index into triggers with color attribute
            (current_screen->triggers[getattrib_at(4,player_x+move_x,player_y+move_y)])();
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
    set_transitionindex(2);
    print_hor_line(21,31,14,6);
    set_page(1);
    set_color(DarkGrey,Black);
    print_fancy_box(dialogueboxstyle,30,9,5,7);
    set_color(DarkGrey,DarkGrey);
    print_filled_rect(' ',0,15,SCREEN_WIDTH-5,4);
    return;
}


void draw_screen2()
{
    set_color(DarkGrey,DarkGrey);
    print_filled_rect(' ',12,10,16,5);
    set_color(LightGrey,LightGrey);
    print_filled_rect(' ',15,10,10,5);
    set_color(Green,Green);
    print_filled_rect(' ',17,10,6,3);
    set_color(LightGreen,Green);
    putchar_at(179,20,10);
    set_color(Yellow,Black);
    putchar_at('*',20,9);
    set_page(4);
    set_transitionindex(0);
    print_filled_rect(1,17,9,6,4);
    set_page(1);
    return;
}

void trigger1_screen2()
{
    start_dialog(&(flowey_lines[0]));
    set_page(4);
    print_filled_rect(' ',17,9,6,4);
    set_page(0);
    return;
}

void setupscreens()
{
    screen_t screen0;
    screen_t screen1;
    screen_t screen2;

    screen0.draw = &draw_screen0;
    screen0.startx = 11;
    screen0.starty = 11;
    screens[0] = screen0;

    screen1.draw = &draw_screen1;
    screen1.startx = 1;
    screen1.starty = 16;
    screens[1] = screen1;

    screen2.draw = &draw_screen2;
    screen2.startx = SCREEN_WIDTH/2;
    screen2.starty = SCREEN_HEIGHT-1;
    screen2.triggers[0] = &trigger1_screen2;
    screens[2] = screen2;
}

int main()
{

    init_dialog(&(flowey_lines[0]),NULL,lines[0],lines[1],&(flowey_lines[1]),NULL);
    init_dialog(&(flowey_lines[1]),NULL,lines[0],lines[2],&(flowey_lines[2]),NULL);
    init_dialog(&(flowey_lines[2]),NULL,lines[0],lines[3],&(flowey_lines[3]),NULL);
    init_dialog(&(flowey_lines[3]),NULL,lines[0],lines[4],&(flowey_lines[4]),NULL);
    init_dialog(&(flowey_lines[4]),NULL,lines[0],lines[5],&(flowey_lines[5]),NULL);
    init_dialog(&(flowey_lines[5]),NULL,lines[0],lines[6],&(flowey_lines[6]),NULL);
    init_dialog(&(flowey_lines[6]),NULL,lines[0],lines[7],NULL,NULL);



    dialogueboxstyle.top = 205;
    dialogueboxstyle.bottom = 205;
    dialogueboxstyle.left = 186;
    dialogueboxstyle.right = 186;
    dialogueboxstyle.tl = 201;
    dialogueboxstyle.tr = 187;
    dialogueboxstyle.bl = 200;
    dialogueboxstyle.br = 188;
    

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

            default:
            break;
        }
    }
    set_color(White,Black);
    reset_screen();
    return 0;
}