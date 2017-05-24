

#include "HE_edge.h"
#include <string>

HE_edge::HE_edge()
{
	prev = next = pair = NULL;
    face = NULL;
    vert = NULL;
}

HE_edge::HE_edge(HE_vert* v)
{
    prev = next = pair = NULL;
    face = NULL;
    vert = v;
}

void HE_edge::setVert(HE_vert* v)
{
	vert = v;
}

void HE_edge::setFace(HE_face* f)
{
	face = f;
}

void HE_edge::setEdge(HE_edge* e, std::string str)
{
    if (str == "prev") {
        prev = e;
    }
    else if (str == "next")
    {
        next = e;
    }
    else if(str == "pair")
    {
        pair = e;
    }
    else
    {
        printf("error!");
    }
}

HE_edge* HE_edge::getPair()
{
    return pair;
}

HE_vert* HE_edge::getVert()
{
    return vert;
}

HE_edge* HE_edge::getNext()
{
    return next;
}

HE_edge* HE_edge::getPrev()
{
    return prev;
}

Vector3f HE_edge::edgeToVector()
{
    Vector3f vv;
    vv.setX(next->vert->getX() - vert->getX());
    vv.setY(next->vert->getY() - vert->getY());
    vv.setZ(next->vert->getZ() - vert->getZ());
    
    return vv;
}

Vector3f HE_edge::crossProduct(HE_edge *e)
{
    Vector3f a = edgeToVector();
    Vector3f b = e->edgeToVector();
    Vector3f cross;
    cross.setX(a.getY()*b.getZ()-a.getZ()*b.getY());
    cross.setY(a.getZ()*b.getX()-a.getX()*b.getZ());
    cross.setZ(a.getX()*b.getY()-a.getY()*b.getX());
    return cross;
}
