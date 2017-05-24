
#ifndef HE_face_h
#define HE_face_h

#include "HE_edge.h"

class HE_edge;

class HE_face {
    HE_edge* edge;
    
public:
	HE_face();
    void setEdge(HE_edge* e);
    HE_edge* getEdge();
    bool direction();
};

#endif /* HE_face_h */

