
#include <iostream>
#include <random>
#include <cstdlib>

// I hope windows is still POSIX compliant
#include <sys/time.h>

#include <GL/glut.h>

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

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
  //std::cout << "Frame!" << std::endl;
  //std::cout << "x=" << pos_x << "    y=" << pos_y << std::endl;

  // Note: GL is a state machine. This means you need to translate
  //       a tree of draw commands into a list of operations, and
  //       to keep yourself sane boundaries like glPushMatrix/glPopMatrix and
  //       glBegin/glEnd are used between individual drawing routines.
  
  // Begin by using projection mode assuming an 800x600 px window,
  // which lets us do everything in eg 50px instead of 0.02 of window width.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
  glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)

  // Draw white text for instructions
  // NB: Text is a bitch, we'll be making abstractions on top of this soon.
  glPushMatrix();
  {
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glRasterPos2f(2, HEIGHT-20);  // Text pos (kinda upper-left)
    const char* message = "Welcome to <TODO game name>";
    for (const char* c = message; *c != '\0'; c++) {
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    glRasterPos2f(2, (HEIGHT-20)-20 );  // Text pos (20 lower than "kinda upper-left")
    const char* message_pt2 = "WASD to move, Q to quit.";
    for (const char* c = message_pt2; *c != '\0'; c++) {
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
  }
  glPopMatrix();


  if (player_died) {
    // Print replay text
    glPushMatrix();
    {
      glColor3f(player_color[0], player_color[0], player_color[0]);
      glRasterPos2f((WIDTH / 2) - 200, (HEIGHT / 2) - 50 );  // Kinda center
      const char* message = "You're DEAD!";
      for (const char* c = message; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
      }
    }
    glPopMatrix();
  }
  else {
    // Draw a Red 1x1 Square
    glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
    {
      glColor3f(player_color[0], player_color[0], player_color[0]);
      glVertex2f(-8 + pos_x, -8 + pos_y);    // x, y
      glVertex2f( 8 + pos_x, -8 + pos_y);
      glVertex2f( 8 + pos_x,  8 + pos_y);
      glVertex2f(-8 + pos_x,  8 + pos_y);
    }
    glEnd();
  }

  // Render now
  glFlush();

  // Restore default rendering mode (not that any other code uses it)
  glMatrixMode(GL_MODELVIEW);
}

/* proxy to satisfy void (*func)(int value) needed by glutTimerFunc */
void display_timer(int unused) {
  // Mark window as needing render (aka display() gets run)
  glutPostRedisplay();
  
  // Recurse in 50ms
  glutTimerFunc(50, display_timer, 0);
}

/* Handler for keypresses */
void keyPressed(unsigned char key, int x, int y) {
  static int STEP_SIZE = 5;

  switch (key) {
    case 'w':
      pos_y += STEP_SIZE;
      break;

    case 'a':
      pos_x -= STEP_SIZE;
      break;

    case 's':
      pos_y -= STEP_SIZE;
      break;

    case 'd':
      pos_x += STEP_SIZE;
      break;

    case 'q':
      std::exit(0);
      break;
    
    default:
      std::cout << "unhandled key " << key << " x=" << x << " y=" << y << std::endl;
      break;
  }

  // Record if we died after moving
  player_died = pos_x < 0 || pos_y < 0 || pos_x > WIDTH || pos_y > HEIGHT;

}

void mouseClicks(int button, int state, int x, int y) {
  std::cout << "unhandled mouse " << button << " state=" << state << " x=" << x << " y=" << y << std::endl;

  if (button == 2) {
    // right-click scrambles pos_x and pos_y
    std::uniform_int_distribution<int> our_random_dist(-50, 50);
    std::default_random_engine engine;
    engine.seed( time(nullptr) );
    pos_x = 100 + our_random_dist(engine);
    pos_y = 100 + our_random_dist(engine);
  }

}

void resize(int width, int height) {
  // Ignore new size + request we go back to WIDTHxHEIGHT
  glutReshapeWindow(WIDTH, HEIGHT);
}

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

int main(int argc, char** argv) {
  std::cout << "Hello!" << std::endl;

  // Generate player's color using USERNAME env var (windows), falling back to USER (linux/macos)
  set_user_colors();
  
  glutInit(&argc, argv);
  
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(
    (glutGet(GLUT_SCREEN_WIDTH)-WIDTH)/2,
    (glutGet(GLUT_SCREEN_HEIGHT)-HEIGHT)/2
  );

  glutCreateWindow("TODO write game title");
  
  // When GL detects a repaint event, display() will be called
  glutDisplayFunc(display);

  // Every 50ms we call glutPostRedisplay to mark the window as needing a render
  // This line only schedules 1, we do the same at the end of display_timer() for recursion
  glutTimerFunc(50, display_timer, 0);

  // Register input handlers
  glutKeyboardFunc(keyPressed);
  glutMouseFunc(mouseClicks);
  glutReshapeFunc(resize);

  // Finally run main graphics loop
  glutMainLoop();

  return 0;
}
