#include<ncurses.h>

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    WINDOW * log = newwin(LINES,COLS/2,0,0);
    box(log, 0, 0);
    WINDOW * bestNet = newwin(LINES/3, COLS/2 ,0, COLS/2);
    box(bestNet, 0, 0);
    WINDOW * status = newwin(LINES*2/3, COLS/2 ,LINES/3, COLS/2);
    box(status, 0, 0);
    refresh();
    wrefresh(log);
    wrefresh(bestNet);
    wrefresh(status);
    getch();
    endwin();
}
