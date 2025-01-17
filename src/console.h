#include "emu/et3400.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifdef _WIN32

#include <windows.h>

void gotoxy(int x, int y) {
    COORD p = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
}

#else

#include <term.h>
#include <unistd.h>

void gotoxy(int x, int y) {
    int err;
    if (!cur_term)
        if (setupterm(NULL, STDOUT_FILENO, &err) == ERR)
            return;
    putp(tparm(tigetstr("cup"), y, x, 0, 0, 0, 0, 0, 0, 0));
}

#endif

et3400emu* emu;

void render_memory(et3400emu* emu);

void sigintHandler(int sig_num) {
    emu->stop();
}
