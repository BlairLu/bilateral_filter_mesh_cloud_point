
#ifndef HE_vert_h
#define HE_vert_h

#include "HE_edge.h"
#include "Vector3f.h"
#include <vector>
#include <GLUT/GLUT.h>

using namespace std;

class HE_vert;
class HE_edge;

struct list {
    HE_vert* endvert;
    HE_edge* e;
};

class HE_vert {
    Vector3f position;
    Vector3f normal;
    Vector3f cpnormal;
	Vector3f denpos;
    //GLfloat x, y, z;
    //GLfloat nx, ny, nz;
    HE_edge* edge;
    vector<list*> tempList;
    vector<HE_vert*> cpneighs;
    
public:
    HE_vert();
    HE_vert(GLfloat x, GLfloat y, GLfloat z);
    ~HE_vert();
    
    void addEdge(HE_edge* edge);
    vector<HE_vert*> cloudpointGetneigh(vector<HE_vert*> Fulllist,HE_vert* vertex,Vector3f boxmax,Vector3f boxmin);
    void setNormal(GLfloat nnx, GLfloat nny, GLfloat nnz);
	void setDenpos(GLfloat ddx, GLfloat ddy, GLfloat ddz);
    bool computeVertNormal();
    bool computeCPVertNormal(vector<HE_vert*> FullVert,HE_vert* TARGET,Vector3f boundarymax,Vector3f boundrymin);
    void addTempList(HE_edge* e, HE_vert* v2);
    HE_edge* searchTempList(HE_vert* vert);

	//get the neighborhood of the vertex with tempList
	vector<HE_vert*> neighborhood();

    GLfloat getX();
    GLfloat getY();
    GLfloat getZ();
    GLfloat getnX();
    GLfloat getnY();
    GLfloat getnZ();
    
    GLfloat getcpnX();
    GLfloat getcpnY();
    GLfloat getcpnZ();
    
	GLfloat getdX();
	GLfloat getdY();
	GLfloat getdZ();
    void freeTempList();
    Vector3f* getPosition();
    HE_edge* getEdge();
    
     void getVertex(int index);
};

#endif /* HE_vert_h */
