#include "Obj.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "include/hounds.h"
#define COORD 3
using namespace std;

void Obj::init(player p)
{
    _player = p;
    if (_player == HOUNDS) {
        model = glGenLists(1);
    } else { // otherwise, HARE is passed
        model = glGenLists(2);
    }
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

// returns a Model object with a count of the obj's
// vertices, normals, faces, and
// the coordinates for each in a coor object
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
            model.verticesNum++;
        } else if (type.compare("vn") == 0) {
            sscanf(l, "vn %f %f %f", &x, &y, &z);
            model.normals.push_back(coord(x, y, z));
            model.normalsNum++;
        } else if (type.compare("f ") == 0) {
            face currFace;
            int a, b, c, d, e, f;
            // REQUIRE obj to be triangulated
            // also faces format is : Vertex Normal Indices Without Texture Coordinate Indices
            //sscanf(l, "f %d//%d %d//%d %d//%d", &a, &b, &c, &d, &e, &f);
            sscanf(l, "f %d//%d %d//%d %d//%d", &a, &b, &c, &d, &e, &f);
            // a-f are indices starting from 1
            face_vertex v0, v1, v2;
            v0 = face_vertex(model.vertices[a - 1], model.normals[b - 1]);
            v1 = face_vertex(model.vertices[c - 1], model.normals[d - 1]);
            v2 = face_vertex(model.vertices[e - 1], model.normals[f - 1]);
            currFace.addVertex(v0); currFace.addVertex(v1); currFace.addVertex(v2);
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
        for(face_vertex fv : _face.vertices) {
            coord fv_v = fv.vertex;
            coord fv_n = fv.normal;

            this->vertices.push_back(fv_v);

            this->normals.push_back(fv_n);
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

    outH << "// Faces: " << model.faces.size() << endl;
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
    glNewList(model, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
            int _size = hounds::indicesNum * COORD;
            // _size is the length of normals and vertices
            for (int i = 0; i < _size; i+=9) {
                for (int j = i; j < (i + 9); j+=3) {
                    glNormal3f(hounds::normals[j], hounds::normals[j+1], hounds::normals[j+1]);
                    glVertex3f(hounds::vertices[j], hounds::vertices[j+1], hounds::vertices[j+1]);
                }
            }
        glEnd();
    }
    glPopMatrix();
    glEndList();
}

void Obj::draw(float z, float _angle, float y_angle)
{
    glPushMatrix();
    glTranslated(0, 0, z);
    glColor3f(1.0, 0.23, 0.27);
    glRotatef(_angle, 0, 1, 0);
    glRotated(y_angle, 1, 0, 0);
    glCallList(model);
    glPopMatrix();
    /**glPushMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
                glVertexPointer(3, GL_FLOAT, 0, this->vertices);
                glNormalPointer(GL_FLOAT, 0, this->normals);

                glDrawElements(GL_TRIANGLES, this->indicesNum, GL_UNSIGNED_SHORT, this->indices);
            glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();*/
}

Obj::~Obj()
{
    //dtor
}
