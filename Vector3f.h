

#ifndef Vector3f_h
#define Vector3f_h
#include <GLUT/GLUT.h>

class Vector3f {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    
public:
    Vector3f();
    Vector3f(float xx, float yy, float zz);
    
    void add(Vector3f);
    GLfloat getX();
    GLfloat getY();
    GLfloat getZ();
    
    void setX(GLfloat x);
    void setY(GLfloat y);
    void setZ(GLfloat z);
    void normalize();
    void sub(Vector3f v2);
    void divide(GLfloat scale);
    GLfloat product(Vector3f v2);
};

#endif /* Vector3f_h */
