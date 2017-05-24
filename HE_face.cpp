

#include "HE_face.h"

HE_face::HE_face()
{
	edge = NULL;
}

void HE_face::setEdge(HE_edge* e)
{
	edge = e;
}

HE_edge* HE_face::getEdge()
{
    return edge;
}

bool HE_face::direction()
{
    bool flag = false;
    GLfloat nn;
    HE_vert* v1 = edge->getVert();
    HE_vert* v2 = edge->getNext()->getVert();
    HE_vert* v3 = edge->getNext()->getNext()->getVert();
    Vector3f* end = new Vector3f(50,50,50);
    Vector3f* begin = new Vector3f();
    begin->setX((v1->getX()+v2->getX()+v3->getX())/3);
    begin->setY((v1->getY()+v2->getY()+v3->getY())/3);
    begin->setZ((v1->getZ()+v2->getZ()+v3->getZ())/3);
    end->sub(*begin);
    Vector3f* normal = new Vector3f();
    normal->setX((v1->getnX()+v2->getnX()+v3->getnX())/3);
    normal->setY((v1->getnY()+v2->getnY()+v3->getnY())/3);
    normal->setZ((v1->getnZ()+v2->getnZ()+v3->getnZ())/3);
    nn = end->product(*normal);
    if (nn > 0) {
        flag = true;
    }
    return flag;
}
