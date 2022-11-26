#ifndef DIALOG_H_INCLUDED
#define DIALOG_H_INCLUDED
#include "graphics.h"

struct dialog_t{
    char* label;
    char* speaker;
    char* text;
    struct dialog_t* option_a;
    struct dialog_t* option_b;
};

typedef struct dialog_t dialog_t;

boxstyle_t dialogueboxstyle;

char* lines[] = {
    "FLOWEY",
    "Howdy!\nI'm FLOWEY.\nFLOWEY the FLOWER!",
    "Hmmm...",
    "You're new to the UNDERGROUND, aren'tcha?",
    "Golly, you must be so confused.",
    "Someone ought to teach you how things work around here!",
    "I guess little old me will have to do.",
    "Ready?\nHere we go!"
};

#define init_dialog(ptr,l,s,t,a,b) (ptr)->label=l; (ptr)->speaker=s; (ptr)->text=t; (ptr)->option_a=a; (ptr)->option_b=b
dialog_t flowey_lines[7];


void cleanup_dialog()
{
    set_page(0);
    redraw_rect(0,0,SCREEN_WIDTH,8);
    putchar_at('@',player_x,player_y);
    gamestate = EXPLORING;
}

void continue_dialog(dialog_t* dialog)
{
    int i;
    cursor_x = 1;
    cursor_y = 1;
    
    print_filled_rect(' ',1,1,SCREEN_WIDTH-2,6);
    
    for(i = 0; dialog->text[i] != '\0'; i++)
    {
        if(cursor_x >= SCREEN_WIDTH-1)
        {
            cursor_x = 1;
            cursor_y++;
        }
        switch (dialog->text[i])
        {
        case '\n':
            cursor_x = 1;
            cursor_y++;
            get_key();
            break;

        default:
            putchar_at(dialog->text[i],cursor_x,cursor_y);
            delay(50);
            cursor_x++;
            break;
        }
    }
    get_key();
    if(dialog->option_a != NULL && dialog->option_b != NULL)
    {
        key_t key;
        cursor_x = 1;
        cursor_y = 5;
        print_string("1. ");
        print_string(dialog->option_a->label);
        cursor_x = 1;
        cursor_y = 6;
        print_string("2. ");
        print_string(dialog->option_b->label);
        key = get_key();
        while(key.ascii != '1' || key.ascii != '2')
        {
            key = get_key();
        }
        if(key.ascii == '1')
        {
            continue_dialog(dialog->option_a);
        }
        else
        {
            continue_dialog(dialog->option_b);
        }
    }
    else if(dialog->option_a == NULL && dialog->option_b == NULL)
    {
        cleanup_dialog();
    }
    else
    {
        continue_dialog(dialog->option_a);
    }
}



void start_dialog(dialog_t* dialog)
{
    while(check_key() != 0)
    {
        get_key();
    }
    gamestate = TALKING;
    set_color(White,Black);
    print_fancy_box(dialogueboxstyle,0,0,SCREEN_WIDTH,8);
    set_color(White,Black);
    print_filled_rect(' ',1,1,SCREEN_WIDTH-2,6);
    cursor_x = 1;
    cursor_y = 0;
    print_string(dialog->speaker);

    continue_dialog(dialog);
}
#endif