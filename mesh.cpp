

#include <stdio.h>
#include "mesh.h"
#include <iostream>
#include "Vector3f.h"
#include <math.h>


Mesh::Mesh()
{
    aabb.xMin = aabb.yMin = aabb.zMin = INT_MAX;
    aabb.xMax = aabb.yMax = aabb.zMax = INT_MIN;
    
    vector<HE_vert*> _vertices;
    vector<HE_face*> _faces;
    vector<HE_edge*> _edges;
}

void Mesh::getVertex(int index)
{
    _vertices.at(index);
}

void Mesh::addEdge(HE_edge* e)
{
    _edges.push_back(e);
}

void Mesh::addVertex(HE_vert *v)
{
    int lala=_vertices.size();
    v->SetOrder(lala);
    _vertices.push_back(v);
    
}

void Mesh::addFace(HE_face *f)
{
    _faces.push_back(f);
}

void Mesh::box()
{
    for (int i = 0; i < _vertices.size(); i ++) {
        GLfloat ax = _vertices.at(i)->getX();
        GLfloat ay = _vertices.at(i)->getY();
        GLfloat az = _vertices.at(i)->getZ();
        if (ax < aabb.xMin) {
            aabb.xMin = ax;
        }
        if (ax > aabb.xMax) {
            aabb.xMax = ax;
        }
        if (ay < aabb.yMin) {
            aabb.yMin = ay;
        }
        if (ay > aabb.yMax) {
            aabb.yMax = ay;
        }
        if (az < aabb.zMin) {
            aabb.zMin = az;
        }
        if (az > aabb.zMax) {
            aabb.zMax = az;
        }
    }
}

void Mesh::addTriangle(int vec[3])
{
    HE_vert* v1 = _vertices.at(vec[0]-1);
    HE_vert* v2 = _vertices.at(vec[1]-1);
    HE_vert* v3 = _vertices.at(vec[2]-1);
    HE_edge* edge1 = new HE_edge(v1);
    HE_edge* edge2 = new HE_edge(v2);
    HE_edge* edge3 = new HE_edge(v3);
    v1->addTempList(edge1, v2);
    v2->addTempList(edge2, v3);
    v3->addTempList(edge3, v1);
    v1->addEdge(edge1);
    v2->addEdge(edge2);
    v3->addEdge(edge3);
    
    edge1->setEdge(edge2, "next");
    edge1->setEdge(edge3, "prev");
    edge2->setEdge(edge1, "prev");
    edge2->setEdge(edge3, "next");
    edge3->setEdge(edge2, "prev");
    edge3->setEdge(edge1, "next");
    
    HE_face* f = new HE_face();
    f->setEdge(edge1);
    
    edge1->setFace(f);
    edge2->setFace(f);
    edge3->setFace(f);
    
    _faces.push_back(f);
    
    pairEdge(edge1);
    pairEdge(edge2);
    pairEdge(edge3);
    
    _edges.push_back(edge1);
    _edges.push_back(edge2);
    _edges.push_back(edge3);
}

bool Mesh::pairEdge(HE_edge *e)
{
    HE_edge* pairedge =new HE_edge();
    if (e->getPair()== NULL) {
        //flag = true;
        HE_edge* nextedge = e->getNext();
        HE_vert* v = nextedge->getVert();
        pairedge = v->searchTempList(e->getVert());
        
        e->setEdge(pairedge, "pair");
        if (pairedge != NULL) {
            pairedge->setEdge(e, "pair");
        }
    }
    return 1;
}

void Mesh::setDenoised(bool flag)
{
    denoised = flag;
}

bool Mesh::getDenoised()
{
    return denoised;
}

void Mesh::computeCloudPointNormal()
{
  ;
    Vector3f max,min;
    max.setX(aabb.xMax);
    max.setY(aabb.yMax);
    max.setZ(aabb.zMax);
    
    min.setX(aabb.xMin);
    min.setY(aabb.yMin);
    min.setZ(aabb.zMin);
    for (int i = 0; i < _vertices.size(); i ++)
    {
        bool flag = true;
       
        
       //  vector<HE_vert*> neighbors = _vertices.at(i)->cloudpointGetneigh(_vertices,_vertices.at(i),max,min);
        flag = _vertices.at(i)->computeCPVertNormal(_vertices,_vertices.at(i),max,min);//set cloud point vertex normal
    }
    
}



void Mesh::computeNormal()
{
        bool flag = true;
    for (int i = 0; i < _vertices.size(); i ++) {
        flag = _vertices.at(i)->computeVertNormal(); //已把normal set好
        if (flag == false) {
            _boundary_vert.push_back(_vertices.at(i));
            flag = true;
        }
    }
    computeBoundaryNormal();
}

void Mesh::releaseTempList()
{
    for (int i=0; i < _vertices.size(); i ++) {
        _vertices.at(i)->freeTempList();
    }
}

void Mesh::computeBoundaryNormal()
{
    for (int i = 0; i < _edges.size(); i ++) {
        if (_edges.at(i)->getPair() == NULL) {
        }
    }
    for (int i = 0; i < _boundary_vert.size();i ++) {
        HE_vert* v1 = _boundary_vert.at(i);
        HE_edge* outgoing = v1->getEdge();
        HE_edge* curr = outgoing;
        Vector3f* outcome = new Vector3f(0, 0, 0);
        //cck
        while (curr->getPair()!=NULL) {
            outcome->add(curr->crossProduct(curr->getPair()->getNext()));
            curr = curr->getPair()->getNext();
        };
        curr = outgoing;
        while (curr->getPrev()->getPair()!=NULL) {
            outcome->add((curr->getPrev()->getPair())->crossProduct(curr));
            curr = curr->getPrev()->getPair();
        }
        outcome->normalize();
        _boundary_vert.at(i)->setNormal(outcome->getX(), outcome->getY(), outcome->getZ());
    }
}

void Mesh::drawAABB(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPushMatrix();
    glRotatef(x_angle, 0, 1,0);
    glRotatef(y_angle, 1,0,0);
    glScalef(scale_size, scale_size, scale_size);
    glTranslatef(xShift, yShift, 0);
    
    glColor3f(0, 1, 1);
    glLineWidth(3.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(aabb.xMin, aabb.yMax, aabb.zMin);
    glVertex3f(aabb.xMin, aabb.yMax, aabb.zMax);
    glVertex3f(aabb.xMax, aabb.yMax, aabb.zMax);
    glVertex3f(aabb.xMax, aabb.yMax, aabb.zMin);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glVertex3f(aabb.xMin, aabb.yMin, aabb.zMin);
    glVertex3f(aabb.xMin, aabb.yMin, aabb.zMax);
    glVertex3f(aabb.xMax, aabb.yMin, aabb.zMax);
    glVertex3f(aabb.xMax, aabb.yMin, aabb.zMin);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex3f(aabb.xMin, aabb.yMax, aabb.zMin);
    glVertex3f(aabb.xMin, aabb.yMin, aabb.zMin);
    glVertex3f(aabb.xMin, aabb.yMax, aabb.zMax);
    glVertex3f(aabb.xMin, aabb.yMin, aabb.zMax);
    glVertex3f(aabb.xMax, aabb.yMax, aabb.zMax);
    glVertex3f(aabb.xMax, aabb.yMin, aabb.zMax);
    glVertex3f(aabb.xMax, aabb.yMax, aabb.zMin);
    glVertex3f(aabb.xMax, aabb.yMin, aabb.zMin);
    glEnd();
    glPushMatrix();
}

void Mesh::renderPoints(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPushMatrix();
    glRotatef(x_angle, 0, 1,0);
    glRotatef(y_angle, 1,0,0);
    glScalef(scale_size, scale_size, scale_size);
    glTranslatef(xShift, yShift, 0);
    
    // points rendering
    glPointSize(2);            // display the control points
    glColor3f(1, 0, 0);
    glBegin(GL_POINTS);
    for (int i = 0; i<_vertices.size(); i++)
    {
        if (denoised)
            glVertex3f(_vertices.at(i)->getdX(), _vertices.at(i)->getdY(), _vertices.at(i)->getdZ());
        else
            glVertex3f(_vertices.at(i)->getX(), _vertices.at(i)->getY(), _vertices.at(i)->getZ());
    }
    glEnd();
    glPushMatrix();
}

void Mesh::renderFlat(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPushMatrix();
    glRotatef(x_angle, 0, 1,0);
    glRotatef(y_angle, 1,0,0);
    glScalef(scale_size, scale_size, scale_size);
    glTranslatef(xShift, yShift, 0);
    
    glColor3f(1, 0, 0);
    
    glShadeModel(GL_FLAT);
    for (int i=0; i<_faces.size(); i++){
        glBegin(GL_TRIANGLES);
        HE_edge* e1 = _faces.at(i)->getEdge();
        HE_vert* v1 = e1->getVert();
        glNormal3f(v1->getnX(), v1->getnY(), v1->getnZ());
        
        HE_edge* e2 = e1->getNext();
        HE_vert* v2 = e2->getVert();
        glNormal3f(v2->getnX(), v2->getnY(), v2->getnZ());
        
        HE_edge* e3 = e2->getNext();
        HE_vert* v3 = e3->getVert();
        glNormal3f(v3->getnX(), v3->getnY(), v3->getnZ());
        if (denoised) {
            glVertex3f(v1->getdX(), v1->getdY(), v1->getdZ());
            glVertex3f(v2->getdX(), v2->getdY(), v2->getdZ());
            glVertex3f(v3->getdX(), v3->getdY(), v3->getdZ());
        }
        else {
            glVertex3f(v1->getX(), v1->getY(), v1->getZ());
            glVertex3f(v2->getX(), v2->getY(), v2->getZ());
            glVertex3f(v3->getX(), v3->getY(), v3->getZ());
        }
        
        glEnd();
    }
    glPopMatrix();
}


void Mesh::renderSolid(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPushMatrix();
    glRotatef(x_angle, 0, 1,0);
    glRotatef(y_angle, 1,0,0);
    glScalef(scale_size, scale_size, scale_size);
    glTranslatef(xShift, yShift, 0);
    
    glColor3f(1, 0, 0);
    glShadeModel(GL_SMOOTH);
    for (int i=0; i<_faces.size(); i++){
        glBegin(GL_TRIANGLES);
        
        HE_edge* e1 = _faces.at(i)->getEdge();
        HE_vert* v1 = e1->getVert();
        glNormal3f(v1->getnX(), v1->getnY(), v1->getnZ());
        
        HE_edge* e2 = e1->getNext();
        HE_vert* v2 = e2->getVert();
        glNormal3f(v2->getnX(), v2->getnY(), v2->getnZ());
        
        HE_edge* e3 = e2->getNext();
        HE_vert* v3 = e3->getVert();
        glNormal3f(v3->getnX(), v3->getnY(), v3->getnZ());
        
        if (denoised) {
            glVertex3f(v1->getdX(), v1->getdY(), v1->getdZ());
            glVertex3f(v2->getdX(), v2->getdY(), v2->getdZ());
            glVertex3f(v3->getdX(), v3->getdY(), v3->getdZ());
        }
        else {
            glVertex3f(v1->getX(), v1->getY(), v1->getZ());
            glVertex3f(v2->getX(), v2->getY(), v2->getZ());
            glVertex3f(v3->getX(), v3->getY(), v3->getZ());
        }
        
        glEnd();
    }
    glPopMatrix();
}

void Mesh::renderWireframe(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPushMatrix();
    glRotatef(x_angle, 0, 1,0);
    glRotatef(y_angle, 1,0,0);
    glScalef(scale_size, scale_size, scale_size);
    glTranslatef(xShift, yShift, 0);
    
    glLineWidth(0.5);
    for (int i=0; i<_faces.size(); i++){
        glBegin(GL_LINE_LOOP);
        HE_edge* e1 = _faces.at(i)->getEdge();
        HE_vert* v1 = e1->getVert();
        
        HE_edge* e2 = e1->getNext();
        HE_vert* v2 = e2->getVert();
        
        HE_edge* e3 = e2->getNext();
        HE_vert* v3 = e3->getVert();
        
        if (denoised) {
            glVertex3f(v1->getdX(), v1->getdY(), v1->getdZ());
            glVertex3f(v2->getdX(), v2->getdY(), v2->getdZ());
            glVertex3f(v3->getdX(), v3->getdY(), v3->getdZ());
        }
        else {
            glVertex3f(v1->getX(), v1->getY(), v1->getZ());
            glVertex3f(v2->getX(), v2->getY(), v2->getZ());
            glVertex3f(v3->getX(), v3->getY(), v3->getZ());
        }
        
        glEnd();
        
    }
    glPopMatrix();
}

void Mesh::drawNormal(GLfloat x_angle, GLfloat y_angle, GLfloat xShift, GLfloat yShift, GLfloat scale_size)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPushMatrix();
    glRotatef(x_angle, 0, 1,0);
    glRotatef(y_angle, 1,0,0);
    glScalef(scale_size, scale_size, scale_size);
    glTranslatef(xShift, yShift, 0);
    glColor3f(0, 1, 0);
    glLineWidth(0.5);
    
    for (int i = 0; i < _vertices.size(); i ++) {
        glBegin(GL_LINES);
        GLfloat sx, sy, sz;
        GLfloat ex, ey, ez;
        ex = _vertices.at(i)->getnX()+_vertices.at(i)->getX();
        ey = _vertices.at(i)->getnY()+_vertices.at(i)->getY();
        ez = _vertices.at(i)->getnZ()+_vertices.at(i)->getZ();
        sx = _vertices.at(i)->getX();
        sy = _vertices.at(i)->getY();
        sz = _vertices.at(i)->getZ();
        glVertex3f(sx, sy, sz);
        glVertex3f(ex, ey, ez);
        glEnd();
    }
    glPopMatrix();
}

void Mesh::scale()
{
    GLfloat xl = aabb.xMax - aabb.xMin;
    GLfloat yl = aabb.yMax - aabb.yMin;
    GLfloat zl = aabb.zMax - aabb.zMin;
    GLfloat scale, max;
    if (xl>yl) {
        max = xl;
        scale = xl/20;
    } else {
        max = yl;
        scale = yl/20;
    }
    if (zl > max) {
        scale = zl/20;
    }
    
    for (int i = 0; i < _vertices.size(); i ++) {
        Vector3f* p = _vertices.at(i)->getPosition();
        p->divide(scale);
    }
    /*
     aabb.xMax = aabb.xMax/scale;
     aabb.yMax = aabb.yMax/scale;
     aabb.zMax = aabb.zMax/scale; */
}

void Mesh::move()
{
    Vector3f v1;
    v1.setX(aabb.xMin);
    v1.setY(aabb.yMin);
    v1.setZ(aabb.zMin);
    
    for (int i = 0; i < _vertices.size(); i ++) {
        Vector3f* p = _vertices.at(i)->getPosition();
        p->sub(v1);
    }
    /*
     aabb.xMax -= aabb.xMin;
     aabb.xMin -= aabb.xMin;
     aabb.yMax -= aabb.yMin;
     aabb.yMin -= aabb.yMin;
     aabb.zMax -= aabb.zMin;
     aabb.zMin -= aabb.zMin;
     */
}

void Mesh::scaleAndMove()
{
    box();
    //modelCentroid
    scale();
    //move();
    box();
}

//vector<HE_vert*> Mesh::neighborhood(HE_vert * vert)
//{
//	vector<HE_vert*> neighbors;
//	HE_edge * outgoing_he = vert->getEdge();
//	HE_edge * curr = outgoing_he;
//
//	while (curr->getPair() != NULL) {
//		neighbors.push_back(curr->getPair()->getVert());
//		curr = curr->getPair()->getNext();
//		if (curr == outgoing_he) break;
//	}
//
//	if (curr->getPair() == NULL) {
//		neighbors.push_back(curr->getNext()->getVert());
//		curr = outgoing_he->getPrev();
//		while (1) {
//			neighbors.push_back(curr->getVert());
//			if (curr->getPair() != NULL)
//				curr = curr->getPair()->getPrev();
//			else break;
//		}
//	}
//
//	return neighbors;
//}

void Mesh::bilateralFiltering(GLfloat sigmac, GLfloat sigmas,int iteration)
{
    //interation
    for(int it=0;it<iteration;it++){
        
        //bilateral filtering on each vertex
    for (int i = 0; i < _vertices.size(); i++) {
        
        //for ith vertex 'vert'
        HE_vert * vert = _vertices.at(i);
        
        // vert->setDenpos(vert->getX(), vert->getY(), vert->getZ());
        //}
        
        /*vector<HE_vert*> neighbors = neighborhood(vert);*/
       
        vector<HE_vert*> neighbors = vert->neighborhood();
        GLfloat sum = 0, normalizer = 0;
        
        /*for(int it=0;it<iteration;it++){
         vector<Vector3f> tempVertices;
         for (int i = 0; i < _vertices.size(); i++) {
         Vector3f tempV;
         HE_vert * vert = _vertices.at(i);
         /*vector<HE_vert*> neighbors = neighborhood(vert);*/
        /*vector<HE_vert*> neighbors = vert->neighborhood();
         GLfloat sum = 0, normalizer = 0;*/
        
        
        
        for (int j = 0; j < neighbors.size(); j++) {
            HE_vert * q = neighbors.at(j);
            GLfloat t, h, wc, ws;
            
            t = sqrt(pow(vert->getX() - q->getX(), 2) + pow(vert->getY() - q->getY(), 2) + pow(vert->getZ() - q->getZ(), 2));
            h = vert->getnX() * (vert->getX() - q->getX()) +
            vert->getnY() * (vert->getY() - q->getY()) +
            vert->getnZ() * (vert->getZ() - q->getZ());
            wc = exp(-t * t / (2 * sigmac * sigmac));
            ws = exp(-h * h / (2 * sigmas * sigmas));
            sum += (wc * ws * h);
            normalizer += wc * ws;
        }
        GLfloat temp = sum / normalizer;
        vert->setDenpos(vert->getX() + vert->getnX() * temp, vert->getY() + vert->getnY() * temp, vert->getZ() + vert->getnZ() * temp);
        vert->setPosition(vert->getX() + vert->getnX() * temp, vert->getY() + vert->getnY() * temp, vert->getZ() + vert->getnZ() * temp);
        /*tempV.setX(vert->getdX() + vert->getnX() * temp);
         tempV.setY(vert->getdY() + vert->getnY() * temp);
         tempV.setZ(vert->getdZ() + vert->getnZ() * temp);
         tempVertices.push_back(tempV);*/
    }
    /*for (int j = 0; j < _vertices.size(); j++) {
     HE_vert * changeDepos = _vertices.at(j);
     changeDepos->setDenpos(tempVertices.at(j).getX(), tempVertices.at(j).getY(), tempVertices.at(j).getZ());*/
}
 }
//}

void Mesh::cpbilateralFiltering(GLfloat sigmac, GLfloat sigmas,int iteration)
{
    Box boundry;
    Vector3f max,min;
    max.setX(boundry.xMax);
    max.setY(boundry.yMax);
    max.setZ(boundry.zMax);
    
    min.setX(boundry.xMin);
    min.setY(boundry.yMin);
    min.setZ(boundry.zMin);
    //interation
    for(int it=0;it<iteration;it++){
        
        //bilateral filtering on each vertex
        for (int i = 0; i < _vertices.size(); i++) {
            
            //for ith vertex 'vert'
            HE_vert * vert = _vertices.at(i);
            
            // vert->setDenpos(vert->getX(), vert->getY(), vert->getZ());
            //}
            
            /*vector<HE_vert*> neighbors = neighborhood(vert);*/
           
            vector<HE_vert*> neighbors = vert->cloudpointGetneigh();
            GLfloat sum = 0, normalizer = 0;
            
            /*for(int it=0;it<iteration;it++){
             vector<Vector3f> tempVertices;
             for (int i = 0; i < _vertices.size(); i++) {
             Vector3f tempV;
             HE_vert * vert = _vertices.at(i);
             /*vector<HE_vert*> neighbors = neighborhood(vert);*/
            /*vector<HE_vert*> neighbors = vert->neighborhood();
             GLfloat sum = 0, normalizer = 0;*/
            
            
            
            for (int j = 0; j < neighbors.size(); j++) {
                HE_vert * q = neighbors.at(j);
                GLfloat t, h, wc, ws;
                
                t = sqrt(pow(vert->getX() - q->getX(), 2) + pow(vert->getY() - q->getY(), 2) + pow(vert->getZ() - q->getZ(), 2));
                h = vert->getnX() * (vert->getX() - q->getX()) +
                vert->getnY() * (vert->getY() - q->getY()) +
                vert->getnZ() * (vert->getZ() - q->getZ());
                wc = exp(-t * t / (2 * sigmac * sigmac));
                ws = exp(-h * h / (2 * sigmas * sigmas));
                sum += (wc * ws * h);
                normalizer += wc * ws;
            }
            GLfloat temp = sum / normalizer;
         vert->setDenpos(vert->getX() + vert->getcpnX() * temp, vert->getY() + vert->getcpnY() * temp, vert->getZ() + vert->getcpnZ() * temp);
            vert->setPosition(vert->getX() + vert->getcpnX() * temp, vert->getY() + vert->getcpnY() * temp, vert->getZ() + vert->getcpnZ() * temp);
            
            /*tempV.setX(vert->getdX() + vert->getnX() * temp);
             tempV.setY(vert->getdY() + vert->getnY() * temp);
             tempV.setZ(vert->getdZ() + vert->getnZ() * temp);
             tempVertices.push_back(tempV);*/
        }
        /*for (int j = 0; j < _vertices.size(); j++) {
         HE_vert * changeDepos = _vertices.at(j);
         changeDepos->setDenpos(tempVertices.at(j).getX(), tempVertices.at(j).getY(), tempVertices.at(j).getZ());*/
    }

}
int Mesh::vertCount()
{
    return _vertices.size();
}

int Mesh::edgeCount()
{
    return _edges.size();
}

int Mesh::faceCount()
{
    return _faces.size();
}



vector<Vector3f> Mesh::getVert()
{
    vector<Vector3f> ans;
    for(int i=0;i<_vertices.size();i++)
    {
        
        GLfloat res1=_vertices.at(i)->getX();
        GLfloat res2=_vertices.at(i)->getY();
        GLfloat res3=_vertices.at(i)->getZ();
        Vector3f res;
        res.setX(res1);
        res.setY(res2);
        res.setZ(res3);
        ans.push_back(res);
    }
    return ans;
}
vector<vector<int>> Mesh::getFacet()
{
    vector<vector<int>> Answer;
    Mesh* _currMesh = new Mesh();
    
    
    for(int i=0;i<_faces.size();i++)
    {
        HE_edge* first;
        first=_faces.at(i)->getEdge();
        HE_vert* firstV=first->getVert();
        int v1=firstV->getOrder();
        
        
        HE_edge *second,*second2;
        second=_faces.at(i)->getEdge();
        second2=second->getNext();
        HE_vert* secondV=second2->getVert();
        int v2=secondV->getOrder();
        
        HE_edge *third,*third2;
        third=_faces.at(i)->getEdge();
        third2=second->getPrev();
        HE_vert* thirdV=third2->getVert();
        int v3=thirdV->getOrder();
        
        vector<int> res;
        res.push_back(v1);
        res.push_back(v2);
        res.push_back(v3);
        Answer.push_back(res);
    }
    return Answer;
}
void Mesh::addNoise(int Noise_Type)
{
    
    if(Noise_Type==99)
    {
        double distance;
        for (int i = 0; i < _vertices.size(); i++) {
            HE_vert * vert = _vertices.at(i);
            
            
            distance = 0;
            
            //vert->updatePosition(vert->getX() + distance * vert->getnX(), vert->getY() + distance * vert->getnY(), vert->getZ() + distance * vert->getnZ());
            //设置点位置
            vert->setDenpos(vert->getX() + distance * vert->getnX(), vert->getY() + distance * vert->getnY(), vert->getZ() + distance * vert->getnZ());}
    }
    else if(Noise_Type==100)
    {
    double distance;
    srand((unsigned)time(NULL));
    for (int i = 0; i < _vertices.size(); i++) {
        HE_vert * vert = _vertices.at(i);
        
        //随机数范围-0.005~0.005 
        distance = rand() / double(RAND_MAX);
        distance = (distance-0.8)/2;
        
        //vert->updatePosition(vert->getX() + distance * vert->getnX(), vert->getY() + distance * vert->getnY(), vert->getZ() + distance * vert->getnZ());
        //设置点位置
        vert->setDenpos(vert->getX() + distance * vert->getnX(), vert->getY() + distance * vert->getnY(), vert->getZ() + distance * vert->getnZ());
    }
    }
    else if(Noise_Type==101)
    {
        double distance;
        srand((unsigned)time(NULL));
        int i;
        i=_vertices.size()/2;
        for(int j=0;j<10;j++,i++){
            HE_vert * vert = _vertices.at(i);
            
            //随机数范围
            distance = rand() / double(RAND_MAX)*2;
            
            
            //vert->updatePosition(vert->getX() + distance * vert->getnX(), vert->getY() + distance * vert->getnY(), vert->getZ() + distance * vert->getnZ());
            //设置点位置
            vert->setDenpos(vert->getX() + distance * vert->getnX(), vert->getY() + distance * vert->getnY(), vert->getZ() + distance * vert->getnZ());}
        }
    
    else if(Noise_Type==102)
    {
        double distance;
        srand((unsigned)time(NULL));
        for (int i = 0; i < _vertices.size(); i++) {
            HE_vert * vert = _vertices.at(i);
           
            GLfloat separate=(aabb.xMax+aabb.xMin)*0.5;
            
            if(vert->getX()>separate)
            //随机数范围-0.005~0.005
            { distance = rand() / double(RAND_MAX);
                distance = (distance - 0.5) ;}
            else
                distance=0;
            
            //vert->updatePosition(vert->getX() + distance * vert->getnX(), vert->getY() + distance * vert->getnY(), vert->getZ() + distance * vert->getnZ());
            //设置点位置
                vert->setDenpos(vert->getX() + distance * vert->getnX(), vert->getY() + distance * vert->getnY(), vert->getZ() + distance * vert->getnZ());
      
        }
        }
 
    
    
}

