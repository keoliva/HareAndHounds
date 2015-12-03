#include "Obj.h"

#include <fstream>
#include <iostream>
#include <cstring> /* strerror, memcpy */
#include <stdlib.h> /* exit */
#include "include/hounds.h"
#include "include/hare.h"
#define COORD 3
using namespace std;

void Obj::init(player p)
{
    _player = p;
    model = glGenLists(2);
}
Obj::Obj(char *path, player p)
{
    init(p);
    parseData(path);
}
Obj::Obj(player p)
{
    init(p);
}

Model Obj::extractObjData(char *path) {
    ifstream inObj(path, ios::in);
    if (!inObj.good()) {
        cout << strerror(errno);
        exit(1);
    }
    Model model = {0};
    string line, type;
    char *l;
    float x, y, z;
    while (getline(inObj, line)) {
        l = new char[line.size()+1];
        memcpy(l, line.c_str(), line.size()+1);
        type = line.substr(0, 2);
        if (type.compare("v ") == 0) {
            sscanf(l, "v %f %f %f", &x, &y, &z);
            model.vertices.push_back(coord(x, y, z));
        } else if (type.compare("vn") == 0) {
            sscanf(l, "vn %f %f %f", &x, &y, &z);
            model.normals.push_back(coord(x, y, z));
        } else if (type.compare("f ") == 0) {
            face currFace;
            int a, b, c, d, e, f;
            // REQUIRE obj to be triangulated so format of faces is
            // Vertex-Normal Indices Without Texture Coordinate Indices
            sscanf(l, "f %d//%d %d//%d %d//%d", &a, &b, &c, &d, &e, &f);
            // v0-v2, n0-n2 are indices references starting from 1
            currFace.vertices[0] = model.vertices[a - 1];
            currFace.normals[0] = model.normals[b - 1];
            currFace.vertices[1] = model.vertices[c - 1];
            currFace.normals[1] = model.normals[d - 1];
            currFace.vertices[2] = model.vertices[e - 1];
            currFace.normals[2] = model.normals[f - 1];
            model.faces.push_back(currFace);
            model.facesNum++;
        } else { continue; }
        delete[] l;
    }
    model.indicesNum = model.facesNum * 3;
    return model;
}

void Obj::parseData(char *path)
{
    Model model = extractObjData(path);
    this->indicesNum = model.indicesNum;
    for (face _face : model.faces) {
        for(int i = 0; i < 3; i++) {
            this->vertices.push_back(_face.vertices[i]);

            this->normals.push_back(_face.normals[i]);
        }
    }
    if (_player == HOUNDS) {
        writeH("include/hounds.h", "hounds", model);
    } else { // only other passed should be HARE
        writeH("include/hare.h", "hare", model);
    }
}
void Obj::writeH(std::string fp, std::string nameObj, Model model)
{
    ofstream outH(fp, ios::out);
    if (!outH.good()) {
        cout << strerror(errno);
        exit(1);
    }
    outH << "// This is a .cpp file for the model: " << nameObj << endl;
    outH << endl;

    outH << "// Faces: " << model.facesNum << endl;
    outH << "// Number of Indices: " << model.indicesNum << endl;

    outH << "namespace " << nameObj << endl;
    outH << "{" << endl;
    outH << "\tconst int indicesNum = " << this->indicesNum << ";" << endl;
    outH << "\tconst float vertices[" << this->vertices.size()*3 << "] = {" << endl;
    for (coord v : this->vertices)
        outH << "\t\t" << v.x << ", " << v.y << ", " << v.z << "," << endl;
    outH << "\t};" << endl;

    outH << "\tconst float normals[" << this->normals.size()*3 << "] = {" << endl;
     for (coord v : this->normals)
        outH << "\t\t" << v.x << ", " << v.y << ", " << v.z << "," << endl;
    outH << "\t};" << endl;

    outH << "}" << endl;
    outH.close();
}

void Obj::loadObj()
{
    GLuint num = (_player == HOUNDS) ? model : model+1;
    glNewList(num, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
            int _size = (_player == HOUNDS) ? hounds::indicesNum*COORD : hare::indicesNum*COORD;
            float vx, vy, vz, nx, ny, nz;
            // every 9 elements represents the xyz-coordinates of the three vertices/normals of a face
            for (int i = 0; i < _size; i+=(COORD*3)) { // COORD*3 = 9
                for (int j = i; j < (i + 9); j+=COORD) {
                    if (_player == HOUNDS) {
                        vx = hounds::vertices[j]; vy = hounds::vertices[j+1]; vz = hounds::vertices[j+2];
                        nx = hounds::normals[j]; ny = hounds::normals[j+1]; nz = hounds::normals[j+2];
                    } else { // HARE is passed
                        vx = hare::vertices[j]; vy = hare::vertices[j+1]; vz = hare::vertices[j+2];
                        nx = hare::normals[j]; ny = hare::normals[j+1]; nz = hare::normals[j+2];
                    }
                    glNormal3f(nx, ny, nz);
                    glVertex3f(vx, vy, vz);
                }
            }
        glEnd();
    }
    glPopMatrix();
    glEndList();
}

void Obj::draw(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(180, 0, 1, 0);
    if (_player == HOUNDS) {
        glColor3f(66/255.0, 24/255.0, 5/255.0);
        glCallList(model);
    } else {
        glColor3f(241/255.0, 236/255.0, 214/255.0);
        glCallList(model+1);
    }
    glPopMatrix();
    glColor3d(1, 1, 1);
}

Obj::~Obj()
{
    //dtor
}
