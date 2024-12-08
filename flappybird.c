#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>

#if defined(__IPHONEOS__)
const Uint32 Window_Flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI;
#else
const Uint32 Window_Flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
#endif

#define COLLISION_DETECTION  1
#define DRAW_PLAYER_COLLIDER 0

// Sprite Mapping

const SDL_Rect Sprite_Background     = { .x = 3,   .y = 0,   .w = 144, .h = 256 };
const SDL_Rect Sprite_Ground         = { .x = 215, .y = 10,  .w = 12,  .h = 56  };
const SDL_Rect Sprite_Logo           = { .x = 152, .y = 200, .w = 89,  .h = 24  };
const SDL_Rect Sprite_Button_Start   = { .x = 212, .y = 230, .w = 40,  .h = 14  };
const SDL_Rect Sprite_Button_OK      = { .x = 212, .y = 154, .w = 40,  .h = 14  };
const SDL_Rect Sprite_Button_Pause   = { .x = 261, .y = 174, .w = 13,  .h = 14  };
const SDL_Rect Sprite_Button_Play    = { .x = 412, .y = 94,  .w = 13,  .h = 14  };
const SDL_Rect Sprite_Get_Ready      = { .x = 254, .y = 71,  .w = 92,  .h = 25  };
const SDL_Rect Sprite_Tap            = { .x = 370, .y = 43,  .w = 57,  .h = 49  };
const SDL_Rect Sprite_Game_Over      = { .x = 152, .y = 173, .w = 96,  .h = 21  };
const SDL_Rect Sprite_Board          = { .x = 260, .y = 195, .w = 113, .h = 57  };
const SDL_Rect Sprite_New            = { .x = 214, .y = 126, .w = 16,  .h = 7   };
const SDL_Rect Sprite_Medal_Bronze   = { .x = 214, .y = 102, .w = 22,  .h = 22  };
const SDL_Rect Sprite_Medal_Silver   = { .x = 214, .y = 78,  .w = 22,  .h = 22  };
const SDL_Rect Sprite_Medal_Gold     = { .x = 384, .y = 154, .w = 22,  .h = 22  };
const SDL_Rect Sprite_Medal_Platinum = { .x = 384, .y = 130, .w = 22,  .h = 22  };
const SDL_Rect Sprite_Pipe           = { .x = 152, .y = 3,   .w = 26,  .h = 147 };
const SDL_Rect Sprite_Pipe_Top       = { .x = 152, .y = 150, .w = 26,  .h = 13  };
const SDL_Rect Sprite_Pipe_Bottom    = { .x = 180, .y = 3,   .w = 26,  .h = 13  };
const SDL_Rect Sprite_Players[3]     = {
    { .x = 381, .y = 187, .w = 17, .h = 12  },
    { .x = 381, .y = 213, .w = 17, .h = 12  },
    { .x = 381, .y = 239, .w = 17, .h = 12  },
};

const SDL_Rect Sprite_Numbers[10] = {
    { .x = 254, .y = 98,  .w = 12, .h = 18 },
    { .x = 238, .y = 80,  .w = 8,  .h = 18 },
    { .x = 325, .y = 148, .w = 12, .h = 18 },
    { .x = 339, .y = 148, .w = 12, .h = 18 },
    { .x = 353, .y = 148, .w = 12, .h = 18 },
    { .x = 367, .y = 148, .w = 12, .h = 18 },
    { .x = 325, .y = 172, .w = 12, .h = 18 },
    { .x = 339, .y = 172, .w = 12, .h = 18 },
    { .x = 353, .y = 172, .w = 12, .h = 18 },
    { .x = 367, .y = 172, .w = 12, .h = 18 },
};

const SDL_Rect Sprite_Numbers_Sm[10] = {
    { .x = 279, .y = 171, .w = 6,  .h = 7 },
    { .x = 282, .y = 180, .w = 3,  .h = 7 },
    { .x = 289, .y = 171, .w = 6, .h = 7 },
    { .x = 289, .y = 180, .w = 6, .h = 7 },
    { .x = 298, .y = 171, .w = 6, .h = 7 },
    { .x = 298, .y = 180, .w = 6, .h = 7 },
    { .x = 306, .y = 171, .w = 6, .h = 7 },
    { .x = 306, .y = 180, .w = 6, .h = 7 },
    { .x = 315, .y = 171, .w = 6, .h = 7 },
    { .x = 315, .y = 180, .w = 6, .h = 7 },
};

const float Sprite_Scale = 7.0f;

#define PLAYER_X (window_width / 5)

const int Window_Width_Initial  = 1170 * 0.5;
const int Window_Height_Initial = 2532 * 0.5;

int window_width_real;
int window_height_real;

float window_width_ratio;
float window_height_ratio;

const float Gravity         = 3800.0f;
const float Jump_Velocity_X = -1100.0f;
const float Pipe_Velocity_X = -400.0f;

const float Pipe_Gap                = 380.0f;
#define     PIPE_SPACING            (Sprite_Pipe.w * Sprite_Scale * 3.0f)

SDL_Window   *window;
SDL_Renderer *renderer;
int           window_width  = 0;
int           window_height = 0;
SDL_Texture  *texture;

Mix_Chunk *sfx_die;
Mix_Chunk *sfx_hit;
Mix_Chunk *sfx_point;
Mix_Chunk *sfx_swooshing;
Mix_Chunk *sfx_wing;

typedef enum game_state_t {
    STATE_MENU      = 0,
    STATE_READY     = 1,
    STATE_PLAY      = 2,
    STATE_GAME_OVER = 3,
} game_state_t;

game_state_t game_state;
void go_to_state(game_state_t state);

void (*process_events)();
void (*update)(float dt);
void (*render)();

int mouse_button_down = 0;
SDL_FPoint mouse_pos;

int menu_button_offset_y      = 0;
int game_over_button_offset_y = 0;

float logo_offset   = 0; // menu
float ground_offset = 0; // menu, state, play

int      running;
int      game_over;
uint64_t ticks;

int score;
int max_score;
int new_max_score;

float player_y;
float player_velocity_y;

const SDL_Rect *player_sprite;
const SDL_Rect *medal_sprite;

typedef struct pipe {
    float x;
    float gap_y;
} pipe;

#define   Max_Pipes 10
pipe      pipes[Max_Pipes];
int       pipes_len = 0;
int       pipe_to_pass = 0;

int pause = 0;

float game_over_flash_alpha;

float clamp(float val, float min, float max) {
    const float t = val < min ? min : val;
    return t > max ? max : t;
}

int get_sprite_width(const SDL_Rect *rect) {
    return rect->w * Sprite_Scale;
}

int get_sprite_height(const SDL_Rect *rect) {
    return rect->h * Sprite_Scale;
}

SDL_Thread   *save_thread;
#define       Save_File_Path_Len 256
char          save_file_path[Save_File_Path_Len];

int load_file() {
    FILE* f = fopen(save_file_path, "r");
    if (!f) {
        if (errno == ENOENT) {
            printf("save file doesn't exist, max_score = 0\n");
            return 0;
        }

        perror("load: failed to open file");
        return 1;
    }

    if (fscanf(f, "%d\n", &max_score) < 0) {
        perror("load: failed to read from file");
        fclose(f);
        return 1;
    }

    printf("save: successfully read from file. max_score = %d\n", max_score);
    fclose(f);

    return 0;
}

#define      Save_File_Err_Len 128
char         save_file_err[Save_File_Err_Len];

typedef enum user_codes_t {
    USER_CODE_SAVE_SUCCESS = 0,
    USER_CODE_SAVE_ERROR   = 1,
} user_codes_t;

int save_file(void *data) {
    SDL_Event event;

    event.type = SDL_USEREVENT;

    FILE* f = fopen(save_file_path, "w+");
    if (!f) {
        event.user.code = USER_CODE_SAVE_ERROR;
        event.user.data1 = (void *)save_file_err;
        snprintf(save_file_err, Save_File_Err_Len, "failed to save: %s", strerror(errno));
        SDL_PushEvent(&event);
        fclose(f);
        return 1;
    }

    if (fprintf(f, "%d\n", max_score) < 0) {
        event.user.code = USER_CODE_SAVE_ERROR;
        event.user.data1 = (void *)save_file_err;
        snprintf(save_file_err, Save_File_Err_Len, "failed to save: %s", strerror(errno));
        SDL_PushEvent(&event);
        fclose(f);
        return 1;
    }

    event.user.code = USER_CODE_SAVE_SUCCESS;
    event.user.data1 = NULL;
    SDL_PushEvent(&event);
    fclose(f);

    return 0;
}

void reset() {
    game_over = 0;
    player_y = ((window_height) - (window_height / 8.0f)) / 2 - get_sprite_height(&Sprite_Players[0]) / 2;
    player_velocity_y = 0.0f;
    pipes_len = 0;
    score = 0;
    pipe_to_pass = 0;
    new_max_score = 0;
}

void jump() {
    Mix_PlayChannel(-1, sfx_wing, 0);
    player_velocity_y = Jump_Velocity_X;
}

int get_gap_y() {
    int min_y = ((window_height) - (window_height / 8.0f)) / 2 - Pipe_Gap / 2 - 400;
    int max_y = ((window_height) - (window_height / 8.0f)) / 2 - Pipe_Gap / 2 + 400;
    int range = max_y - min_y;
    return (rand() % range) + min_y;
}

void get_rect_background(SDL_FRect *rect) {
    rect->x = 0;
    rect->y = 0;
    rect->w = window_width;
    rect->h = window_height;
}

void get_rect_menu_logo(SDL_FRect *rect) {
    rect->x = (window_width / 2) - get_sprite_width(&Sprite_Logo) / 2 - get_sprite_width(player_sprite) / 2 - player_sprite->w;
    rect->y = 300 + logo_offset;
    rect->w = get_sprite_width(&Sprite_Logo);
    rect->h = get_sprite_height(&Sprite_Logo);
}

void get_rect_menu_player(SDL_FRect *rect) {
    rect->x = (window_width / 2) + get_sprite_width(&Sprite_Logo) / 2  - get_sprite_width(player_sprite) / 2 + player_sprite->w;
    rect->y = 300 + logo_offset + get_sprite_height(player_sprite) / 3;
    rect->w = get_sprite_width(player_sprite);
    rect->h = get_sprite_height(player_sprite);
}

void get_rect_menu_button(SDL_FRect *rect, int offset) {
    rect->x = (window_width / 2) - get_sprite_width(&Sprite_Button_Start) / 2;
    rect->y = (window_height / 4.0f) * 3.0f + offset;
    rect->w = get_sprite_width(&Sprite_Button_Start);
    rect->h = get_sprite_height(&Sprite_Button_Start);
}

void get_rect_ready_get_ready(SDL_FRect *rect) {
    rect->x = (window_width / 2) - get_sprite_width(&Sprite_Get_Ready) / 2;
    rect->y = (window_height / 4.0f);
    rect->w = get_sprite_width(&Sprite_Get_Ready);
    rect->h = get_sprite_height(&Sprite_Get_Ready);
}

void get_rect_ready_tap(SDL_FRect *rect) {
    rect->x = (window_width / 2) - get_sprite_width(&Sprite_Tap) / 2;
    rect->y = (window_height / 2) - get_sprite_height(&Sprite_Tap) / 2;
    rect->w = get_sprite_width(&Sprite_Tap);
    rect->h = get_sprite_height(&Sprite_Tap);
}

void get_rect_play_pause(SDL_FRect *rect) {
    rect->x = (window_width / 10);
    rect->y = (window_width / 10);
    rect->w = get_sprite_width(&Sprite_Button_Pause);
    rect->h = get_sprite_height(&Sprite_Button_Pause);
}

void get_rect_game_over(SDL_FRect *rect) {
    rect->x = (window_width / 2) - get_sprite_width(&Sprite_Game_Over) / 2;
    rect->y = (window_height / 4.0f);
    rect->w = get_sprite_width(&Sprite_Game_Over);
    rect->h = get_sprite_height(&Sprite_Game_Over);
}

void get_rect_game_over_board(SDL_FRect *rect) {
    rect->x = (window_width / 2) - get_sprite_width(&Sprite_Board) / 2;
    rect->y = (window_height / 2) - get_sprite_height(&Sprite_Board) / 2;
    rect->w = get_sprite_width(&Sprite_Board);
    rect->h = get_sprite_height(&Sprite_Board);
}

void get_rect_game_over_button(SDL_FRect *rect, int offset) {
    rect->x = (window_width / 2) - get_sprite_width(&Sprite_Button_OK) / 2;
    rect->y = (window_height / 4.0f) * 3.0f + offset;
    rect->w = get_sprite_width(&Sprite_Button_OK);
    rect->h = get_sprite_height(&Sprite_Button_OK);
}

void get_rect_player(SDL_FRect *rect) {
    rect->x = PLAYER_X;
    rect->y = player_y;
    rect->w = get_sprite_width(player_sprite);
    rect->h = get_sprite_height(player_sprite);
}

void get_rect_player_collider(SDL_FRect *rect) {
    rect->x = PLAYER_X + 1 * Sprite_Scale;
    rect->y = player_y + 1 * Sprite_Scale;
    rect->w = get_sprite_width(player_sprite) - 2 * Sprite_Scale;
    rect->h = get_sprite_height(player_sprite) - 2 * Sprite_Scale;
}

void get_pipe_top_rect(int i, SDL_FRect *rect) {
    rect->x = pipes[i].x;
    rect->y = -1000;
    rect->w = get_sprite_width(&Sprite_Pipe);
    rect->h = pipes[i].gap_y + 1000;
}

void get_pipe_top_end_rect(int i, SDL_FRect *rect) {
    rect->x = pipes[i].x;
    rect->y = pipes[i].gap_y - (Sprite_Pipe_Top.h * Sprite_Scale);
    rect->w = get_sprite_width(&Sprite_Pipe);
    rect->h = (Sprite_Pipe_Top.h * Sprite_Scale);
}

void get_pipe_bottom_rect(int i, SDL_FRect *rect) {
    rect->x = pipes[i].x;
    rect->y = pipes[i].gap_y + Pipe_Gap;
    rect->w = get_sprite_width(&Sprite_Pipe);
    rect->h = window_height - (window_height / 8.0f) - (pipes[i].gap_y + Pipe_Gap);
}

void get_pipe_bottom_end_rect(int i, SDL_FRect *rect) {
    rect->x = pipes[i].x;
    rect->y = pipes[i].gap_y + Pipe_Gap;
    rect->w = get_sprite_width(&Sprite_Pipe);
    rect->h = (Sprite_Pipe_Top.h * Sprite_Scale);
}

void get_top_rect(SDL_FRect *rect) {
    rect->x = 0;
    rect->y = -100;
    rect->w = window_width;
    rect->h = 100;
}

void get_bottom_rect(SDL_FRect *rect) {
    rect->x = 0;
    rect->y = (window_height / 8.0f) * 7;
    rect->w = window_width;
    rect->h = (window_height / 8.0f) * 1;
}

void get_ground_segment_rect(float x, SDL_FRect *rect) {
    rect->x = x;
    rect->y = (window_height / 8.0f) * 7;
    rect->w = get_sprite_width(&Sprite_Ground);
    rect->h = (window_height / 8.0f) * 1;
}

void draw_ground(SDL_FRect *rect) {
    for (int i = 0; ; i++) {
        int ground_x = ground_offset + get_sprite_width(&Sprite_Ground) * i;
        if (ground_x > window_width) {
            break;
        }

        get_ground_segment_rect(ground_x, rect);
        SDL_RenderCopyF(renderer, texture, &Sprite_Ground, rect);
    }
}

void draw_score(SDL_FRect *rect) {
    int score_cache = score;
    int digit;
    int digit_width;
    int digit_height;
    int diff = 0;

    while (1) {
        digit = score_cache % 10;
        digit_width = get_sprite_width(&Sprite_Numbers[digit]);
        digit_height = get_sprite_height(&Sprite_Numbers[digit]);

        diff += digit_width + 6;

        score_cache /= 10;
        if (score_cache == 0) {
            break;
        }
    }

    score_cache = score;

    int placement_x = window_width / 2 + diff / 2;

    while (1) {
        digit = score_cache % 10;
        digit_width = get_sprite_width(&Sprite_Numbers[digit]);
        digit_height = get_sprite_height(&Sprite_Numbers[digit]);

        rect->x = placement_x - digit_width;
        rect->y = 200;
        rect->w = digit_width;
        rect->h = digit_height;
        SDL_RenderCopyF(renderer, texture, &Sprite_Numbers[digit], rect);

        score_cache /= 10;
        if (score_cache == 0) {
            break;
        }

        placement_x = placement_x - digit_width - 6;
    }
}

void draw_score_small(SDL_FRect *rect) {
    SDL_FRect board_rect;

    get_rect_game_over_board(&board_rect);

    int score_cache = score;
    int digit;
    int digit_width;
    int digit_height;

    int placement_x = board_rect.x + board_rect.w - 76;

    while (1) {
        digit = score_cache % 10;
        digit_width = get_sprite_width(&Sprite_Numbers_Sm[digit]);
        digit_height = get_sprite_height(&Sprite_Numbers_Sm[digit]);

        rect->x = placement_x - digit_width;
        rect->y = board_rect.y + 125;
        rect->w = digit_width;
        rect->h = digit_height;
        SDL_RenderCopyF(renderer, texture, &Sprite_Numbers_Sm[digit], rect);

        score_cache /= 10;
        if (score_cache == 0) {
            break;
        }

        placement_x = placement_x - digit_width - 6;
    }
}

void draw_max_score_small(SDL_FRect *rect) {
    SDL_FRect board_rect;
    get_rect_game_over_board(&board_rect);

    int score_cache = max_score;
    int digit;
    int digit_width;
    int digit_height;

    int placement_x = board_rect.x + board_rect.w - 76;

    while (1) {
        digit = score_cache % 10;
        digit_width = get_sprite_width(&Sprite_Numbers_Sm[digit]);
        digit_height = get_sprite_height(&Sprite_Numbers_Sm[digit]);

        rect->x = placement_x - digit_width;
        rect->y = board_rect.y + 270;
        rect->w = digit_width;
        rect->h = digit_height;
        SDL_RenderCopyF(renderer, texture, &Sprite_Numbers_Sm[digit], rect);

        placement_x = placement_x - digit_width - 6;

        score_cache /= 10;
        if (score_cache == 0) {
            break;
        }
    }

    if (new_max_score) {
        rect->x = board_rect.x + 460;
        rect->y = board_rect.y + 204;
        rect->w = get_sprite_width(&Sprite_New);
        rect->h = get_sprite_height(&Sprite_New);
        SDL_RenderCopyF(renderer, texture, &Sprite_New, rect);
    }
}

void draw_medal(SDL_FRect *rect) {
    if (medal_sprite == NULL) {
        return;
    }

    SDL_FRect board_rect;
    get_rect_game_over_board(&board_rect);

    rect->x = board_rect.x + 94;
    rect->y = board_rect.y + 148;
    rect->w = get_sprite_width(medal_sprite);
    rect->h = get_sprite_height(medal_sprite);
    SDL_RenderCopyF(renderer, texture, medal_sprite, rect);
}

void get_mouse_state() {
    int x, y;
    mouse_button_down = SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK;
    mouse_pos.x = x * window_width_ratio;
    mouse_pos.y = y * window_height_ratio;
}

int mouse_is_in_rect(const SDL_FRect *rect) {
    get_mouse_state();
    return SDL_PointInFRect(&mouse_pos, rect);
}

void process_events_menu() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                if (event.key.repeat != 0) {
                    break;
                }

                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = 0;
                        break;
                    case SDLK_SPACE:
                        go_to_state(STATE_READY);
                        reset();
                    default:
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                get_mouse_state();
                break;
            case SDL_MOUSEBUTTONUP: {
                SDL_FRect rect;
                get_rect_menu_button(&rect, 0);
                if (mouse_is_in_rect(&rect)) {
                    go_to_state(STATE_READY);
                }
                break;
            }
            case SDL_WINDOWEVENT:
                SDL_GetRendererOutputSize(renderer, &window_width, &window_height);
                break;
            default:
                break;
        }
    }
}

void process_events_ready() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                if (event.key.repeat != 0) {
                    break;
                }

                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = 0;
                        break;
                    case SDLK_SPACE:
                        go_to_state(STATE_PLAY);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                get_mouse_state();
                break;
            case SDL_MOUSEBUTTONDOWN:
                go_to_state(STATE_PLAY);
                break;
            case SDL_WINDOWEVENT:
                SDL_GetRendererOutputSize(renderer, &window_width, &window_height);
                break;
            default:
                break;
        }
    }
}

void process_events_play() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                if (event.key.repeat != 0) {
                    break;
                }

                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = 0;
                        break;
                    case SDLK_SPACE:
                        jump();
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                get_mouse_state();
                break;
            case SDL_MOUSEBUTTONDOWN: {
                SDL_FRect rect;
                get_rect_play_pause(&rect);
                if (mouse_is_in_rect(&rect)) {
                    pause = !pause;
                    break;
                }

                if (!pause) {
                    jump();
                }

                break;
            }
            case SDL_WINDOWEVENT:
                SDL_GetRendererOutputSize(renderer, &window_width, &window_height);
                break;
            default:
                break;
        }
    }
}

void process_events_game_over() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                if (event.key.repeat != 0) {
                    break;
                }

                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = 0;
                        break;
                    case SDLK_SPACE:
                        go_to_state(STATE_MENU);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                get_mouse_state();
                break;
            case SDL_MOUSEBUTTONUP: {
                SDL_FRect rect;
                get_rect_game_over_button(&rect, 0);
                if (mouse_is_in_rect(&rect)) {
                    go_to_state(STATE_MENU);
                }
                break;
            }
            case SDL_WINDOWEVENT:
                SDL_GetRendererOutputSize(renderer, &window_width, &window_height);
                break;
            case SDL_USEREVENT:
                switch (event.user.code) {
                    case USER_CODE_SAVE_SUCCESS:
                        printf("successful file save, max_score = %d\n", max_score);
                        break;
                    case USER_CODE_SAVE_ERROR:
                        printf("error saving file: %s\n", (const char *)event.user.data1);
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}

void update_menu(float dt) {
    logo_offset = sin(ticks / 200.0) * 10;

    player_sprite = &Sprite_Players[ticks % 300 / 100];

    ground_offset += Pipe_Velocity_X * dt;
    ground_offset = fmodf(ground_offset, Sprite_Ground.w * Sprite_Scale);

    SDL_FRect rect;
    get_rect_menu_button(&rect, 0);
    menu_button_offset_y = mouse_is_in_rect(&rect) * mouse_button_down * 1 * Sprite_Scale;
}

void update_ready(float dt) {
    logo_offset = sin(ticks / 200.0) * 10;

    player_sprite = &Sprite_Players[ticks % 300 / 100];

    ground_offset += Pipe_Velocity_X * dt;
    ground_offset = fmodf(ground_offset, Sprite_Ground.w * Sprite_Scale);
}

void update_play(float dt) {
    if (pause) {
        return;
    }

    ground_offset += Pipe_Velocity_X * dt;
    ground_offset = fmodf(ground_offset, Sprite_Ground.w * Sprite_Scale);

    // If there are no pipes, create a pipe
    if (pipes_len == 0) {
        pipes[0].x = window_width;
        pipes[0].gap_y = get_gap_y();
        pipes_len++;
    }

    // If the last pipe is futher away than PIPE_SPACING, create a new one
    if (pipes_len < Max_Pipes && pipes[pipes_len - 1].x < window_width - PIPE_SPACING) {
        pipes[pipes_len].x = window_width;
        pipes[pipes_len].gap_y = get_gap_y();
        pipes_len++; 
    }

    // Remove pipes than the player has passed
    while (pipes[0].x + get_sprite_width(&Sprite_Pipe) < 0) {
        // Move pipes down in the array
        for (int i = 0; i < pipes_len - 1; i++) {
            pipes[i] = pipes[i + 1];
        }
        pipes_len--;
        pipe_to_pass--;
    }

    for (int i = 0; i < pipes_len; i++) {
        pipes[i].x += Pipe_Velocity_X * dt;
    }

    player_velocity_y += Gravity * dt;
    player_y += player_velocity_y * dt;
    player_sprite = &Sprite_Players[ticks % 300 / 100];

#if COLLISION_DETECTION
    // Collision detection
    SDL_FRect a;
    SDL_FRect b;

    get_rect_player_collider(&a);

    get_bottom_rect(&b);
    if (SDL_HasIntersectionF(&a, &b)) {
        go_to_state(STATE_GAME_OVER);
        return;
    }

    for (int i = 0; i < pipes_len; i++) {
        get_pipe_top_rect(i, &b);
        if (SDL_HasIntersectionF(&a, &b)) {
            go_to_state(STATE_GAME_OVER);
            return;
        }

        get_pipe_bottom_rect(i, &b);
        if (SDL_HasIntersectionF(&a, &b)) {
            go_to_state(STATE_GAME_OVER);
            return;
        }
    }
#endif // COLLISION_DETECTION

    if (PLAYER_X > pipes[pipe_to_pass].x) {
        Mix_PlayChannel(-1, sfx_point, 0);
        score++;
        pipe_to_pass++;
    }
}

void update_game_over(float dt) {
    SDL_FRect rect;
    get_rect_game_over_button(&rect, 0);
    game_over_button_offset_y = mouse_is_in_rect(&rect) * mouse_button_down * 1 * Sprite_Scale;
    game_over_flash_alpha = clamp(game_over_flash_alpha -= dt * 500, 0, 255);
}

void render_menu() {
    SDL_FRect rect;

    get_rect_background(&rect);
    SDL_RenderCopyF(renderer, texture, &Sprite_Background, &rect);

    get_rect_menu_logo(&rect);
    SDL_RenderCopyF(renderer, texture, &Sprite_Logo, &rect);

    get_rect_menu_player(&rect);
    SDL_RenderCopyF(renderer, texture, player_sprite, &rect);

    draw_ground(&rect);

    get_rect_menu_button(&rect, menu_button_offset_y);
    SDL_RenderCopyF(renderer, texture, &Sprite_Button_Start, &rect);

    SDL_RenderPresent(renderer);
}

void render_ready() {
    SDL_FRect rect;

    get_rect_background(&rect);
    SDL_RenderCopyF(renderer, texture, &Sprite_Background, &rect);

    get_rect_ready_get_ready(&rect);
    SDL_RenderCopyF(renderer, texture, &Sprite_Get_Ready, &rect);

    get_rect_player(&rect);
    SDL_RenderCopyF(renderer, texture, player_sprite, &rect);

    draw_ground(&rect);

    get_rect_ready_tap(&rect);
    SDL_RenderCopyF(renderer, texture, &Sprite_Tap, &rect);

    draw_score(&rect);

    SDL_RenderPresent(renderer);
}

void render_play() {
    SDL_FRect rect;

    get_rect_background(&rect);
    SDL_RenderCopyF(renderer, texture, &Sprite_Background, &rect);

    draw_ground(&rect);

    for (int i = 0; i < pipes_len; i++) {
        get_pipe_top_rect(i, &rect);
        SDL_RenderCopyF(renderer, texture, &Sprite_Pipe, &rect);

        get_pipe_top_end_rect(i, &rect);
        SDL_RenderCopyF(renderer, texture, &Sprite_Pipe_Top, &rect);

        get_pipe_bottom_rect(i, &rect);
        SDL_RenderCopyF(renderer, texture, &Sprite_Pipe, &rect);

        get_pipe_bottom_end_rect(i, &rect);
        SDL_RenderCopyF(renderer, texture, &Sprite_Pipe_Bottom, &rect);
    }

#if DRAW_PLAYER_COLLIDER
    get_rect_player_collider(&rect);
    SDL_SetRenderDrawColor(renderer, 200, 100, 100, 100);
    SDL_RenderFillRectF(renderer, &rect);
#endif // DRAW_PLAYER_COLLIDER

    get_rect_player(&rect);
    SDL_RenderCopyF(renderer, texture, player_sprite, &rect);

    get_rect_play_pause(&rect);
    SDL_RenderCopyF(renderer, texture, pause ? &Sprite_Button_Play : &Sprite_Button_Pause, &rect);

    draw_score(&rect);

    SDL_RenderPresent(renderer);
}

void render_game_over() {
    SDL_FRect rect;

    get_rect_background(&rect);
    SDL_RenderCopyF(renderer, texture, &Sprite_Background, &rect);

    draw_ground(&rect);

    for (int i = 0; i < pipes_len; i++) {
        get_pipe_top_rect(i, &rect);
        SDL_RenderCopyF(renderer, texture, &Sprite_Pipe, &rect);

        get_pipe_top_end_rect(i, &rect);
        SDL_RenderCopyF(renderer, texture, &Sprite_Pipe_Top, &rect);

        get_pipe_bottom_rect(i, &rect);
        SDL_RenderCopyF(renderer, texture, &Sprite_Pipe, &rect);

        get_pipe_bottom_end_rect(i, &rect);
        SDL_RenderCopyF(renderer, texture, &Sprite_Pipe_Bottom, &rect);
    }

#if DRAW_PLAYER_COLLIDER
    get_rect_player_collider(&rect);
    SDL_SetRenderDrawColor(renderer, 200, 100, 100, 100);
    SDL_RenderFillRectF(renderer, &rect);
#endif // DRAW_PLAYER_COLLIDER

    get_rect_player(&rect);
    SDL_RenderCopyF(renderer, texture, player_sprite, &rect);

    draw_score(&rect);

    get_rect_game_over(&rect);
    SDL_RenderCopyF(renderer, texture, &Sprite_Game_Over, &rect);

    get_rect_game_over_board(&rect);
    SDL_RenderCopyF(renderer, texture, &Sprite_Board, &rect);

    draw_score_small(&rect);
    draw_max_score_small(&rect);
    draw_medal(&rect);

    get_rect_game_over_button(&rect, game_over_button_offset_y);
    SDL_RenderCopyF(renderer, texture, &Sprite_Button_OK, &rect);

    rect.x = 0;
    rect.y = 0;
    rect.w = window_width;
    rect.h = window_height;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, game_over_flash_alpha);
    SDL_RenderFillRectF(renderer, &rect);

    SDL_RenderPresent(renderer);
}

void go_to_state(game_state_t state) {
    game_state = state;

    switch (state) {
        case STATE_MENU:
            process_events = process_events_menu;
            update = update_menu;
            render = render_menu;
            reset();
            Mix_PlayChannel(-1, sfx_swooshing, 0);
            break;
        case STATE_READY:
            process_events = process_events_ready;
            update = update_ready;
            render = render_ready;
            Mix_PlayChannel(-1, sfx_swooshing, 0);
            break;
        case STATE_PLAY:
            process_events = process_events_play;
            update = update_play;
            render = render_play;
            reset();
            jump();
            break;
        case STATE_GAME_OVER: {
            process_events = process_events_game_over;
            update = update_game_over;
            render = render_game_over;

            Mix_PlayChannel(-1, sfx_hit, 0);

            if (score >= 40) {
                medal_sprite = &Sprite_Medal_Platinum;
            } else if (score >= 30) {
                medal_sprite = &Sprite_Medal_Gold;
            } else if (score >= 20) {
                medal_sprite = &Sprite_Medal_Silver;
            } else if (score >= 10) {
                medal_sprite = &Sprite_Medal_Bronze;
            } else {
                medal_sprite = NULL;
            }

            game_over_flash_alpha = 255;

            if (score <= max_score) {
                break;
            }

            new_max_score = 1;
            max_score = score;

            save_thread = SDL_CreateThread(save_file, "save_file", NULL);
            if (!save_thread) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), window);
            }
            SDL_DetachThread(save_thread);

            break;
        }
        default:
            break;
    }
}

void run() {
    running            = 1;
    uint64_t lastTicks = SDL_GetTicks64();

    go_to_state(STATE_MENU);

    while (running) {
        ticks     = SDL_GetTicks64();
        float dt  = (ticks - lastTicks) / 1000.0f;
        lastTicks = ticks;

        process_events();
        update(dt);
        render();
    }
}

void show_error(const char* fmt, ...) {
    const size_t errlen = 128;
    char         errstr[errlen];

    va_list args;
    va_start(args, fmt);

    vsnprintf(errstr, errlen, fmt, args);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", errstr, window);

    va_end(args);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    const size_t errlen = 128;
    char         errstr[errlen];

#if defined(__IPHONEOS__)
    snprintf(save_file_path, Save_File_Path_Len, "%s/Documents/data.txt", getenv("HOME"));
#else
    snprintf(save_file_path, Save_File_Path_Len, "data.txt");
#endif

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        show_error("Error initialising SDL: %s", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        show_error("Error initialising SDL_image: %s", IMG_GetError());
        return 1;
    }

    if (Mix_Init(0) != 0) {
        show_error("Error initialising SDL_mixer: %s", Mix_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Window_Width_Initial, Window_Height_Initial, Window_Flags);
    if (!window) {
        show_error("Error creating window: %s", SDL_GetError());
        return 1;
    }

    SDL_GetWindowSize(window, &window_width_real, &window_height_real);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        show_error("Error creating renderer: %s", SDL_GetError());
        return 1;
    }

    if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) < 0) {
        show_error("Error setting renderer blend mode: %s", SDL_GetError());
        return 1;
    };

    if (SDL_GetRendererOutputSize(renderer, &window_width, &window_height) != 0) {
        show_error("Error getting renderer output size: %s", SDL_GetError());
        return 1;
    }

    window_width_ratio = (float)window_width / (float)window_width_real;
    window_height_ratio = (float)window_height / (float)window_height_real;

    texture = IMG_LoadTexture(renderer, "spritesheet.png");
    if (!texture) {
        show_error("Error loading sprite sheet: %s", IMG_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
        show_error("Error opening audio device: %s", Mix_GetError());
        return 1;
    }

    sfx_die = Mix_LoadWAV("sfx_die.wav");
    if (!(sfx_die)) {
        show_error(Mix_GetError());
        return 1;
    }

    sfx_hit = Mix_LoadWAV("sfx_hit.wav");
    if (!sfx_hit) {
        show_error(Mix_GetError());
        return 1;
    }

    sfx_point = Mix_LoadWAV("sfx_point.wav");
    if (!sfx_point) {
        show_error(Mix_GetError());
        return 1;
    }

    sfx_swooshing = Mix_LoadWAV("sfx_swooshing.wav");
    if (!sfx_swooshing) {
        show_error(Mix_GetError());
        return 1;
    }

    sfx_wing = Mix_LoadWAV("sfx_wing.wav");
    if (!sfx_wing) {
        show_error(Mix_GetError());
        return 1;
    }

    if (load_file() != 0) {
        show_error(strerror(errno));
        return 1;
    }

    run();

    Mix_FreeChunk(sfx_wing);
    Mix_FreeChunk(sfx_swooshing);
    Mix_FreeChunk(sfx_point);
    Mix_FreeChunk(sfx_hit);
    Mix_FreeChunk(sfx_die);
    Mix_CloseAudio();
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
