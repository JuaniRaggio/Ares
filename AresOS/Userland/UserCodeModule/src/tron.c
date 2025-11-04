#include <stdio.h>
#include <syscalls.h>
#include <tron.h>

/* Game grid dimensions and rendering */
#define GRID_WIDTH 64
#define GRID_HEIGHT 48
#define CELL_SIZE 16
#define GAME_SPEED_TICKS 2 /* ~9 updates per second (2/18) */
#define for_ever for (;;)

/* Color definitions */
#define PLAYER1_COLOR 0x00FF00
#define PLAYER2_COLOR 0xFF0000
#define TRAIL_COLOR_P1 0x008800
#define TRAIL_COLOR_P2 0x880000
#define BACKGROUND_COLOR 0x000000

typedef enum {
        DIR_UP,
        DIR_DOWN,
        DIR_LEFT,
        DIR_RIGHT,
} direction_t;

typedef struct {
        int16_t x;
        int16_t y;
        direction_t dir;
        uint32_t color;
        uint32_t trail_color;
        uint8_t alive;
} player_t;

typedef struct {
        uint8_t grid[GRID_HEIGHT][GRID_WIDTH];
        player_t player1;
        player_t player2;
        uint8_t game_over;
        uint8_t winner;
} game_state_t;

static game_state_t game;

void tron_init(void) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
                for (int x = 0; x < GRID_WIDTH; x++) {
                        game.grid[y][x] = 0;
                }
        }

        game.player1.x           = GRID_WIDTH / 4;
        game.player1.y           = GRID_HEIGHT / 2;
        game.player1.dir         = DIR_RIGHT;
        game.player1.color       = PLAYER1_COLOR;
        game.player1.trail_color = TRAIL_COLOR_P1;
        game.player1.alive       = 1;

        game.player2.x           = (GRID_WIDTH * 3) / 4;
        game.player2.y           = GRID_HEIGHT / 2;
        game.player2.dir         = DIR_LEFT;
        game.player2.color       = PLAYER2_COLOR;
        game.player2.trail_color = TRAIL_COLOR_P2;
        game.player2.alive       = 1;

        game.game_over = 0;
        game.winner    = 0;

        game.grid[game.player1.y][game.player1.x] = 1;
        game.grid[game.player2.y][game.player2.x] = 2;
}

void tron_render(void) {
        syscall_clear();

        for (int y = 0; y < GRID_HEIGHT; y++) {
                for (int x = 0; x < GRID_WIDTH; x++) {
                        if (game.grid[y][x] != 0) {
                                uint32_t color = (game.grid[y][x] == 1)
                                                     ? TRAIL_COLOR_P1
                                                     : TRAIL_COLOR_P2;
                                syscall_draw_rect(x * CELL_SIZE, y * CELL_SIZE,
                                                  CELL_SIZE, CELL_SIZE, color);
                        }
                }
        }

        if (game.player1.alive) {
                syscall_draw_rect(game.player1.x * CELL_SIZE,
                                  game.player1.y * CELL_SIZE, CELL_SIZE,
                                  CELL_SIZE, game.player1.color);
        }

        if (game.player2.alive) {
                syscall_draw_rect(game.player2.x * CELL_SIZE,
                                  game.player2.y * CELL_SIZE, CELL_SIZE,
                                  CELL_SIZE, game.player2.color);
        }
}

void tron_handle_input(char key) {
        switch (key) {
        case 'w':
        case 'W':
                if (game.player1.dir != DIR_DOWN)
                        game.player1.dir = DIR_UP;
                break;
        case 's':
        case 'S':
                if (game.player1.dir != DIR_UP)
                        game.player1.dir = DIR_DOWN;
                break;
        case 'a':
        case 'A':
                if (game.player1.dir != DIR_RIGHT)
                        game.player1.dir = DIR_LEFT;
                break;
        case 'd':
        case 'D':
                if (game.player1.dir != DIR_LEFT)
                        game.player1.dir = DIR_RIGHT;
                break;
        case 'i':
        case 'I':
                if (game.player2.dir != DIR_DOWN)
                        game.player2.dir = DIR_UP;
                break;
        case 'k':
        case 'K':
                if (game.player2.dir != DIR_UP)
                        game.player2.dir = DIR_DOWN;
                break;
        case 'j':
        case 'J':
                if (game.player2.dir != DIR_RIGHT)
                        game.player2.dir = DIR_LEFT;
                break;
        case 'l':
        case 'L':
                if (game.player2.dir != DIR_LEFT)
                        game.player2.dir = DIR_RIGHT;
                break;
        }
}

void tron_update_player(player_t *player, uint8_t player_id) {
        if (!player->alive)
                return;

        int16_t new_x = player->x;
        int16_t new_y = player->y;

        switch (player->dir) {
        case DIR_UP:
                new_y--;
                break;
        case DIR_DOWN:
                new_y++;
                break;
        case DIR_LEFT:
                new_x--;
                break;
        case DIR_RIGHT:
                new_x++;
                break;
        }

        if (new_x < 0 || new_x >= GRID_WIDTH || new_y < 0 ||
            new_y >= GRID_HEIGHT || game.grid[new_y][new_x] != 0) {
                player->alive = 0;
                return;
        }

        player->x               = new_x;
        player->y               = new_y;
        game.grid[new_y][new_x] = player_id;
}

void tron_update(void) {
        if (game.game_over)
                return;

        tron_update_player(&game.player1, 1);
        tron_update_player(&game.player2, 2);

        if (!game.player1.alive || !game.player2.alive) {
                game.game_over = 1;
                if (!game.player1.alive && !game.player2.alive) {
                        game.winner = 0;
                } else if (!game.player1.alive) {
                        game.winner = 2;
                } else {
                        game.winner = 1;
                }
        }
}

void tron_show_menu(void) {
        char *logo = " ,d                                        \n 88         "
                     "                               \n MM88MMM 8b,dPPYba,  "
                     ",adPPYba,  8b,dPPYba,   \n 88    88P'   \"Y8 a8\"     "
                     "\"8a 88P'   `\"8a  \n 88    88         8b       d8 88    "
                     "   88  \n 88,   88         \"8a,   ,a8\" 88       88  \n "
                     "\"Y888 88          `\"YbbdP\"'  88       88  \n";
        syscall_clear();
        printf("\n");
        printf(logo);
        printf("\n");
        printf("  TRON LIGHT CYCLES\n\n");
        printf("  HOW TO PLAY:\n");
        printf("  - Two players control light cycles\n");
        printf("  - Each cycle leaves a trail behind\n");
        printf("  - Crash into a trail or wall and you lose\n");
        printf("  - Last player alive wins!\n\n");
        printf("  CONTROLS:\n");
        printf("  Player 1 (Green):  W/A/S/D\n");
        printf("  Player 2 (Red):    I/J/K/L\n");
        printf("  Quit game:         Q\n\n");
        printf("  Press SPACE to start...\n");

        for_ever {
                char c = getchar();
                if (c == ' ') {
                        break;
                }
                if (c == 'q' || c == 'Q') {
                        return;
                }
        }
}

void tron_game(void) {
        tron_show_menu();
        tron_init();

        uint64_t last_update  = syscall_get_ticks();
        uint8_t frame_counter = 0;

        syscall_clear();
        tron_render();

        while (!game.game_over) {
                uint64_t now = syscall_get_ticks();

                /* Process all pending input */
                char c;
                while ((c = getchar()) != 0) {
                        if (c == 'q' || c == 'Q') {
                                return;
                        }
                        tron_handle_input(c);
                }

                /* Update game at fixed intervals */
                if (now - last_update >= GAME_SPEED_TICKS) {
                        tron_update();
                        tron_render();
                        last_update = now;
                        frame_counter++;
                }
        }

        syscall_clear();
        printf("\n");
        printf("  ============================\n");
        printf("  |        GAME OVER         |\n");
        printf("  ============================\n\n");

        if (game.winner == 0) {
                printf("  Draw! Both players crashed!\n\n");
        } else {
                printf("  Player %d wins!\n\n", game.winner);
        }

        printf("  Total frames: %d\n\n", frame_counter);
        printf("  Press any key to return to shell...\n");
        while (getchar() == 0)
                ;
        getchar();
        syscall_clear();
}
