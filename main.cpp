
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



  return 0;
}

