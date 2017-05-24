

#ifndef HE_edge_h
#define HE_edge_h

#include "HE_vert.h"
#include "Vector3f.h"
#include "HE_face.h"
#include <string>

class HE_face;
class HE_vert;

class HE_edge {
    HE_edge* prev;
    HE_edge* next;
    HE_edge* pair;
    HE_vert* vert;
    HE_face* face;
    
public:
    HE_edge();
    HE_edge(HE_vert* v);
    void setEdge(HE_edge* e, std::string s);
    void setFace(HE_face* f);
    void setVert(HE_vert* v);
    Vector3f crossProduct(HE_edge* e);
    Vector3f edgeToVector();
    HE_edge* getPair();
    HE_vert* getVert();
    HE_edge* getNext();
    HE_edge* getPrev();
};

#endif /* HE_edge_h */


