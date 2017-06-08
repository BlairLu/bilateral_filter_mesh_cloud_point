//
//  main.cpp
//  Bilateral Filtering of Meshes
//
//  Created by lu jinghan on 2017/5/15.
//  Copyright © 2017年 lu jinghan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "HE_edge.h"
#include "HE_vert.h"
#include "HE_face.h"
#include "mesh.h"
#include "Viewer.h"

int fNumb=0;
int vNumb=0;
using namespace std;

GLfloat minx=INT_MAX, miny=INT_MAX, minz=INT_MAX;
GLfloat maxx=INT_MIN, maxy=INT_MIN, maxz=INT_MIN;
Mesh* _mesh = new Mesh();

void parsingVertex(string line)
{
    GLfloat position[3];
    unsigned long a[4];
    //找到line这行每个字符前的那个空格，将空格所在位置的下标index分别赋值给数组a
    for (string::size_type index = 0, i = 0; i < 4; index ++) {
        if(isspace(line[index]))
        {
            //查看是否有多余空格，有的话更新空格下标，从而保证空格位置在非空格字符前一个；
            if (i>0 && (a[i - 1] == (index - 1))) {
                a[i-1] = index;
            }
            else
            {
                a[i] = index;
                i ++;
            }
        }
    }
    int j = 1;
    while (j <= 3) {
        if (j == 1) {
            string strx = line.substr(a[j], a[j+1]-a[j]+1);
            string s = strx.c_str();
            position[0] = atof(strx.c_str());
            if (position[0] < minx) {
                minx = position[0];
            }
            if (position[0] > maxx) {
                maxx = position[0];
            }
        }
        if (j == 2) {
            string stry = line.substr(a[j], a[j+1]-a[j]+1);
            position[1] = atof(stry.c_str());
            if (position[1] < miny) {
                miny = position[1];
            }
            if (position[1] > maxy) {
                maxy = position[1];
            }
        }
        if (j == 3) {
            string strz = line.substr(a[j], a[j+1]-a[j]+1);
            position[2] = atof(strz.c_str());
            if (position[2] < minz) {
                minz = position[2];
            }
            if (position[2] > maxz) {
                maxz = position[2];
            }
        }
        j++;
    }
    HE_vert* v = new HE_vert(position[0], position[1], position[2]);
    _mesh->addVertex(v);
}

void parsingFace(string line, int faceNum)
{
    unsigned long b[5];
    int vec[3];
    // get the space position
    for (string::size_type index = 0, i = 0; i < 4; index ++) {
        if(isspace(line[index]))
        {
            if ((b[i-1] == (index - 1)) && i>0) {
                b[i-1] = index;
            }
            else
            {
                b[i] = index;
                i ++;
            }
        }
    }
    b[4] = line.length();
    //get face's 3 vertexs
    for (int i = 1; i < 4; i ++) {
        string str = line.substr(b[i]+1, b[i+1]-b[i]-1);
        vec[i - 1] = atoi(str.c_str()); //represent the vertex number
    }
    //initialize vertex/ face/ edge from a LINE
    _mesh->addTriangle(vec);
}

void readMFile(string filename)
{
    ifstream in;
    // string filename;
    // filename="gargoyle.m";
    
    in.open(filename);
    
    if(!in)
        cerr << "open file wrongly!" << endl;
    
    string line;
    int faceNum = 0;
    while (!in.eof()) {
        getline(in, line);
        //parsing vertexs
        if (line[0] == 'V')
        {
            parsingVertex(line);
            vNumb++;
        }
        if (line[0] == 'F') {
            //parsing faces
            parsingFace(line, faceNum);
            faceNum++;
            fNumb++;
        }
    }
    //_mesh->bilateralFiltering();
    //_mesh->computeNormal();
    _mesh->scaleAndMove();
    _mesh->computeNormal();
    _mesh->computeCloudPointNormal();
   
   // _mesh->addNoise();
    /*_mesh->releaseTempList();*/
}

int main(int argc, char * argv[]) {
    string filename;
    
    printf ("\n Please enter a filename: ");
   printf ("\n Choose 'bunny.m' or 'cap.m' or 'eight.m' or 'knot.m' or 'gargoyle.m': ");
    cin >> filename;
    
    readMFile(filename);       
    //readMFile();
    
    Viewer* _viewer = new Viewer(argc, argv, _mesh,filename);
    _viewer->startEngine();
    _mesh->releaseTempList();
    
    return 0;
}
