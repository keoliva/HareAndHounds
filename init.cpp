#include "include/init.h"
#include <iostream>
using namespace std;

const double z_coord = -6.0;

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };


void drawSquare(float x, float y, float width)
{
    float len = width/2;
    float bottom_left_coord[2] = { x-len, y-len };
    float top_right_coord[2] = { x+len, y+len };
    // first the outline's drawn, then the gray inside
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int j = 0; j < 2; j++) {
        glRectfv(bottom_left_coord, top_right_coord);

        len *= .85;
        bottom_left_coord[0] = x-len; bottom_left_coord[1] = y-len;
        top_right_coord[0] = x+len; top_right_coord[1] = y+len;
        glColor3f(211/255.0, 211/255.0, 211/255.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glColor3d(1, 1, 1);
}

void drawCircle(float cx, float cy, float radius)
{
    int num_of_seg = 6;
    float x, y;
    x = radius + 0.06;
    y = 0;
    float tangent_vector_x, tangent_vector_y;
    float theta = 2 * 3.14159265359 / num_of_seg;
    float tan = tanf(theta);
    float cos = cosf(theta);
    glFrontFace(GL_CW);
    // first the outline's drawn, then the gray inside
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int j = 0; j < 2; j++) {
        glBegin(GL_POLYGON);
            for (int i = 0; i < num_of_seg; i++) {
                glVertex2f(cx + x, cy + y);

                tangent_vector_x = y;
                tangent_vector_y = -1 * x;

                x += tangent_vector_x * tan;
                y += tangent_vector_y * tan;

                x *= cos;
                y *= cos;
            }
        glEnd();
        x *= .85;
        y = 0;
        glColor3f(211/255.0, 211/255.0, 211/255.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glColor3d(1, 1, 1);
    glFrontFace(GL_CCW);
}
void drawDiamond(void)
{
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2d(-1, 1);
    glVertex2d(-2, 0);
    glVertex2d(-1, 0);
    glVertex2d(-1, -1);
    glVertex2d(0, 0);
    glEnd();

    glBegin(GL_LINES);
    glVertex2d(-1, 1);
    glVertex2d(0, 0);
    glEnd();
}
void updateText(void);

void drawBoard(void)
{
    float radius = 0.25f;
    glColor3d(1, 1, 1);
    glTranslated(0, 0, z_coord);
    glScaled(1.5, 1.5, 1.0);

    glLineWidth(3.0f);
    glEnable(GL_LINE_SMOOTH);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);// reset
    // draw place mats at vertices on board
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); int i = 0;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawCircle(-2, 0, radius); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawCircle(-1, 1, radius); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawSquare(-1, 0, radius*2); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawCircle(0, 0, radius); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawSquare(0, 1, radius*2); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawCircle(1, 1, radius); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawSquare(1, 0, radius*2); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawCircle(2, 0, radius); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawCircle(1, -1, radius); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawSquare(0, -1, radius*2); i++;

    glStencilFunc(GL_ALWAYS, i + 1, -1);
    drawCircle(-1, -1, radius);

    glLineWidth(0.5);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    drawDiamond();
    glPushMatrix();
        glTranslated(2, 0, 0);
        drawDiamond();
    glPopMatrix();
    glBegin(GL_LINES);
        glVertex2d(-1, 1);
        glVertex2d(1, 1); // top of board
        glVertex2d(-1, -1);
        glVertex2d(1, -1); // bottom of board
        glVertex2d(0, 1);
        glVertex2d(0, -1); // middle of board
    glEnd();

    //update text
    updateText();
}

void *font = GLUT_BITMAP_9_BY_15;
void updateText()
{
    // dimensions of current window
    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);
    setOrthographicProjection(window_width, window_height);
    renderString(window_width/5, window_height*6.5/7, font, "ROUND: 1", 8);
    renderString(window_width/5, window_height/7, font, "YOU: 1", 6);
    renderString(window_width*3.5/5, window_height/7, font, "OPP: 2", 6);
    renderString(window_width*2.25/5, window_height/7, font, "YOUR TURN", 9);
    restorePerspectiveProjection();
}

float *getSpacesWidthLengths(float width, int num_circles, float frac)
{
    float radius_circle, spaces_len; // unknown variables
    float spaces = num_circles + 1;
    /** solution to system of equations
        (num_circles)*(2*radius_circle) + (spaces)*(spaces_len)
        = width;
        spaces_len = frac * radius_circle)
        */
    radius_circle = width / (2*num_circles + spaces*frac);
    spaces_len = frac * radius_circle;
    float result[2] = { radius_circle, spaces_len };
    return result;
}
void setOrthographicProjection(int w, int h)
{
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);
    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();
    glLoadIdentity(); // reset matrix
    // set a 2D orthographic projection
    gluOrtho2D(0, w, 0, h);

    // Note: origin is at bottom left corner

    // switch back to modelview mode
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}
void restorePerspectiveProjection()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); // restore previous projection matrix
    // get back to modelview mode
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
void renderString(int x, int y, void *font, const char *str, int len)
{
    glRasterPos2i(x, y);
    for (int i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

void windowCoordToSpaceCoord(int x, int y)
{
    GLdouble z;
    cout << "inside windowCoordToSpaceCoord fn " << endl;
    //glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_DOUBLE, &z);
    //cout << "z was found: " << z << endl;

    GLdouble model;
    glGetDoublev(GL_MODELVIEW_MATRIX, &model);
    cout << "model: " << model;
    GLdouble proj;
    glGetDoublev(GL_PROJECTION_MATRIX, &proj);
    cout << "proj: " << proj;
    GLint view;
    glGetIntegerv(GL_VIEWPORT, &view);
    cout << "view: " << view;

    GLdouble objX, objY, objZ;
    gluUnProject(x, y, 1, &model, &proj, &view, &objX, &objY, &objZ);
    cout << "X: " << objX;
    cout << "objX: " << objX << " objY: " << objY << " objZ: " << objZ;
}

void displayEntireModel(float cx, float cy, float cz, float diam)
{
    cout << "displayEntireModel";
    gluLookAt(0.0, 0.0,diam,
              cx, cy, cz,
              0.0, 1.0, 0.0);
}
