
#ifndef mesh_h
#define mesh_h

#include <vector>
#include "HE_edge.h"
#include "HE_face.h"
#include "HE_vert.h"
#include <GLUT/GLUT.h>

using namespace std;

struct Box {
    GLfloat xMin, xMax, yMin, yMax, zMin, zMax;
};

class Mesh
{
public:
    Mesh();
    int vertCount();
    int edgeCount();
    int faceCount();
    
    void addEdge(HE_edge* e);
    void addVertex(HE_vert* v);
    void addFace(HE_face* f);
    void addTriangle(int vec[3]);
    void getEdge();
    void getVertex(int index);
    void getFace();
    bool pairEdge(HE_edge* e);

	void setDenoised(bool flag);
	bool getDenoised();
    
    bool read(const std::string& mesh_path);
    void createEdges();
    void computeNormal();
   
    void computeCloudPointNormal();
    void releaseTempList();
    void computeBoundaryNormal();
    void drawAABB(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size);
    void renderPoints(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size);
	//void renderDenoisePoints(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size);
    void renderSolid(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size);
    void renderWireframe(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size);
    void renderFlat(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size);
	//void renderDenoiseFlat(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size);
    void box();
    void scaleAndMove();
    void scale();
    void move();
    void drawNormal(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size);

	void bilateralFiltering(GLfloat sigmac, GLfloat sigmas,int iteration);
    void cpbilateralFiltering(GLfloat sigmac, GLfloat sigmas,int iteration);
    void addNoise(int noise_type);
    vector<Vector3f> getVert();
    vector<vector<int>> getFacet();
  
    
private:
    void setup_mesh();
    void setup_gl();
    
    vector<HE_vert*> _boundary_vert;
    vector<HE_vert*> _vertices;
    vector<HE_face*> _faces;
    vector<HE_edge*> _edges;
    Box aabb;

	bool denoised = false;

	//vector<HE_vert*> neighborhood(HE_vert * vert);
};

#endif /* mesh_h */
