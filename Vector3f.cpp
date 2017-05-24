

#include <stdio.h>
#include <math.h>
#include "Vector3f.h"

Vector3f::Vector3f()
{
    x = y = z = 0;
}

Vector3f::Vector3f(GLfloat xx, GLfloat yy, GLfloat zz)
{
    x = xx;
    y = yy;
    z = zz;
}

void Vector3f::add(Vector3f v2)
{
    x = x + v2.x;
    y = y + v2.y;
    z = z + v2.z;
}

void Vector3f::sub(Vector3f v2)
{
    x = x - v2.x;
    y = y - v2.y;
    z = z - v2.z;
}

GLfloat Vector3f::getX()
{
    return x;
}

GLfloat Vector3f::getY()
{
    return y;
}

GLfloat Vector3f::getZ()
{
    return z;
}

void Vector3f::setX(GLfloat xx)
{
    x = xx;
}

void Vector3f::setY(GLfloat yy)
{
    y = yy;
}

void Vector3f::setZ(GLfloat zz)
{
    z = zz;
}

void Vector3f::normalize()
{
    GLfloat denominator = sqrt(powf(x, 2) + powf(y, 2) + powf(z, 2));
    x = -(x/denominator);
    y = -(y/denominator);
    z = -(z/denominator);
}

void Vector3f::divide(GLfloat scale)
{
    if (scale!=0) {
        x = x/scale;
        y = y/scale;
        z = z/scale;
    }
}

GLfloat Vector3f::product(Vector3f v2)
{
    x = x*v2.getX();
    y = y*v2.getY();
    z = z*v2.getZ();
    GLfloat n =x + y + z;
    return n;
}
