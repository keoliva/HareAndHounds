#ifndef OBJ_H
#define OBJ_H
#include "Game.h"
#include <GL/gl.h>

typedef struct coord {
    float x, y, z;
    coord() {};
    coord(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
} coord;
// in this project, obj files are being exported where texture is not included
// and the faces are triangulated
typedef struct face {
    coord vertices[3];
    coord normals[3];
} face;

typedef struct Model {
    int facesNum, indicesNum;
    std::vector<coord> vertices, normals;
    std::vector<face> faces;
} Model;

class Obj
{
    public:
        Obj() {};
        Obj(player p);
        Obj(char *path, player p);
        void loadObj();
        void draw(float x, float y);
        virtual ~Obj();
    private:
        int model;
        std::vector<coord> vertices, normals;
        int indicesNum;
        player _player;

        void init(player p);
        Model extractObjData(char *path);
        void parseData(char *path);
        void writeH(std::string fp, std::string nameObj, Model model);
};

#endif // OBJ_H
