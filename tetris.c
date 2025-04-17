#include <stdio.h>
#include <stdlib.h> 
#include <time.h> 
#include <termios.h> 
#include <unistd.h> 
#include <sys/select.h>
#include <sys/ioctl.h>  
#include <string.h>

#define WIDTH 10
#define HEIGHT 20  
#define BLOCK "\033[32m[]\033[0m"
#define FILLED "[]"              
int grid[HEIGHT][WIDTH] = {0}; 
struct termios original_termios;  

typedef struct {
    int coords[4][2];
} Rotation;

typedef struct {
    Rotation rotations[4];
    int rotation_count;
    int color;
} Shape;

Shape shapes[7];

typedef struct {
    int x, y;
    int rotation;
    int shape;
} Piece;

Piece current;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disable_raw_mode);
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int get_terminal_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

void init_shapes() {
    
    // I-shape (cyan)
    shapes[0].rotation_count = 2;
    shapes[0].rotations[0] = (Rotation){{{0,0}, {0,1}, {0,2}, {0,3}}};
    shapes[0].rotations[1] = (Rotation){{{0,0}, {1,0}, {2,0}, {3,0}}};
    shapes[0].color = 36;

    // O-shape (yellow)
    shapes[1].rotation_count = 1;
    shapes[1].rotations[0] = (Rotation){{{0,0}, {0,1}, {1,0}, {1,1}}};
    shapes[1].color = 37;

    // T-shape (magenta)
    shapes[2].rotation_count = 4;
    shapes[2].rotations[0] = (Rotation){{{1,0}, {0,1}, {1,1}, {2,1}}};
    shapes[2].rotations[1] = (Rotation){{{0,1}, {1,0}, {1,1}, {1,2}}};
    shapes[2].rotations[2] = (Rotation){{{1,2}, {0,1}, {1,1}, {2,1}}};
    shapes[2].rotations[3] = (Rotation){{{2,1}, {1,0}, {1,1}, {1,2}}};
    shapes[2].color = 35;

    // L-shape (orange)
    shapes[3].rotation_count = 4;
    shapes[3].rotations[0] = (Rotation){{{0,0}, {0,1}, {0,2}, {1,2}}};
    shapes[3].rotations[1] = (Rotation){{{0,1}, {1,1}, {2,1}, {0,0}}};
    shapes[3].rotations[2] = (Rotation){{{0,0}, {1,0}, {1,1}, {1,2}}};
    shapes[3].rotations[3] = (Rotation){{{0,1}, {1,1}, {2,1}, {2,2}}};
    shapes[3].color = 33;

    // J-shape (blue)
    shapes[4].rotation_count = 4;
    shapes[4].rotations[0] = (Rotation){{{1,0}, {1,1}, {1,2}, {0,2}}};
    shapes[4].rotations[1] = (Rotation){{{0,1}, {1,1}, {2,1}, {0,2}}};
    shapes[4].rotations[2] = (Rotation){{{0,0}, {1,0}, {0,1}, {0,2}}};
    shapes[4].rotations[3] = (Rotation){{{0,0}, {1,0}, {2,0}, {2,1}}};
    shapes[4].color = 34;

    // S-shape (green)
    shapes[5].rotation_count = 2;
    shapes[5].rotations[0] = (Rotation){{{1,0}, {2,0}, {0,1}, {1,1}}};
    shapes[5].rotations[1] = (Rotation){{{0,0}, {0,1}, {1,1}, {1,2}}};
    shapes[5].color = 32;

    // Z-shape (red)
    shapes[6].rotation_count = 2;
    shapes[6].rotations[0] = (Rotation){{{0,0}, {1,0}, {1,1}, {2,1}}};
    shapes[6].rotations[1] = (Rotation){{{1,0}, {0,1}, {1,1}, {0,2}}};
    shapes[6].color = 31;
}

void place_piece_in_grid(Piece *p) {
    Rotation rot = shapes[p->shape].rotations[p->rotation];
    for (int i = 0; i < 4; i++) {
        int x = p->x + rot.coords[i][0];
        int y = p->y + rot.coords[i][1];
        if (y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH)
            grid[y][x] = 1;
    }
}

void clear_lines() {
    for (int y = HEIGHT - 1; y >= 0; y--) {
        int full = 1;
        for (int x = 0; x < WIDTH; x++) {
            if (!grid[y][x]) {
                full = 0;
                break;
            }
        }

        if (full) {
            
            for (int row = y; row > 0; row--) {
                for (int col = 0; col < WIDTH; col++) {
                    grid[row][col] = grid[row - 1][col];
                }
            }

            for (int col = 0; col < WIDTH; col++) grid[0][col] = 0;

            y++;
        }
    }
}

int can_move(Piece *p, int dx, int dy, int new_rotation) {
    Shape *shape = &shapes[p->shape];
    Rotation rot = shape->rotations[new_rotation];

    for (int i = 0; i < 4; i++) {
        int x = p->x + rot.coords[i][0] + dx;
        int y = p->y + rot.coords[i][1] + dy;

        if (x < 0 || x >= WIDTH || y >= HEIGHT) return 0;
        if (y >= 0 && grid[y][x]) return 0;
    }
    return 1;
}

void spawn_piece() {
    current.shape = rand() % 7;
    current.rotation = 0;
    current.x = WIDTH / 2 - 2;
    current.y = -2; 

    if (!can_move(&current, 0, 0, current.rotation)) {
        // printf("\033[H\033[JGame Over!\n");
        printf("Game Over!\n");
        exit(0);
    }
}

void print_title() {
    printf("\033[H"); 
    printf("\033[36m"); // Cyan
    printf(
        " _____ _____ _____ ____  ___ ____  \n"
        "|_   _| ____|_   _|  _ \\|_ _/ ___|\n"
        "  | | |  _|   | | | |_) || |\\___ \\ \n"
        "  | | | |___  | | |  _ < | | ___) |\n"
        "  |_| |_____| |_| |_| \\_\\___|____/ \n"
        "     Inspired by the Electronika 60\n"
    );
    printf("\033[0m");
}


void draw() {
    printf("\033[H"); 

    int terminal_width = get_terminal_width();
    int board_width = WIDTH * 2 + 4; 
    int padding = (terminal_width - board_width) / 2;
    if (padding < 0) padding = 0;

    // Top border
    printf("%*s<!", padding, "");
    for (int i = 0; i < WIDTH; i++) printf(" .");
    printf("!>\n");

    // Playfield
    for (int y = 0; y < HEIGHT; y++) {
        printf("%*s<!", padding, "");  // Left border with padding

        for (int x = 0; x < WIDTH; x++) {
            int is_current = 0;
            Rotation r = shapes[current.shape].rotations[current.rotation];

            for (int i = 0; i < 4; i++) {
                int px = current.x + r.coords[i][0];
                int py = current.y + r.coords[i][1];
                if (px == x && py == y) {
                    is_current = 1;
                    break;
                }
            }

            if (is_current) {
                int color = shapes[current.shape].color;
                printf("\033[%dm[]\033[0m", color);
            } else if (grid[y][x]) {
                printf("\033[%dm[]\033[0m", grid[y][x]);
            } else {
                printf(" .");
            }
        }

        printf("!>\n");  // Right border
    }

    // Bottom border
    printf("%*s<!", padding, "");
    for (int i = 0; i < WIDTH; i++) printf("==");
    printf("!>\n");

    // Bottom decoration
    printf("%*s", padding, "");
    printf("  \\/\\/\\/\\/\\/\\/\\/\\/\\/\\/  \n");
    
}



void handle_input() {
    char c;
    if (read(STDIN_FILENO, &c, 1) > 0) {
        switch (c) {
            case 'q': exit(0);
            case '4':
                if (can_move(&current, -1, 0, current.rotation)) current.x--;
                break;
            case '6':
                if (can_move(&current, 1, 0, current.rotation)) current.x++;
                break;
            case '2':
                if (can_move(&current, 0, 1, current.rotation)) current.y++;
                break;
            case '8': {
                int new_rot = (current.rotation + 1) % shapes[current.shape].rotation_count;
                if (can_move(&current, 0, 0, new_rot)) current.rotation = new_rot;
                break;
            }
        }
    }
}

int main() {
    srand(time(0));
    enable_raw_mode();
    init_shapes();

    spawn_piece();

    printf("\033[2J"); 

    print_title();
    sleep(2); 
    
    while (1) {
        if (kbhit()) handle_input();

        static int frame = 0;
        if (frame++ % 5 == 0) { // Adjust speed here
            if (can_move(&current, 0, 1, current.rotation)) {
                current.y++;
            } else {
                place_piece_in_grid(&current);
                clear_lines();
                spawn_piece();
            }
            draw();
        }

        usleep(50000); // 50ms per tick
    }

    return 0;
}
