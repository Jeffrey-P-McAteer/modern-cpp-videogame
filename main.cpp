
#include <iostream>
#include <random>
#include <cstdlib>

// I hope windows is still POSIX compliant
#include <sys/time.h>

// 3rd-party libs that build.py downloads, compiles, + adds to our include path
#include <SDL.h>
//#include <SDL_image.h>
#include <SDL_timer.h>

const unsigned int WIDTH = 600;
const unsigned int HEIGHT = 400;

// Helper to turn 0-255 numbers into 0-1.0 for GL
#define RGB(R,G,B) { (double) R / 255.0, (double) G / 255.0, (double) B / 255.0}

// http://www.colourlovers.com/pattern/5939008/arcoiris
// Also several from https://lospec.com/palette-list
const double COLORS[][3] = {
  RGB(0x1a, 0x1c, 0x2c),
  RGB(0x5d, 0x27, 0x5d),
  RGB(0xb1, 0x3e, 0x53),
  RGB(0xef, 0x7d, 0x57),
  RGB(0xff, 0xcd, 0x75),
  RGB(0xa7, 0xf0, 0x70),
  RGB(0x38, 0xb7, 0x64),
  RGB(0x25, 0x71, 0x79),
  RGB(0x29, 0x36, 0x6f),
  RGB(0x3b, 0x5d, 0xc9),
  RGB(0x41, 0xa6, 0xf6),
  RGB(0x73, 0xef, 0xf7),
  RGB(0xf4, 0xf4, 0xf4),
  RGB(0x94, 0xb0, 0xc2),
  RGB(0x56, 0x6c, 0x86),
  RGB(0x33, 0x3c, 0x57),
};

#undef RGB

/* Game state globals */
int pos_x = 100;
int pos_y = 100;
bool player_died = false;
double player_color[3] = {0.0, 0.0, 0.0}; // set in main()



void set_user_colors() {
  const int user_colors_len = sizeof COLORS / sizeof COLORS[0];
  std::uniform_int_distribution<int> our_random_dist(0, user_colors_len-1);
  std::default_random_engine engine;
  // We seed the engine with every char in USERNAME, USER, HOST, HOSTNAME
  const char* env_vars[] = {
    "USERNAME", "USER", "HOST", "HOSTNAME"
  };
  int sum = 0;
  for (const char* env_var : env_vars) {
    const char* env_p = std::getenv(env_var);
    if (env_p != nullptr) {
      for (const char* c = env_p; *c != '\0'; c++) {
        sum += *c;
      }
    }
  }
  engine.seed(sum);
  for (int i=0; i<5; i++) {
    sum += our_random_dist(engine);
  }

  // Now we use the next random int as our color index
  int color_i = our_random_dist(engine);
  
  // Finally assign player_color to COLORS
  for (int i=0; i<3; i++) {
    player_color[i] = COLORS[color_i][i];
  }

  std::cout << "Player color = #" << color_i << " " <<
    "(" << player_color[0] << "," << player_color[1] << "," << player_color[2] << ")" <<
    "  sum=" << sum <<
    std::endl;
}

void dieif(bool should_exit, const char* msg) {
  if (should_exit) {
    std::cerr << msg << std::endl;
    std::exit(1);
  }
}

int main(int argc, char** argv) {
  std::cout << "Hello!" << std::endl;

  // Generate player's color using USERNAME env var (windows), falling back to USER (linux/macos)
  set_user_colors();

  dieif(SDL_Init(SDL_INIT_EVERYTHING) != 0, "Could not start SDL!");

  SDL_Window* main_window = SDL_CreateWindow(
    "TODO title here",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
  );
  dieif(main_window == nullptr, "Could not create SDL window!");

  SDL_Renderer* rend = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
  dieif(rend == nullptr, "Could not create SDL rendering engine!");

  bool want_exit = false;

  while (!want_exit) {
    // Handle all events in the buffer (mouse clicks, keypresses)
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          want_exit = true;
          break;

        case SDL_KEYDOWN:
          switch (event.key.keysym.scancode) { 
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
              pos_y -= 10;
              if (pos_y < 0) {
                pos_y = 0;
              }
              break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
              pos_x -= 10;
              if (pos_x < 0) {
                pos_x = 0;
              }
              break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
              pos_y += 10;
              if (pos_y > HEIGHT-10) {
                pos_y = HEIGHT-10;
              }
              break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
              pos_x += 10;
              if (pos_x > WIDTH-10) {
                pos_x = WIDTH-10;
              }
              break;
            case SDL_SCANCODE_Q:
              std::exit(1);
              break;
            default:
              std::cout << "Unhandled SDL_KEYDOWN, key scancode = " << event.key.keysym.scancode << std::endl;
              break;
          }
          break;

        default:
          std::cout << "Unhandled SDL_Event: " << event.type << std::endl;
          break;
      }
    }

    // Render the window
    {
      SDL_SetRenderDrawColor(rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(rend); 

      SDL_SetRenderDrawColor(rend, (int) (player_color[0] * 255.0), (int) (player_color[1] * 255.0), (int) (player_color[2] * 255.0), SDL_ALPHA_OPAQUE);
      SDL_Rect player_rect;
      player_rect.x = pos_x;
      player_rect.y = pos_y;
      player_rect.w = 10;
      player_rect.h = 10;
      SDL_RenderFillRect(rend, &player_rect);

    }

    // Flip buffers
    SDL_RenderPresent(rend); 

    // Delay 8ms, giving us 120 fps if the event handling + rendering takes 0ms.
    SDL_Delay(1000 / 120);

  }

  return 0;
}

