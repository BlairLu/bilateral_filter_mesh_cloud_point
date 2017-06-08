
//

#include <iostream>
#include <GLUT/GLUT.h>
#include <GLUI/GLUI.h>
#include "Viewer.h"
#include <fstream>
#include <istream>

#define OBJ_SOLID		0
#define OBJ_POINTS		1
#define OBJ_FLAT		2
#define OBJ_WIREFRAME   3

#define DEN_ORIGINAL    0
#define DEN_CLOUDPOINT	1
#define DEN_ADDNOISE	2

#define READYTOBF 0
#define Mesh_BF 1
#define CP_BF 2


#define NOI_NON 99
#define NOI_RAN 100
#define NOI_EXT 101
#define NOI_HAL 102

#define ORTHOGRAPHIC  1
#define PERSPECTIVE   0

#define DRAW_BOX  0
#define DRAW_NORMAL  0

#define TRANSFORM_NONE    11
#define TRANSFORM_ROTATE  12
#define TRANSFORM_SCALE    13
#define TRANSFORM_TRANSLATE 14

#define LIGHT0_ENABLED_ID    200
#define LIGHT1_ENABLED_ID    201
#define LIGHT0_INTENSITY_ID  250
#define LIGHT1_INTENSITY_ID  251

#define SIGMA_ENABLED_ID 252
#define SIGMA_sigmac_ID  300
#define SIGMA_SIGMAS_ID  301
#define ITERATION_ID     302

#define NOISE_ID 400
int control;
/********** Miscellaneous global variables **********/
//int   light0_enabled = 1;
//int   light1_enabled = 0;
//float light0_intensity = 1.0;
//float light1_intensity = 1.0;
int sigma_enabled = 0;
int win;
int drawbox = 0;
int drawNormal = 0;
int writeoff=0;

int sigmac_val = 1;
int sigmas_val = 1;
int iteration = 1;

int NOISE_VAL=NOI_NON;

string fileName;

GLUI_Spinner *spinner, *light0_spinner, *light1_spinner, *scale_spinner, *iteration_spinner;
GLUI_Listbox *Noise_LIST;
GLUI *glui;
GLUI_RadioGroup *g1, *g2;
GLUI_RadioGroup *radio_option, *radio_rendering,*radio_denoise;

GLUI_Spinner *sigmac_spinner, *sigmas_spinner;

GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light0_diffuse[] =  {1.0f, .2f, 0.2f, 1.0f};
GLfloat light0_position[] = {.5f, .5f, 1.0f, 0.0f};

GLfloat light1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light1_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
GLfloat light1_position[] = {-1.0f, -1.0f, 1.0f, 0.0f};

Mesh* _mm = new Mesh();
static int press_x, press_y;
static GLfloat x_angle = 0.0;
static GLfloat y_angle = 0.0;
static GLfloat scale_size = 1;
GLfloat xShift;
GLfloat yShift;
int model_mode = DEN_ORIGINAL;
int denoise_mode=READYTOBF;
int obj_mode = 0;
int proj_mode = 0;
static int xform_mode = 0;

int iterations = 1;

void drawPlane()
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glLineWidth(1.0);
    for (int i = 0; i < 21 ; i++) {
        if (i == 10) {
            glColor3f(0, 0, 0);
        } else {
            glColor3f(1.0, 1.0, 1.0);
        }
        glVertex3f(-100+i*10, 0, -100);
        glVertex3f(-100+i*10, 0, 100);
    }
    for (int j = 0; j < 21; j++) {
        if (j == 10) {
            glColor3f(0, 0, 0);
        } else {
            glColor3f(1.0, 1.0, 1.0);
        }
        glVertex3f(-100, 0, -100+j*10);
        glVertex3f(100, 0, -100+j*10);
    }
    glEnd();
    
    glEnable(GL_LIGHTING);
    
}

void draw_z_axis()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPushMatrix();
    glTranslatef(0, 0, 30);
    glColor3f(0, 0, 1.0);
    glutSolidCone(0.6, 2, 20.0, 20);
    glPopMatrix();
    glPushMatrix();
    GLUquadricObj* a = gluNewQuadric();
    gluCylinder(a, 0.3, 0.3, 30, 20, 20);
    glPopMatrix();
}

void draw_x_axis()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    //draw_z_axis();
    glTranslatef(0, 0, 30);
    glColor3f(1.0, 0, 0);
    glutSolidCone(0.6, 2, 20.0, 20);
    glPopMatrix();
    
    glPushMatrix();
    GLUquadricObj* a = gluNewQuadric();
    glRotatef(90, 0, 1, 0);
    gluCylinder(a, 0.3, 0.3, 30.0, 20, 20);
    glPopMatrix();
}

void draw_y_axis()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glRotatef(90, -1, 0, 0);
    
    glTranslatef(0, 0, 30);
    glColor3f(0, 1.0, 0);
    glutSolidCone(0.6, 2.0, 20.0, 20);
    glPopMatrix();
    
    glPushMatrix();
    GLUquadricObj* a = gluNewQuadric();
    glRotatef(90, -1, 0, 0);
    gluCylinder(a, 0.3, 0.3, 30.0, 20, 20);
    glPopMatrix();
}

void drawAxis()
{
    glDisable(GL_LIGHTING);
    draw_z_axis();
    draw_x_axis();
    draw_y_axis();
    glEnable(GL_LIGHTING);
}



int writeOFF(string fn)
{
    string filename_off=fn;
    int lengthnew=filename_off.length();
    string newname=".off";
    
    filename_off.assign(fn,0,lengthnew-2);
    filename_off+=newname;
    FILE *fp = NULL;
    if( (fp = fopen(filename_off.c_str(), "w")) == NULL)
        return 0;
    std::cout << "Printing denoised file... "<<filename_off <<std::endl;
    ofstream fout(filename_off);
    int vertnum,facenum,edgenum;
    vertnum=_mm->vertCount();
    facenum=_mm->faceCount();
    edgenum=_mm->edgeCount();
    vector<Vector3f> vertice1=_mm->getVert();
   
    vector<vector<int>> face1=_mm->getFacet();
    
    
    if(fout){
        fout<<"OFF"<<std::endl;
        fout<<vertnum<<" "<<facenum<<" "<<edgenum<<std::endl;
        
        for(int i=0;i<vertnum;++i) {
           
            fout<<vertice1[i].getX()<<" "<<vertice1[i].getY()<<" "<<vertice1[i].getZ()<<std::endl;
        }
        for(int i=0;i<face1.size();++i)
        {
            fout<<"3 "<<face1[i][0]<<" "<<face1[i][1]<<" "<<face1[i][2]<<std::endl;
        }
    }
    std::cout <<"printing finish"<< std::endl;
    
    return 1;
}
void Setfn(string l)
{
    fileName=l;
}

Viewer::Viewer(int argc, char** argv, Mesh* _m,string fName)
{
    // normal initialisation
    glutInit(&argc, argv);
    // use double buffer to get better results on animation
    // use depth buffer for hidden surface removal
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(50,50);
    
    win = glutCreateWindow("Mesh Viewer     @Lu Jinghan");
    createMenu();
    Setfn(fName);
    _mm = _m;
}

static void disp(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // setup the perspective projection
    proj_mode = g2->get_int_val();
    if (proj_mode == ORTHOGRAPHIC) {
        glOrtho(-30, 30, -30, 30, -30, 300);
        gluLookAt(50,50,50,0,0,0,0,1,0);
        
    } else {
        gluPerspective(60, 1, .1, 500);
        gluLookAt(30,30,30,0,0,0,0,1,0);
        //set camera
    }
    drawAxis();
    drawPlane();
    if (drawbox) {
        _mm->drawAABB(x_angle, y_angle, xShift, yShift, scale_size);
    }
    if(drawNormal){
        _mm->drawNormal(x_angle, y_angle, xShift, yShift, scale_size);
    }
    
    model_mode = radio_option->get_int_val();
    switch (model_mode)
    {
        case DEN_ORIGINAL:
            sigmac_spinner->disable();
            sigmas_spinner->disable();
            iteration_spinner->disable();
            Noise_LIST->disable();
            _mm->setDenoised(true);
            _mm->addNoise(NOI_NON);
            radio_rendering->enable();
            radio_denoise->disable();
            break;
            
        case DEN_CLOUDPOINT:
            sigmac_spinner->disable();
            sigmas_spinner->disable();
            iteration_spinner->disable();
            radio_denoise->disable();
            Noise_LIST->disable();
            _mm->setDenoised(true);
           // _mm->addNoise();
            radio_rendering->set_selected(OBJ_POINTS);
            radio_rendering->disable();
            break;
            
        case DEN_ADDNOISE:
             Noise_LIST->enable();
            radio_denoise->enable();
            sigmac_spinner->enable();
            sigmas_spinner->enable();
            iteration_spinner->enable();
            _mm->setDenoised(true);
            _mm->addNoise(NOISE_VAL);
            break;
            
      /*  case DEN_DENOISED:
            sigmac_spinner->enable();
            sigmas_spinner->enable();
            iteration_spinner->enable();
            _mm->setDenoised(true);  
            radio_rendering->enable();
            _mm->bilateralFiltering(sigmac_val, sigmas_val,iteration);
             break;
            
        case DEN_CPDENOISED:
            sigmac_spinner->enable();
            sigmas_spinner->enable();
            iteration_spinner->enable();
            _mm->setDenoised(true);
            _mm->cpbilateralFiltering(sigmac_val, sigmas_val,iteration);
            break;
           */
        default:
            break;
    }
    denoise_mode = radio_denoise->get_int_val();
    switch (denoise_mode) {
        case READYTOBF:
            break;
            
        case Mesh_BF:
            sigmac_spinner->enable();
            sigmas_spinner->enable();
            iteration_spinner->enable();
            _mm->bilateralFiltering(sigmac_val, sigmas_val,iteration);
            _mm->setDenoised(false);
            break;
            
        case CP_BF:
            sigmac_spinner->enable();
            sigmas_spinner->enable();
            iteration_spinner->enable();
            _mm->cpbilateralFiltering(sigmac_val, sigmas_val,iteration);
             _mm->setDenoised(false);
            break;
            
        default:
            break;
    }
    
    
    if(model_mode==DEN_CLOUDPOINT)
        obj_mode=OBJ_POINTS;
    else
        obj_mode = radio_rendering->get_int_val();
    
    
    switch (obj_mode) {
        case OBJ_POINTS:
            _mm->renderPoints(x_angle, y_angle, xShift, yShift, scale_size);
            break;
            
        case OBJ_WIREFRAME:
            _mm->renderWireframe(x_angle, y_angle, xShift, yShift, scale_size);
            break;
        case OBJ_FLAT:
            //sigmac_spinner->disable();
            //sigmas_spinner->disable();
            //light1_spinner->disable();
            _mm->renderFlat(x_angle, y_angle, xShift, yShift, scale_size);
            break;
        case OBJ_SOLID:
            _mm->renderSolid(x_angle, y_angle, xShift, yShift, scale_size);
            break;
        default:
            break;
    }
    
    // swap the buffers
    glutSwapBuffers();
}

static void mymouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        press_x = x; press_y = y;
        if (button == GLUT_LEFT_BUTTON)
            xform_mode = TRANSFORM_ROTATE;
        else if (button == GLUT_RIGHT_BUTTON)
            xform_mode = TRANSFORM_SCALE;
        else if (button == GLUT_MIDDLE_BUTTON)
            xform_mode = TRANSFORM_TRANSLATE;
    }
    else if (state == GLUT_UP)
    {
        xform_mode = TRANSFORM_NONE;
    }
}

static void mymotion(int x, int y)
{
    if (xform_mode == TRANSFORM_ROTATE)
    {
        x_angle += (x - press_x)/5.0;
        
        if (x_angle > 180)
            x_angle -= 360;
        else if (x_angle <-180)
            x_angle += 360;
        
        press_x = x;
        
        y_angle += (y - press_y)/5.0;
        
        if (y_angle > 180)
            y_angle -= 360;
        else if (y_angle <-180)
            y_angle += 360;
        
        press_y = y;
    }
    else if (xform_mode == TRANSFORM_SCALE)
    {
        GLfloat old_size = scale_size;
        
        scale_size *= (1 + (y - press_y)/60.0);
        
        if (scale_size <0)
            scale_size = old_size;
        press_y = y;
    }
    else if (xform_mode == TRANSFORM_TRANSLATE)
    {
        xShift += (x-press_x)/5;
        yShift -= (y-press_y)/5;
        press_x=x;
        press_y=y;
    }
    
    // force the redraw function
    glutPostRedisplay();
}

void Viewer::startEngine()
{
    //register callback handlers
    glutDisplayFunc(disp);
    glutMouseFunc(mymouse);
    glutMotionFunc(mymotion);
    GLUI_Master.set_glutMouseFunc(mymouse);
    glui->set_main_gfx_window (win);
    
    // enter main loop
    glutMainLoop();
}

/* GLUI control callback */
void output()
{
    writeoff=1;
    writeOFF(fileName);
}
void clear()
{
    model_mode=DEN_ORIGINAL;
    NOISE_VAL=NOI_NON;
    obj_mode=OBJ_SOLID;
    disp();
    
}
void control_cb( int control )
{
    
    /*if ( control == LIGHT0_ENABLED_ID ) {
     if ( light0_enabled ) {
     glEnable( GL_LIGHT0 );
     light0_spinner->enable();
     }
     else {
     glDisable( GL_LIGHT0 );
     light0_spinner->disable();
     }
     }
     else if ( control == LIGHT1_ENABLED_ID ) {
     if ( light1_enabled ) {
     glEnable( GL_LIGHT1 );
     light1_spinner->enable();
     }
     else {
     glDisable( GL_LIGHT1 );
     light1_spinner->disable();
     }
     }
     else if ( control == LIGHT0_INTENSITY_ID ) {
     float v[] = { light0_diffuse[0],  light0_diffuse[1],
     light0_diffuse[2],  light0_diffuse[3] };
     
     v[0] *= light0_intensity;
     v[1] *= light0_intensity;
     v[2] *= light0_intensity;
     
     glLightfv(GL_LIGHT0, GL_DIFFUSE, v );
     }
     else if ( control == LIGHT1_INTENSITY_ID ) {
     float v[] = { light1_diffuse[0],  light1_diffuse[1],
     light1_diffuse[2],  light1_diffuse[3] };
     
     v[0] *= light1_intensity;
     v[1] *= light1_intensity;
     v[2] *= light1_intensity;
     
     glLightfv(GL_LIGHT1, GL_DIFFUSE, v );
     }
     else if (control == SIGMA_ENABLED_ID) {
     if (sigma_enabled) {
     sigmac_spinner->enable();
     sigmas_spinner->enable();
     }
     else {
     sigmac_spinner->disable();
     sigmas_spinner->disable();
     }
     }*/
}

void Viewer::createMenu()
{
    glEnable(GL_LIGHTING);
    glEnable( GL_NORMALIZE );
    
    glEnable(GL_LIGHT0);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glEnable(GL_DEPTH_TEST);
    glClearColor (0.8, 0.8, 0.8, 0.0);   // set display-window color to white
    
    
    glui = GLUI_Master.create_glui( "Mesh Viewer", 0, 550, 50 ); /* name, flags, x, and y */
    
    panel_option = new GLUI_Panel(glui, "Option");
    radio_option = glui->add_radiogroup_to_panel(panel_option, &model_mode, 3, control_cb);
    glui->add_radiobutton_to_group(radio_option, "Mesh");
    glui->add_radiobutton_to_group(radio_option, "Point Cloud");
    glui->add_radiobutton_to_group(radio_option, "Add Noise");
  //  glui->add_radiobutton_to_group(radio_option, "Cloud Point Denoising");
    
    panel_denoise = new GLUI_Panel(glui, "Denoising" );
    radio_denoise = glui->add_radiogroup_to_panel(panel_denoise, &denoise_mode, 3, control_cb);
    glui->add_radiobutton_to_group(radio_denoise, "Deoise");
    glui->add_radiobutton_to_group(radio_denoise, "Mesh Deoise");
    glui->add_radiobutton_to_group(radio_denoise, "Point Cloud Denoise");
  
   
    
    panel_rendering = new GLUI_Panel(glui, "Rendering" );
    radio_rendering = glui->add_radiogroup_to_panel(panel_rendering, &obj_mode, 4, control_cb);
    glui->add_radiobutton_to_group(radio_rendering, "Smooth shading");
    glui->add_radiobutton_to_group(radio_rendering, "Points");
    glui->add_radiobutton_to_group(radio_rendering, "Flat shading");
    glui->add_radiobutton_to_group(radio_rendering, "Wireframe");
    
   
    Noise_LIST=
    new GLUI_Listbox(panel_option,"",&NOISE_VAL,NOISE_ID,control_cb);
    
    Noise_LIST->add_item( NOI_NON, "No Noise");
    Noise_LIST->add_item( NOI_RAN, "Random Noise");
    Noise_LIST->add_item( NOI_EXT, "Extreme Noise");
    Noise_LIST->add_item( NOI_HAL, "Half Noise");
    
    sigmac_spinner =
    new GLUI_Spinner(panel_denoise, "sigmac:",
                     &sigmac_val, SIGMA_sigmac_ID,
                     control_cb);
    sigmac_spinner->set_float_limits(0, 16);
    sigmac_spinner->disable();
    
    sigmas_spinner =
    new GLUI_Spinner(panel_denoise, "sigmas:",
                     &sigmas_val, SIGMA_SIGMAS_ID,
                     control_cb);
    sigmas_spinner->set_float_limits(0, 16);
    sigmas_spinner->disable();
    
    iteration_spinner =
    new GLUI_Spinner(panel_denoise, "iteration:",
                     &iteration, ITERATION_ID,
                     control_cb);
    iteration_spinner->set_int_limits(1,5);
    iteration_spinner->disable();
    
    obj_panel2 = new GLUI_Panel(glui, "Projection" );
    g2 =
    glui->add_radiogroup_to_panel(obj_panel2, &proj_mode, 3, control_cb);
    glui->add_radiobutton_to_group(g2, "Perspective");
    glui->add_radiobutton_to_group(g2, "Orthographic");
    
    
    new GLUI_Button(glui, "Clear", 0,(GLUI_Update_CB)clear);
      new GLUI_Button(glui, "Output .off", 0,(GLUI_Update_CB)output);
    new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);
    
    
    //----------------------------------------------------------------
    /*obj_panel3 = new GLUI_Panel(glui, "Options");
     new GLUI_Checkbox( obj_panel3, "BoundBox", &drawbox,
     DRAW_BOX, control_cb );
     new GLUI_Checkbox( obj_panel3, "DrawNormal", &drawNormal,
     DRAW_NORMAL, control_cb );*/
    
    //******* Add some controls for lights ********
    
    /*GLUI_Panel *light0 = new GLUI_Panel( glui, "Light 1" );
     GLUI_Panel *light1 = new GLUI_Panel( glui, "Light 2" );
     
     new GLUI_Checkbox( light0, "Enabled", &light0_enabled,
     LIGHT0_ENABLED_ID, control_cb );
     light0_spinner =
     new GLUI_Spinner( light0, "Intensity:",
     &light0_intensity, LIGHT0_INTENSITY_ID,
     control_cb );
     light0_spinner->set_float_limits( 0.0, 1.0 );
     
     new GLUI_Checkbox( light1, "Enabled", &light1_enabled,
     LIGHT1_ENABLED_ID, control_cb );
     light1_spinner =
     new GLUI_Spinner( light1, "Intensity:",
     &light1_intensity, LIGHT1_INTENSITY_ID,
     control_cb );
     light1_spinner->set_float_limits( 0.0, 1.0 );
     light1_spinner->disable();*/   /* Disable this light initially */
    
    
    
}

