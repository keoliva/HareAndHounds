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
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "include/draw.h"

using namespace std;

HHGame *game;

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
    glClearStencil(0); // this is the default value
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // reset transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (selection_index) { // something was just clicked
        loc coord = getBoardCoordOfSelection(selection_index);
        cout << "selected: " << coord.print() << endl;
        cout << "current_player: " << ((game->get_player() == HOUNDS)?"HOUNDS":"HARE") << endl;
        if (game->get_player() == HOUNDS) {
            if (selected_coord.x != -1) { // have clicked a player piece before
                cout << "you selected the hound at: " << selected_coord.print() << endl;
                // switch selected coord with this one
                if (game->get_board().get(coord) == HOUNDS) {
                    selected_coord = coord;
                    drawGame(&selected_coord);
                } else {
                     MovePlayer m = MovePlayer(selected_coord, coord);
                    cout << "process of deciding if your move was legal" << endl;
                    if (game->is_legal_move(&m)) {
                        cout << "move IS LEGAL" <<endl;
                        game->make_move(&m);
                        selection_index = 0;
                        selected_coord = loc(-1, -1);
                        drawGame();
                    } else {
                        cout << "move IS NOT LEGAL" << endl;
                        if (!selected_coord.equals(coord))
                            // keep drawing the player piece highlighted
                            drawGame(&selected_coord, "HOUNDS can only move to an adjacent spot from left to right or up and down");
                        else drawGame(&selected_coord);
                    }
                }
            } else { // have not clicked a player piece
                cout << "process of selecting a hound:" << endl;
                if (game->get_board().get(coord) == HOUNDS) {
                    cout<<"place I chose DID have a hound"<<endl;
                    selected_coord = coord;
                    drawGame(&selected_coord);
                } else {
                    cout<<"place I chose DID NOT have a hound"<<endl;
                    selection_index = 0;
                    drawGame(nullptr, "SELECT a hound to move");
                }
            }
        } else { // hare piece doesn't have to be chosen since there's only once, a legal move should've been chosen
            cout << "hiyah i'm a hare"<<endl;
            MovePlayer m = MovePlayer(game->get_hare(), coord);
            if (game->is_legal_move(&m)) {
                game->make_move(&m);
                selection_index = 0;
                // selected_coord is only set if the current player is hounds
                drawGame();
            } else {
                // keep drawing the player piece highlighted
                selection_index = 0;
                drawGame(nullptr, "HARE can move in any direction, as long as it's adjacent to it");
            }
        }
    } else {
        drawGame(&selected_coord);
    }
    glutSwapBuffers();
}

static void mouseButton(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
    cout << "mouse at (" << x << ", " << y << ")" << endl;
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    GLuint index;
    glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

    selection_index = index;
    printf("Clicked on pixel %d, %d, stencil index %u\n", x, y, index);

    glutPostRedisplay();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'q':
        delete game;
        exit(0);
        break;

    case 'r':
        break;
    }
}

static void idle(void)
{
    glutPostRedisplay();
}

void init(void)
{
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    game = new HHGame();
}

/* Program entry point */

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
    glutIdleFunc(idle);

    init();

    glutMainLoop();

    return EXIT_SUCCESS;
}
