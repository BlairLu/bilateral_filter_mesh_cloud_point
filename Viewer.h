
#ifndef Viewer_h
#define Viewer_h
#include <GLUI/GLUI.h>
#include <GLUT/GLUT.h>
#include "mesh.h"

enum DisplayType {VERTICES=0, EDGES, TEXTURE, SOLID, WIRE,
		  FRONTLINES, FEATURES, NOTHING};
enum ActionType {NONE, TRANSLATE, ROTATE, ZOOM};

// Colours
enum colors {BLACK=0, BLUE, GREEN, RED, YELLOW, PURPLE, CYAN, WHITE};

const GLfloat ambientLightSource[] = {0.0, 0.0, 0.0, 1.0};

class Viewer {
    GLUI_Checkbox    *checkbox;
	GLUI_Panel       *panel_option, *panel_rendering,*panel_denoise;
    GLUI_Panel       *obj_panel2, *obj_panel3;
    int obj_mode,noise_mode;
    int press_x, press_y;
    float x_angle;
    float y_angle;
    int xform_mode;
    float scale_size;
    
    // Viewpoint data (Trackball, Translation, Zoom, Clipping)
    GLfloat tbTransform[4][4];
    GLfloat xShift, yShift, zShift;
    GLfloat clipping;
    
    // Viewmode
    enum DisplayType meshDisplayMode, featuresDisplayMode;
    
    // Light intensity
    GLfloat lightBrightness;
    
    // Flags
    bool displayNormals, displayShapeColors, displayFeatures, displayPolygons, displayBoundingBox;
    bool enableTextureFilter, enableCutBackFaces, enableLighting, enableAspectRatio;
    
public:
    Viewer();
    Viewer(int argc, char** argv, Mesh* _m,string fname);
    void createMenu();
    void startEngine();
    
};

#endif /* Viewer_h */
