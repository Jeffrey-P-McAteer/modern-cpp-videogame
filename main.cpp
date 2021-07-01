
#include <iostream>
#include <random>

// I hope windows is still POSIX compliant
#include <sys/time.h>

#include <GL/glut.h>

const unsigned int WIDTH = 600;
const unsigned int HEIGHT = 400;

/* Game state globals */
int pos_x = 100;
int pos_y = 100;
bool player_died = false;

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
      glColor3f(1.0f, 0.0f, 0.0f); // Red
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
      glColor3f(1.0f, 0.0f, 0.0f); // Red
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
    std::uniform_real_distribution<double> our_random_dist(-0.5, 0.5);
    std::default_random_engine engine;
    engine.seed( time(nullptr) );
    pos_x = our_random_dist(engine);
    pos_y = our_random_dist(engine);

  }

}

void resize(int width, int height) {
  // Ignore new size + request we go back to WIDTHxHEIGHT
  glutReshapeWindow(WIDTH, HEIGHT);
}


int main(int argc, char** argv) {
  std::cout << "Hello!" << std::endl;
  
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
