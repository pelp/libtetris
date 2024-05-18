#include "libtetris.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

#define FPS 30
#define RED 1
#define YELLOW 2
#define GREEN 3
#define BLUE 4
#define PINK 5
#define CYAN 6
#define WHITE 7

int lose(tetris_t *game)
{
    endwin();
    printf("YOU LOSE\n");
    printf("Score: %d\n", game->lines);
    return 1;
}

void print_state(tetris_t *game)
{
    for (int i = 0; i < game->height; i++)
    {
        for (int j = 0; j < game->width * 2; j++)
        {
            char state = read_game(game, j / 2, i);
            if (state > 0)
            {
                attron(COLOR_PAIR(state));
            }
            mvaddch(i, j + 1, ' ');
            if (state > 0)
            {
                attroff(COLOR_PAIR(state));
            }
            // printf("(%d, %d) -> %d, (%d, %d)\n", piece_coord_x, piece_coord_y, piece_coord, game->rotated.width, game->rotated.height);
        }
    }
    for (int i = 0; i < game->width * 2; i++)
    {
        mvaddch(game->height, i + 1, (i/2 >= game->x + game->ox && i/2 < game->x + game->ox + game->rotated.width) ? '=' : ' ');
    }
    for (int i = 0; i < game->height; i++)
    {
        mvaddch(i, 0, '|');
        mvaddch(i, game->width*2+1, '|');
    }
    refresh();
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    timeout(0);
    cbreak();
    noecho();
    start_color();
    init_pair(RED, COLOR_WHITE, COLOR_RED);
    init_pair(BLUE, COLOR_WHITE, COLOR_BLUE);
    init_pair(GREEN, COLOR_WHITE, COLOR_GREEN);
    init_pair(YELLOW, COLOR_WHITE, COLOR_YELLOW);
    init_pair(PINK, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(CYAN, COLOR_WHITE, COLOR_CYAN);
    init_pair(WHITE, COLOR_WHITE, COLOR_WHITE);

    tetris_t game;
    init(&game, 10, 20);
    print_state(&game);
    int counter = 0;
    while (true)
    {
        char c = getch();
        switch (c)
        {
            case 'h':
                left(&game);
                break;
            case 'l':
                right(&game);
                break;
            case 'k':
                rotate(&game);
                break;
            case 'j':
                if (step(&game) == 1)
                {
                    return lose(&game);
                }
                break;
            case ' ':
                int rc;
                while ((rc = step(&game)) == 0);
                if (rc == 1)
                {
                    return lose(&game);
                }
                break;

        }
        if (counter > FPS)
        {
            counter = 0;
            if (step(&game) == 1)
            {
                return lose(&game);
            }
        }
        print_state(&game);
        usleep(1000000 / FPS);
        counter++;
    }
    endwin();
    return 0;
}
