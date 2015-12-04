/*
 * Hare and Hounds
 *
 * Written by Kayla Oliva November 2015
 *
 * A human-to-human rendition of the game Hare and Hounds.
 * My prompt is a python homework assignment from my first
 * programming class at CMU.
 * Rules I implemented come from the Wikipedia page on the game:
 * https://en.wikipedia.org/wiki/Hare_games#Hare_and_Hounds
 */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#endif

#include "include/draw.h"

HHGame *game;
Draw *draw;
int selection_index = 0;
loc selected_coord = loc(-1, -1);

/* GLUT callback Handlers */

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
}

static void display(void)
{
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // reset transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (selection_index) { // something was just clicked
        loc coord = draw->getBoardCoordOfSelection(selection_index);
        if (game->get_player() == HOUNDS) {
            if (selected_coord.x != -1) { // have clicked a player piece before
                // switch selected coord with this one
                if (game->get_board().get(coord) == HOUNDS) {
                    selected_coord = coord;
                    draw->drawGame(game, &selected_coord);
                } else {
                    MovePlayer m = MovePlayer(selected_coord, coord);
                    try {
                        game->make_move(&m);
                        selection_index = 0;
                        selected_coord = loc(-1, -1);
                        draw->drawGame(game);
                    } catch(IllegalMoveError &e) {
                        if (!selected_coord.equals(coord))
                            // keep drawing the player piece highlighted
                            draw->drawGame(game, &selected_coord, "HOUNDS can only move to an adjacent spot from left to right or up and down");
                        else draw->drawGame(game, &selected_coord);
                    }
                }
            } else { // have not clicked a player piece
                if (game->get_board().get(coord) == HOUNDS) {
                    selected_coord = coord;
                    draw->drawGame(game, &selected_coord);
                } else {
                    //selection_index = 0;
                    draw->drawGame(game, nullptr, "SELECT a hound to move");
                }
            }
        } else { // hare piece doesn't have to be chosen since there's only once, a legal move should've been chosen
            MovePlayer m = MovePlayer(game->get_hare(), coord);
            try {
                game->make_move(&m);
                selection_index = 0;
                // selected_coord is only set if the current player is hounds
                draw->drawGame(game);
            } catch(IllegalMoveError &e) {
                // keep drawing the player piece highlighted
                // commented out since I want the error message to stay
                //selection_index = 0;
                draw->drawGame(game, nullptr, "HARE can move in any direction, as long as it's adjacent to it");
            }
        }
    } else {
        draw->drawGame(game, &selected_coord);
    }
    glutSwapBuffers();
}

static void mouseButton(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN || game->roundOver()) return;
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    GLuint index;
    glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

    selection_index = index;

    glutPostRedisplay();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'q':
        delete game;
        delete draw;
        exit(0);
        break;
    case 'r':
        if (game->roundOver()) {
            game->restart();
            glutPostRedisplay();
        }
        break;
    }
}

const GLfloat light_ambient[]  = { 0.75f, 0.75f, 0.75f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };
void init(void)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    game = new HHGame();
    draw = new Draw();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

    glutCreateWindow("Hare and Hounds");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutMouseFunc(mouseButton);
    glutKeyboardFunc(key);
    init();

    glutMainLoop();
    return EXIT_SUCCESS;
}
