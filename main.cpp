/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "include/draw.h"
using namespace std;

HHGame *game;
Draw *draw;
float z = -6.0f;
int selection_index = 0;
float _angle = 0.0;
float y_angle = 0.0;
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
    glClearStencil(0); // this is the default value
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // reset transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // add positioned light
    GLfloat ambient[] = { 0.75f,0.75f, 0.75f, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    /**GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    // add directed light
    GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f};
    // coming from direction (-1, 0.5, 0.5)
    GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f}; // last elem represents light is directed, not positioned
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);*/

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
    case 'a':
        z -= 1.0f;
        break;
    case 'b':
        z += 1.0f;
        break;
    case 'd':
        _angle += 1.0;
        break;
    case 'c':
        _angle -= 1.0;
        break;
    case 'e':
        y_angle -= 1.0;
        break;
    case 'f':
        y_angle += 1.0;
        break;
    case 'r':
        if (game->roundOver()) {
            game->restart();
            glutPostRedisplay();
        }
        break;
    }
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

void init(void)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING); // enable lighting
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0); // enable light #0
 // enable light #1
    //glEnable(GL_NORMALIZE); // automatically normalize normals*/
    glEnable(GL_SMOOTH);

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
    //glutIdleFunc(idle);
    init();
    //Obj *hare = new Obj("resources/the_hare.obj", HARE);
    //delete hare;
    glutMainLoop();
    return EXIT_SUCCESS;
}
