#ifndef OBJ_H
#define OBJ_H
#include <GL/glew.h>
#include "Game.h"



typedef struct coord {
    float x, y, z;
    coord() {};
    coord(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
} coord;
// in this project, obj files are being exported where texture is not included
typedef struct face_vertex {
    coord vertex;
    coord normal;
    face_vertex() {};
    face_vertex(coord _vertex, coord _normal) : vertex(_vertex), normal(_normal) {};
} face_vertex;
typedef struct face {
    std::vector<face_vertex> vertices;
    void addVertex(face_vertex v) { vertices.push_back(v); };
} face;

typedef struct Model {
    int verticesNum, positionsNum, normalsNum, facesNum, indicesNum;
    std::vector<coord> vertices, normals;
    std::vector<face> faces;
} Model;

class Obj
{
    public:
        Obj(player p);
        Obj(char *path, player p);

        void loadObj();
        void draw(float z, float _angle, float y_angle);
        virtual ~Obj();
    private:
        std::vector<coord> vertices, normals;
        int indicesNum;
        player _player;
        GLuint model;
        void init(player p);
        Model extractObjData(char *path);
        void parseData(char *path);
        void writeH(std::string fp, std::string nameObj, Model model);
};

#endif // OBJ_H
