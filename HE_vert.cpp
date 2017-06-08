
#include "HE_vert.h"
#include <vector>
#include <iostream>
#include <math.h>
HE_vert::HE_vert()
{
    position = Vector3f();
    normal = Vector3f();
	denpos = Vector3f();
    Order=0;
    edge = NULL;
    
    std::vector<list*> tempList;
}

HE_vert::HE_vert(GLfloat xx, GLfloat yy, GLfloat zz)
{
    position.setX(xx);
    position.setY(yy);
    position.setZ(zz);
    edge = NULL;
    normal = Vector3f();
	denpos = Vector3f();
    std::vector<list*> tempList;
}

HE_vert::~HE_vert()
{
    delete edge;
}

void HE_vert::addEdge(HE_edge *e)
{
    edge = e;
}

void HE_vert::setNormal(GLfloat nnx, GLfloat nny, GLfloat nnz)
{
    normal.setX(nnx);
    normal.setY(nny);
    normal.setZ(nnz);
}

void HE_vert::setDenpos(GLfloat ddx, GLfloat ddy, GLfloat ddz)
{
	denpos.setX(ddx);
	denpos.setY(ddy);
	denpos.setZ(ddz);
}
void HE_vert::setPosition(GLfloat ppx, GLfloat ppy, GLfloat ppz)
{
    position.setX(ppx);
    position.setY(ppy);
    position.setZ(ppz);
}

void HE_vert::addTempList(HE_edge* edge, HE_vert* vert)
{
    list* ll = new list();
    ll->e = edge;
    ll->endvert = vert;
    tempList.push_back(ll);
}
void HE_vert::SetOrder(int tar)
{
    Order=tar;
}

int HE_vert::getOrder()
{
    int res;
    res=Order;
    return res;
    
}

HE_edge* HE_vert::searchTempList(HE_vert *vert)
{
    list* ll = new list();
    HE_edge* pairedge = NULL;
    for(int i = 0; i < tempList.size(); i ++)
    {
        ll = tempList.at(i);
        if(ll->endvert == vert)
        {
            pairedge = ll->e;
            break;
        }
    }
    return pairedge;
}

//get the neighborhood of the vertex with tempList
vector<HE_vert*> HE_vert::neighborhood()
{
	vector<HE_vert*> neighbors;
	for (int i = 0; i < tempList.size(); i++) 
	{
		neighbors.push_back(tempList.at(i)->endvert);
		if (tempList.at(i)->e->getPrev()->getPair() == NULL)
			neighbors.push_back(tempList.at(i)->e->getPrev()->getVert());
	}
    
	return neighbors;
}

void HE_vert::cloudpointSetneigh(vector<HE_vert*> fulllist,HE_vert* Vertex,Vector3f maxi,Vector3f mini)
{
    
    
    vector<HE_vert*> tempneigh;
    GLfloat targetX=Vertex->getX();
    GLfloat targetY=Vertex->getY();
    GLfloat targetZ=Vertex->getZ();
    GLfloat volume=(maxi.getX()-mini.getX())*(maxi.getY()-mini.getY())*(maxi.getZ()-mini.getZ());
    GLfloat radius=volume*2.5/fulllist.size();
  int number=0;
   
do{
    vector<HE_vert*>().swap(tempneigh);
    for (int i = 0; i < fulllist.size(); i ++)
    {
        
        HE_vert* vert;
        
        vert=fulllist[i];
        GLfloat testX= vert->getX();
        GLfloat testY= vert->getY();
        GLfloat testZ= vert->getZ();
        GLfloat dert=0;
        dert=(targetX-testX)*(targetX-testX)+(targetY-testY)*(targetY-testY)+(targetZ-testZ)*(targetZ-testZ)-radius*radius;
        
       if(dert<0&&dert!=(-1)*radius*radius)
       {
           
           tempneigh.push_back(fulllist[i]);
           
       }
        
    }
    number=tempneigh.size();
    if(number>10)
        radius=0.8*radius;
    else if(number<6)
        radius=1.2*radius;
    }while(number>10||number<6);
    cpneighs=tempneigh;
    
    
}

vector<HE_vert*> HE_vert::cloudpointGetneigh()
{
    return cpneighs;
}



bool HE_vert::computeVertNormal()
{
    Vector3f* outcome = new Vector3f();
    HE_edge* outgoing = edge;
    HE_edge* curr = outgoing;
    HE_edge* curr_pair = new HE_edge();
    if(curr->getPair() != NULL &&curr->getPrev()->getPair()!= NULL)
    {
        outcome->add(curr->crossProduct(curr->getPrev()->getPair()));
        curr_pair = curr->getPair();
    }
    else
    {
        return false;
    }
       while (curr_pair->getNext() != outgoing) {
        outcome->add(curr->crossProduct(curr_pair->getNext()));
        curr = curr_pair->getNext();
        if (curr->getPair() != NULL) {
            curr_pair = curr->getPair();
        }
        else
        {
            return false;
        }
    }
    //normalize
    outcome->normalize();
    normal.setX(outcome->getX());
    normal.setY(outcome->getY());
    normal.setZ(outcome->getZ());
    return true;
}

bool HE_vert::computeCPVertNormal(vector<HE_vert*> fullvert,HE_vert *target,Vector3f boundmax,Vector3f boundmin)
{
  Vector3f* outcome = new Vector3f();
    GLfloat a=0,b=0,c=0;
    target->cloudpointSetneigh(fullvert, target,boundmax,boundmin);
    vector<HE_vert*> currNeighbors=target->cloudpointGetneigh();
    GLfloat A11=0,A12=0,A13=0,A21=0,A22=0,A23=0,A31=0,A32=0,A33=0;
    for(int order=0;order<currNeighbors.size();order++)
    {
        HE_vert* Neighbours=currNeighbors[order];
        
        A11+=pow(Neighbours->getX()-target->getX(),2);
        A12+=(Neighbours->getX()-target->getX())*(Neighbours->getY()-target->getY());
        A13+=(Neighbours->getX()-target->getX())*(Neighbours->getZ()-target->getZ());
        A21+=A12;
        A22+=pow(Neighbours->getY()-target->getY(),2);
        A23+=(Neighbours->getY()-target->getY())*(Neighbours->getZ()-target->getZ());
        A31+=A13;
        A32+=A23;
        A33+=pow(Neighbours->getZ()-target->getZ(),2);
    }
    if(A11>A22&&A11>A33)
    {
        a=1;
        GLfloat test1=(A21*A33-A23*A31);
        GLfloat test2=(A22*A33-A23*A32);
        GLfloat test3=(A22*A31-A21*A32);
        b=(-1)*((A21*A33-A23*A31)/(A22*A33-A23*A32));
        c=(-1)*((A22*A31-A21*A32)/(A22*A33-A23*A32));
        
    }
    else if (A22>A11&&A22>A33)
    {
        b=1;
        a=(-1)*((A12*A23-A22*A13)/(A11*A23-A13*A21));
        c=(-1)*((A11*A22-A12*A21)/(A11*A23-A13*A21));
        
    }
    else if(A33>A22&&A33>A11)
    {
        c=1;
        a=(-1)*((A13*A22-A12*A23)/(A11*A22-A12*A21));
        b=(-1)*((A11*A23-A21*A13)/(A11*A22-A12*A21));
    }
    a=a/sqrt(a*a+b*b+c*c);
    b=b/sqrt(a*a+b*b+c*c);
    c=c/sqrt(a*a+b*b+c*c);
    
    cpnormal.setX(a);
    cpnormal.setY(b);
    cpnormal.setX(c);
   
    return true;
}

GLfloat HE_vert::getX()
{
    return position.getX();
}

GLfloat HE_vert::getY()
{
    return  position.getY();
}

GLfloat HE_vert::getZ()
{
    return position.getZ();
}



GLfloat HE_vert::getnX()
{
    return normal.getX();
}

GLfloat HE_vert::getnY()
{
    return normal.getY();
}

GLfloat HE_vert::getnZ()
{
    return normal.getZ();
}



GLfloat HE_vert::getcpnX()
{
    return cpnormal.getX();
}

GLfloat HE_vert::getcpnY()
{
    return cpnormal.getY();
}

GLfloat HE_vert::getcpnZ()
{
    return cpnormal.getZ();
}


GLfloat HE_vert::getdX()
{
	return denpos.getX();
}

GLfloat HE_vert::getdY()
{
	return denpos.getY();
}

GLfloat HE_vert::getdZ()
{
	return denpos.getZ();
}

Vector3f* HE_vert::getPosition()
{
    return &position;
}

GLvoid HE_vert::freeTempList()
{
    tempList.clear();
}

HE_edge* HE_vert::getEdge()
{
    return edge;
}
