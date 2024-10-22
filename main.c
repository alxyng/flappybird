#include <SDL2/SDL.h>
#include <time.h>

const int   window_width_initial = 1170 * 0.5;
const int   window_height_initial = 2532 * 0.5;
const float player_width = 75.0f;
const float player_height = 75.0f;
const float player_y_initial = 100.0f;
const float gravity = 2000.0f;
const float jump_velocity_x = -1000.0f;
const float pipe_velocity_x = -400.0f;
const float pipe_width = 100.0f;
const float pipe_gap = 400.0f;
const float pipe_spacing = 500.0f;
const float pipe_gap_padding_top = 100.0f;
const float pipe_gap_padding_bottom = 100.0f;

int window_width = 0;
int window_height = 0;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
int running;
int game_over;

float player_velocity_y = 0.0f;
float player_y = 100.0f;

typedef struct pipe {
    float x;
    float gap_y;
} pipe;

const int max_pipes = 10;
pipe pipes[max_pipes];
int pipes_len = 0;

void reset() {
    game_over = 0;
    player_y = player_y_initial;
    pipes_len = 0;
}

void action() {
    if (!game_over) {
        player_velocity_y = jump_velocity_x;
    } else {
        reset();
    }
}

void processEvents() {
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
                        action();
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                action();
                break;
            // case SDL_WINDOWEVENT:
            //     SDL_GetRendererOutputSize(renderer, &window_width, &window_height);
            //     break;
            default:
                break;
        }
    }
}

int get_gap_y() {
    int min_x = pipe_gap_padding_top;
    int max_x = window_height - pipe_gap - pipe_gap_padding_bottom;
    int range = max_x - min_x;

    return (rand() % range) + min_x;
}

void get_pipe_top_rect(int i, SDL_FRect *rect) {
    rect->x = pipes[i].x;
    rect->y = 0;
    rect->w = pipe_width;
    rect->h = pipes[i].gap_y;
}

void get_pipe_bottom_rect(int i, SDL_FRect *rect) {
    rect->x = pipes[i].x;
    rect->y = pipes[i].gap_y + pipe_gap;
    rect->w = pipe_width;
    rect->h = window_height - pipes[i].gap_y + pipe_gap;
}

void get_player_rect(SDL_FRect *rect) {
    rect->x = 100;
    rect->y = player_y;
    rect->w = 75;
    rect->h = 75; 
}

void get_top_rect(SDL_FRect *rect) {
    rect->x = 0;
    rect->y = -100;
    rect->w = window_width;
    rect->h = 100;
}

void get_bottom_rect(SDL_FRect *rect) {
    rect->x = 0;
    rect->y = window_height;
    rect->w = window_width;
    rect->h = 100;
}

void update(float dt) {
    if (game_over) {
        return;
    }

    // If there are no pipes, create a pipe
    if (pipes_len == 0) {
        pipes[0].x = window_width;
        pipes[0].gap_y = get_gap_y();
        pipes_len++;
    }

    // If the last pipe is futher away than pipe_spacing, create a new one
    if (pipes_len < max_pipes && pipes[pipes_len - 1].x < window_width - pipe_spacing) {
        pipes[pipes_len].x = window_width;
        pipes[pipes_len].gap_y = get_gap_y();
        pipes_len++; 
    }

    // Remove pipes than the player has passed
    while (pipes[0].x + pipe_width < 0) {
        // Move pipes down in the array
        for (int i = 0; i < pipes_len - 1; i++) {
            pipes[i] = pipes[i + 1];
        }
        pipes_len--;
    }

    for (int i = 0; i < pipes_len; i++) {
        pipes[i].x += pipe_velocity_x * dt;
    }

    player_velocity_y += gravity * dt;
    player_y += player_velocity_y * dt;

    // collision detenction

    SDL_FRect a;
    SDL_FRect b;

    get_player_rect(&a);

    get_top_rect(&b);
    if (SDL_HasIntersectionF(&a, &b)) {
        game_over = 1;
    }

    get_bottom_rect(&b);
    if (SDL_HasIntersectionF(&a, &b)) {
        game_over = 1;
    }

    for (int i = 0; i < pipes_len; i++) {
        get_pipe_top_rect(i, &b);
        if (SDL_HasIntersectionF(&a, &b)) {
            game_over = 1;
        }

        get_pipe_bottom_rect(i, &b);
        if (SDL_HasIntersectionF(&a, &b)) {
            game_over = 1;
        }
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_FRect rect;

    for (int i = 0; i < pipes_len; i++) {
        get_pipe_top_rect(i, &rect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRectF(renderer, &rect);

        get_pipe_bottom_rect(i, &rect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRectF(renderer, &rect);
    }

    get_player_rect(&rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRectF(renderer, &rect);

    SDL_RenderPresent(renderer);
}

void run() {
    running           = 1;
    uint64_t lastTime = SDL_GetTicks64();
    uint64_t frames   = 0;

    reset();

    while (running) {
        uint64_t startTime = SDL_GetTicks64();
        float dt           = (startTime - lastTime) / 1000.0f;
        lastTime           = startTime;

        processEvents();
        update(dt);
        render();

        frames++;
    }

    printf("ticks = %llu, frames = %llu", SDL_GetTicks64(), frames);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width_initial, window_height_initial, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }

    if (SDL_GetRendererOutputSize(renderer, &window_width, &window_height) != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return 1;
    }

    printf("width = %d, height = %d\n", window_width, window_height);    

    run();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}