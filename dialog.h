typedef struct dialog_t{
    char* label;
    char* speaker;
    char* text;
    struct dialog_t* option_a;
    struct dialog_t* option_b;
}dialog_t;


dialog_t lines[] = {
    (dialog_t){NULL,"FLOWEY","Howdy! I'm FLOWEY. FLOWEY the FLOWER!",&(lines[1]),NULL},
    (dialog_t){NULL,"FLOWEY","Hmmm...",&(lines[2]),NULL},
    (dialog_t){NULL,"FLOWEY","You're new to the UNDERGROUND, aren'tcha?",&(lines[3]),NULL},
    (dialog_t){NULL,"FLOWEY","Golly, you must be so confused.",&(lines[4]),NULL},
    (dialog_t){NULL,"FLOWEY","Someone ought to teach you how things work around here!",&(lines[5]),NULL},
    (dialog_t){NULL,"FLOWEY","I guess little old me will have to do.",&(lines[6]),NULL},
    (dialog_t){NULL,"FLOWEY","Ready?\nHere we go!",NULL,NULL},
};

void read_dialog(dialog_t* dialog)
{
    
}