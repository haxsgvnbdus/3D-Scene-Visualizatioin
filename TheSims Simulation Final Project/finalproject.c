/*
 *  Final Project - Han Ngo
 *
 *
 * Key bindings:
 * 0          Reset view angle
 * a          Turn left in first person
 * w          Zoom in, in first person view
 * s          Zoom out, in first person view
 * d          Turn right in first person view
 * arrows     Change view angle (perspective/orthogonal)
 * m or M     Toggle perspective and orthogonal modes
 * f or F     View scene in first person perspective
 * +/-        Change field of view (FOV) angle (perspective)
 * PgDn/PgUp  Zoom in and out
 *  +/-       Change field of view angle
 * ' '        Toggle shadows
 * ESC        Exit

 */

#include "CSCIx229.h"
typedef struct {float x,y,z;} Point;

//  Global variables
int mode=1;    // Display mode
int fp=0;
int obj=15;    // Display objects (bitmap)
int move=0;    // Light movement
int axes=0;    // Display axes
int n=8;       // Number of slices
int th=-30;    // Azimuth of view angle
int ph=+30;    // Elevation of view angle
int viewth=345;    //  Azimuth of view angle
int viewph=40;    //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int tex2d[16];    // Textures (names)
 

// Lighting and texture variables
int          dt=50;     // Timer period (ms)
double       asp=1;     // Aspect ratio
double       dim=25;     // Size of world
int          zh=0;      // Light azimuth
float        Ylight=25;  // Elevation of light
float        Lpos[4];   // Light position
float        Lpos2[4];
unsigned int framebuf=0;// Frame buffer id
double       Svec[4];   // Texture planes S
double       Tvec[4];   // Texture planes T
double       Rvec[4];   // Texture planes R
double       Qvec[4];   // Texture planes Q
int          Width;     // Window width
int          Height;    // Window height
int          shadowdim; // Size of shadow map textures
int          shader;    // Shader
int shadows=1;
double y=0;


// Eye coordinates
double Ex = 13;
double Ey = 1.5;
double Ez = 24;
int rot = -15; //rotation var for first person
 
 
// Camera looking coordinates
double Cx = 0;
double Cy = 0;
double Cz = 0;


char* text[]={"Shadows","Shadow Map"};


/*
 *  Set projection
 */
static void myProject()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if(fp) {
      gluPerspective(fov,asp,.01,4*dim);
   }
   else {
      if (mode)
         gluPerspective(fov,asp,dim/4,4*dim);
      //  Orthogonal projection
      else
         glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   }
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void drawCylinder(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int handle,int size, int seat, int rott, int blackk){
     float r = 0.0;
     float b = 0.0;
     float g = 0.0;
     double radius = 0.25, height = 1;

     glPushMatrix();

     //  Transform cylinder
     glTranslated(x,y,z);
     if (handle==1){
         glRotated(th,0,0,1);
         glRotated(th,1,0,0);}
     else{
          glRotated(th,0,0,1);
     }

     glScaled(dx,dy,dz);

     if(seat==1){
     //  Enable textures
     glEnable(GL_TEXTURE_2D);
     glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
     glBindTexture(GL_TEXTURE_2D,tex2d[1]);
     }
     
     else{
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glBindTexture(GL_TEXTURE_2D,tex2d[4]);
     }


    if (blackk==1){ //for top metal surface of storages only
      r = b = g = 0.1;
    }
    

  glColor3f(r,b,g);
  glNormal3d(0,1,0);

  /* Top of Cylinder */
  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.5,0.5);
  glVertex3d(0.0, height, 0.0);

  double i;

  for(i = 0.0; i <= size; i+=10) {
      glTexCoord2f(0.5*Cos(i)+0.6, -0.5*Sin(i)+0.5);
      if(rott!=0){
      glVertex3d(radius * Cos(i-40), height, radius * Sin(i-40));}
      else{glVertex3d(radius * Cos(i), height, radius * Sin(i));}
  }
  glEnd();


  if(seat==1){ //top texture on for island seats
      glDisable(GL_TEXTURE_2D);
      glColor3f(r,b,g);
      glNormal3d(0,-1,0);
    

      /* Bottom of Cylinder */
      glBegin(GL_TRIANGLE_FAN);
      glVertex3d(0.0, -height, 0.0);
      for(i = 0.0; i <= size; i+=10) {
          glVertex3d(radius * Cos(i), -height, radius * Sin(i));
      }
      glEnd();
      
      glBegin(GL_QUAD_STRIP);
      double j;
      for (j = 0; j <= size; j+=.125) {
          double x = radius * Cos(j);
          double y = height;
          double z = radius * Sin(j);
          glColor3f(r,b,g);//transparency only applies to seats not dinner table chairs
          glNormal3d(Cos(j), 0, Sin(j));
          glVertex3d(x, y, z);
      }
      glEnd();
  }

  else{

    glColor3f(r,b,g);
    glNormal3d(0,-1,0);

    /* Bottom of Cylinder */
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5,0.5);
    glVertex3d(0.0, -height, 0.0);
    for(i = 0.0; i <= size; i+=10) {
        glTexCoord2f(0.5*Cos(i)+0.5, 0.5*Sin(i)+0.5);
        glVertex3d(radius * Cos(i), -height, radius * Sin(i));
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    double j;
    for (j = 0; j <= size; j+=.125) {
        const float tc = (j / (float) 360);
        double x = radius * Cos(j);
        double y = height;
        double z = radius * Sin(j);
        glColor3f(r,b,g);
        glNormal3d(Cos(j), 0, Sin(j));
        glTexCoord2f(-tc, 0.0); glVertex3d(x, -y, z);
        glTexCoord2f(-tc, 1.0); glVertex3d(x, y, z);
    }
    glEnd();
  }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

 

static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int tex, double r, double g, double b, double a)
{

  double tex_x=1;
  double tex_y=1;

 
   //  Enable textures
   if (tex!=0 && tex!= 100 && tex!=5 && tex!=8 && tex!=44 && tex !=1000 && tex!= 10 && tex!= 1 && tex!= 6
              && tex!= 11 && tex!= 12 && tex != 13 && tex!= 19 && tex!= 15){  //texture on
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
       glColor4f(r,g,b,a);
       tex_x=1;
       tex_y=1;
    }
   else if(tex==5){
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
       tex_x=2;
       tex_y=1;
   }
   else if(tex==19){
       tex = 5;
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //adjust floor mat type 2
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
       tex_x=1;
       tex_y=1;
   }

   else if(tex==11){
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
   }

   else if(tex==12){
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
   }

   else if(tex==13){
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
   }
   else if(tex==15){
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
       tex_x = 1;
       tex_y = 1;
   }

   else if(tex==100){
       tex = 0;
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
       tex_x=20;
       tex_y=20;

   }

   else if(tex==1){
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
       tex_x=2;
       tex_y=2;
   }

   else if(tex==100){
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
   }
   else if(tex==8){
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
       tex_x=5;
       tex_y=5;
   }
   else if (tex==10){
       glEnable(GL_TEXTURE_2D);
       glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
       glColor3f(1,1,1);
       glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
       
   }
   else{
        glDisable(GL_TEXTURE_2D);
   }



   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //  Top
   glBegin(GL_QUADS);
   glColor4f(r,g,b,1);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(tex_x,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(tex_x,tex_y); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,tex_y); glVertex3f(-1,+1,-1);
   glEnd();

   if(tex==5){ //just for the mat
       glDisable(GL_TEXTURE_2D);
       r=0.1;
       b=0.1;
       g=0.1;
    }
       

   //  Front
   glBegin(GL_QUADS);
   glColor4f(r,g,b,a);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(tex_x,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(tex_x,tex_y); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,tex_y); glVertex3f(-1,+1, 1);
   glEnd();


    //  Right
    glBegin(GL_QUADS);
    glColor4f(r,g,b,a);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,tex_y);glVertex3f(+1,-1,+1);
    glTexCoord2f(0,0);glVertex3f(+1,-1,-1);
    glTexCoord2f(tex_x,0);glVertex3f(+1,+1,-1);
    glTexCoord2f(tex_x,tex_y);glVertex3f(+1,+1,+1);
    glEnd();
 
    //  Left
    glBegin(GL_QUADS);
    glColor4f(r,g,b,a);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(0,tex_y); glVertex3f(-1,-1,+1);
    glTexCoord2f(tex_x,tex_y); glVertex3f(-1,+1,+1);
    glTexCoord2f(tex_x,0); glVertex3f(-1,+1,-1);
    glEnd();

     //  Bottom
     glBegin(GL_QUADS);
     glColor4f(r,g,b,a);
     glNormal3f( 0,-1, 0);
     glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
     glTexCoord2f(tex_x,0); glVertex3f(+1,-1,-1);
     glTexCoord2f(tex_x,tex_y); glVertex3f(+1,-1,+1);
     glTexCoord2f(0,tex_y); glVertex3f(-1,-1,+1);
     glEnd();

     //  Back
     glBegin(GL_QUADS);
     glColor4f(r,g,b,a);
     glNormal3f( 0, 0,-1);
     glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
     glTexCoord2f(tex_x,0); glVertex3f(-1,-1,-1);
     glTexCoord2f(tex_x,tex_y); glVertex3f(-1,+1,-1);
     glTexCoord2f(0,tex_y); glVertex3f(+1,+1,-1);
     glEnd();


     glPopMatrix();
 

   //  Undo transformations and textures
     glDisable(GL_TEXTURE_2D);
}
static void storage(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,int side) {

  glPushMatrix();
  if(side==1){
    glRotatef(90,0,1,0);
  }
  glDisable(GL_TEXTURE_2D);

  if(side==1 ||side==2){
  
      cube(x,y,z,2.1,1.4,1.4,0,9,1,1,1,1);
      cube(x,y+1.6,z-0.15,2.1,0.2,1.23,0,4,0.5,0.5,0.5,1);
      cube(x,y+0.2,z+1.55, 0.1,0.3,0.07, 0,4,0.5,0.5,0.5,1);//handle

  }
  
  glRotated(0,0,1,0);
 
  if(side==0){
      cube(x,-2,z,     2.1,1.4,1.4,  180, 9,  1,1,1,1);
      cube(x,-0.4,z,  2.1,0.2,1.23, 180, 4,  0.5,0.5,0.5,1);

  }

  glPopMatrix();
  glPushMatrix();

  if(side==1){
      glRotatef(90,0,1,0);
  }
 
  if (th==0){
      if(side==1||side==2){
          drawCylinder(x,y+1.4,z+1,1.62,2.1,1.60,90,0,90,0,0,0);
      }
  }
  
  else {
      //glRotated(180,1,0,0);
      glRotated(180,1,0,0);
      glRotated(180,0,0,1);
      if(side==0){
      drawCylinder(-1,-0.6,7.5,1.62,2.1,1.6,90,0,90,0,0,0);}
  }
  
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
}

static void Lcounter(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{

    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
  
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(0.5f,0.35f,0.05f);
    glBindTexture(GL_TEXTURE_2D,tex2d[3]);
    
    //Left countertop side
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.35f, 0.05f);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,1);glVertex3f(-4, 0, 0);
    glTexCoord2f(1,1);glVertex3f(-4,0,1);
    glTexCoord2f(1,0);glVertex3f(-4,-0.16,1);
    glTexCoord2f(0,0);glVertex3f(-4,-0.16,0);
    glEnd();

     //  Front
     glBegin(GL_QUADS);
     glColor3f(0.5f, 0.35f, 0.05f);
     glNormal3f( 0, 0, 1);
     glTexCoord2f(0,1);glVertex3f(-4,0,+1);
     glTexCoord2f(1,1);glVertex3f(+1.5,0,+1);
     glTexCoord2f(1,0);glVertex3f(+1.5,-0.16,+1);
     glTexCoord2f(0,0);glVertex3f(-4,-0.16,+1);
     glEnd();
     
     //Bottom
     glBegin(GL_QUADS);
     glColor3f(0.5f, 0.35f, 0.05f);
     glNormal3f(0, -1, 0);
     glTexCoord2f(0,1);glVertex3f(-4,-0.16,+1);
     glTexCoord2f(1,1);glVertex3f(+1.5,-0.16,+1);
     glTexCoord2f(1,0);glVertex3f(+1.5,-0.16,-1);
     glTexCoord2f(0,0);glVertex3f(-4,-0.16,-1);
     glEnd();
    
     //parallel countertop outside edge
      glBegin(GL_QUADS);
      glColor3f(0.5f, 0.35f, 0.05f);
      glNormal3f( 0, 0, 1);
      glTexCoord2f(0,1);glVertex3f(4.5,0,-3);
      glTexCoord2f(1,1);glVertex3f(+1.5,0,+1);
      glTexCoord2f(1,0);glVertex3f(+1.5,-0.16,+1);
      glTexCoord2f(0,0);glVertex3f(4.5,-0.16,-3);
      glEnd();

      //parallel countertop bottom edge
      glBegin(GL_QUADS);
      glColor3f(0.5f, 0.35f, 0.05f);
      glNormal3f(0, -1, 0);
      glTexCoord2f(0,1);glVertex3f(4.5,-0.16,-3);
      glTexCoord2f(1,1);glVertex3f(+1.5,-0.16,+1);
      glTexCoord2f(1,0);glVertex3f(+1.5,-0.16,-1);
      glTexCoord2f(0,0);glVertex3f(4.5,-0.16,-3);
      glEnd();

 
      glScaled(dx,dy,dz); //just want to scale from this point

     //  Front
     glBegin(GL_QUADS);
     glColor3f(0.5f, 0.35f, 0.05f);
     glNormal3f( 0, 0, 1);
     glTexCoord2f(0,1);glVertex3f(-4,0,0);
     glTexCoord2f(1,1);glVertex3f(+1.5,0,0);
     glTexCoord2f(1,0);glVertex3f(+1.5,-0.16,0);
     glTexCoord2f(0,0);glVertex3f(-4,-0.16,0);
     glEnd();

     //  Right
     glBegin(GL_QUADS);
     glColor3f(0.5f, 0.35f, 0.05f);
     glNormal3f(1/sqrt(2), 0, 1/sqrt(2));
     glTexCoord2f(0,1);glVertex3f(+1.5,0,0);
     glTexCoord2f(1,1);glVertex3f(+4,0,-3);
     glTexCoord2f(1,0);glVertex3f(+4,-0.16,-3);
     glTexCoord2f(0,0);glVertex3f(+1.5,-0.16,0);
     glEnd();

     //  Left
      glBegin(GL_QUADS);
      glColor3f(0.5f, 0.35f, 0.05f);
      glNormal3f(-1,0,0);
      glTexCoord2f(0,1);glVertex3f(-4, 0, -1);
      glTexCoord2f(1,1);glVertex3f(-4,0,0);
      glTexCoord2f(1,0);glVertex3f(-4,-0.16,0);
      glTexCoord2f(0,0);glVertex3f(-4,-0.16,-1);
      glEnd();

   //  Top
     glBegin(GL_QUADS);
     glColor3f(0.5f, 0.35f, 0.05f);
     glNormal3f(0, 1, 0);
     glTexCoord2f(0,0);glVertex3f(-4,0,+1);//bottom left
     glTexCoord2f(1,1);glVertex3f(+1.5,0,+1);//bottom right
     glTexCoord2f(1,0);glVertex3f(+3,0,-1);//top left
     glTexCoord2f(0,1);glVertex3f(-4,0,-1);//top left
     glEnd();

    //parallelogram
    //top
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.35f, 0.05f);//brown
    glNormal3f(0,1,0);
    glTexCoord2f(0,0);glVertex3f(+1,0,-1);
    glTexCoord2f(1,0);glVertex3f(+3,0,-1);
    glTexCoord2f(1,1);glVertex3f(+4.5,0,-3);
    glTexCoord2f(0,1);glVertex3f(+2.5,0,-3);
    glEnd();
    //glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,tex2d[0]);

 
    //  Back
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f(0, 0, -1);
    glTexCoord2f(10,10);glVertex3f(+2,0,-1);
    glTexCoord2f(0,10);glVertex3f(-4,0,-1);
    glTexCoord2f(0,0);glVertex3f(-4,-0.16,-1);
    glTexCoord2f(10,0);glVertex3f(+2,-0.16,-1);
    glEnd();

    //  Bottom
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0,10);glVertex3f(-4,-0.16,0);//bottom left
    glTexCoord2f(10,10);glVertex3f(+1,-0.16,0);//bottom right
    glTexCoord2f(10,0);glVertex3f(+1,-0.16,-1);//bottom right
    glTexCoord2f(0,0);glVertex3f(-4,-0.16,-1);//top left
    glEnd();

    // Back
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f(0,0,-1);
    glTexCoord2f(10,0);glVertex3f(+2,0,-2);
    glTexCoord2f(0,0);glVertex3f(+2,-0.16,-2);
    glTexCoord2f(0,10);glVertex3f(+4,-0.16,-5);
    glTexCoord2f(10,10);glVertex3f(+4,0,-5);
    glEnd();
  
    //left
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f(-1/sqrt(2),0,-1/sqrt(2));
    glTexCoord2f(10,10);glVertex3f(+2,0,-2);
    glTexCoord2f(10,0);glVertex3f(+2,-0.16,-2);
    glTexCoord2f(0,0);glVertex3f(+1,-0.16,-1);
    glTexCoord2f(0,10);glVertex3f(+1,0,-1);
    glEnd();


    //  Undo transformations and textures
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}

static void island(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,int tex,float r, float g, float b, int woodtop, int mid_div ){
  
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    int tex_x=1;
    int tex_y=1;

    //  Enable textures
     glEnable(GL_TEXTURE_2D);
     glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    if(tex==1){
        glColor3f(0.5f,0.35f,0.05f);
        glBindTexture(GL_TEXTURE_2D,tex2d[3]);   //counter.bmp
    }

    else {
        glColor3f(r,g,b);
        glBindTexture(GL_TEXTURE_2D,tex2d[tex]);
    }

    if(mid_div==1){
      
        glColor3f(1,1,1);
        glBindTexture(GL_TEXTURE_2D,tex2d[0]);
        r=1;
        g=1;
        b=1;
        tex_x=10;
        tex_y=10;
     }

     //  Front -- doesn't need texture anywhere
     glBegin(GL_QUADS);
     glColor3f(r,g,b);
     glNormal3d(0,0,1);
     glTexCoord2f(0,0);glVertex3f(-1,-0.16, 1);
     glTexCoord2f(tex_x,0);glVertex3f(+1,-0.16, 1);
     glTexCoord2f(tex_x,tex_y);glVertex3f(+1,0, 1);
     glTexCoord2f(0,tex_y);glVertex3f(-1,0, 1);
     glEnd();

     //  Back
     glBegin(GL_QUADS);
     glColor3f(r,g,b);
     glNormal3d(0,0,-1);
     glTexCoord2f(0,tex_y);glVertex3f(+1,-0.16,-1);
     glTexCoord2f(0,0);glVertex3f(-1,-0.16,-1);
     glTexCoord2f(tex_x,0);glVertex3f(-1,0,-1);
     glTexCoord2f(tex_x,tex_y);glVertex3f(+1,0,-1);
     glEnd();

     //  Right
     glBegin(GL_QUADS);
     glColor3f(r,g,b);
     glNormal3d(1,0,0);
     glTexCoord2f(tex_x,0);glVertex3f(+1,-0.16,+1);
     glTexCoord2f(0,0);glVertex3f(+1,-0.16,-1);
     glTexCoord2f(0,tex_y);glVertex3f(+1,0,-1);
     glTexCoord2f(tex_x,tex_y);glVertex3f(+1,0,+1);
     glEnd();
 
     if (woodtop==1){

         //  Left-- top needs texture
         glBegin(GL_QUADS);
         glColor3f(r,g,b);
         glNormal3d(-1,0,0);
         glTexCoord2f(0,0);glVertex3f(-1,-0.16,-1);
         glTexCoord2f(tex_x,0);glVertex3f(-1,-0.16,+1);
         glTexCoord2f(tex_x,tex_y); glVertex3f(-1,0,+1);
         glTexCoord2f(0,tex_y);glVertex3f(-1,0,-1);
         glEnd();

         //  Top -- needs texture if it's not the bottom shelf
         glBegin(GL_QUADS);
         glColor3f(r,g,b);
         glNormal3d(0,1,0);
         glTexCoord2f(0,tex_y);glVertex3f(-1,0,+1);
         glTexCoord2f(tex_x,tex_y);glVertex3f(+1,0,+1);
         glTexCoord2f(tex_x,0);glVertex3f(+1,0,-1);
         glTexCoord2f(0,0);glVertex3f(-1,0,-1);
         glEnd();
      }

     //  Bottom
     glBegin(GL_QUADS);
     glColor3f(r,g,b);
     glNormal3d(0,-1,0);
     glTexCoord2f(0,0);glVertex3f(-1,-0.16,-1);
     glTexCoord2f(0,tex_y);glVertex3f(+1,-0.16,-1);
     glTexCoord2f(1,tex_y);glVertex3f(+1,-0.16,+1);
     glTexCoord2f(1,0);glVertex3f(-1,-0.16,+1);
     glEnd();

 
     if (woodtop==0){

         //  Left-- wall textures
         glEnable(GL_TEXTURE_2D);
         glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
         glColor3f(1,1,1);
         glBindTexture(GL_TEXTURE_2D,tex2d[0]);
         glBegin(GL_QUADS);
         glColor3f(1,1,1);
         glNormal3d(-1,0,0);
         glTexCoord2f(0,0);glVertex3f(-1,-0.16,-1);
         glTexCoord2f(10,0);glVertex3f(-1,-0.16,+1);
         glTexCoord2f(10,10); glVertex3f(-1,0,+1);
         glTexCoord2f(0,10);glVertex3f(-1,0,-1);
         glEnd();

         //top
         glBegin(GL_QUADS);
         glColor3f(1,1,1);
         glNormal3d(0,1,0);
         glTexCoord2f(0,10);glVertex3f(-1,0,+1);
         glTexCoord2f(10,10);glVertex3f(+1,0,+1);
         glTexCoord2f(10,0);glVertex3f(+1,0,-1);
         glTexCoord2f(0,0);glVertex3f(-1,0,-1);
         glEnd();
       }
 
     glPopMatrix();
     glDisable(GL_TEXTURE_2D);

}

static void chairs(double x, double z, int zxis) {

    double offs = 0.36;
    //seat base
    cube(x,-1,z,  0.8,0.1,0.8,  90, 3,  0.5,0.35,0.05,1);

    //4 legs
    cube(x+offs,-2.3,z+offs, 0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1);
    cube(x+offs,-2.3,z-offs, 0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1);
    cube(x-offs,-2.3,z+offs, 0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1);
    cube(x-offs,-2.3,z-offs, 0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1);

    if (zxis==1) {
        //side part
        cube(x-offs,0.3,z-offs, 0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1); //3 sides 
        cube(x+offs,0.3,z-offs, 0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1);
        cube(x,0.3,z-offs,      0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1);

        cube(x,1.25,z-offs, 0.1,0.1,0.8,  90, 3,  0.5,0.35,0.05,1); //top
    }
    else {
        //side part
        cube(x-offs,0.3,z+offs, 0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1); //3 sides 
        cube(x+offs,0.3,z+offs, 0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1);
        cube(x,0.3,z+offs,      0.1,1.3,0.1,  90, 3,  0.5,0.35,0.05,1);

        cube(x,1.25,z+offs, 0.1,0.1,0.8,  90, 3,  0.5,0.35,0.05,1); //top 
    }
 }


static void seats(double x, double z){
  
  //seats base 
  drawCylinder(x,-3.1,z,1.5,0.05,1.5,0,0,360,0,0,0);
  drawCylinder(x-2.5,-3.1,z,1.5,0.05,1.5,0,0,360,0,0,0);
  
  //leg
  drawCylinder(x,-2.1,z,0.7,1,0.7,0,0,360,0,0,0);
  drawCylinder(x-2.5,-2.1,z,0.7,1,0.7,0,0,360,0,0,0);
  
  //grey top
  drawCylinder(x,-1.1,z,2,0.04,2,0,0,360,1,0,0);
  drawCylinder(x-2.5,-1.1,z,2,0.04,2,0,0,360,1,0,0);

  
}

static void pictureFrame(double z, int which){

  double x = -18.7;
  glPushMatrix();
  glScaled(1,1.7,1.4);
  if (which == 11) {
    cube(x+0.15,  3.1-1.35,  z+0.09,      0.5,  1.65,  1.1,  0,  11,  0.90, 0.90, 0.90, 1);//hanging picture 1
  }
  else if (which == 12) {
    cube(x+0.15,  3.1-1.35,  z+0.09,      0.5,  1.65,  1.1,  0,  12,  0.90, 0.90, 0.90, 1);//hanging picture 2

  }
  else if (which == 13) {
    cube(x+0.15,  3.1-1.35,  z+0.09,      0.5,  1.65,  1.1,  0,  13,  0.90, 0.90, 0.90, 1);//hanging picture 3
  }
  cube(x,  3.1-1.5,  z,      0.1,  1.8,  1.2,  0,  4,  0.90, 0.90, 0.90, 1);//board
  cube(x,  3.1-1.5,  z-1.2,  0.1,  1.8,  0.04, 0,  7,  1,    1,    1,    1);//right
  cube(x,  3.1-1.5,  z+1.2,  0.1,  1.8,  0.04, 0,  7,  1,    1,    1,    1);//left
  cube(x,  1.25-1.5, z,      0.12, 0.04, 1.24, 0,  7,  1,    1,    1,    1);//right
  cube(x,  4.95-1.5, z,      0.12, 0.04, 1.24, 0,  7,  1,    1,    1,    1);//right
  
  glPopMatrix();
}

 
static void walls(){

  float offset= 6;
  glPushMatrix();
  int theta[2] = {180,90};
  
  //back white
  cube(7.3+4, 4.7,  -15-offset, 7.7-4, 4, 0.16, theta[0], 100,0.9,0.9,0.9,0.9);//top blue back
  cube(7.3+4, -1.3, -15-offset, 7.7-4, 2, 0.16, theta[0], 100,0.9,0.9,0.9,0.9);//bottom blue back

  // main door area  
  cube(7.3+4-8, 4.7-2,  -15-offset, 7.7-3.4, 6, 0.16, theta[0], 10,0.1, 0.5,0.8,1);//top blue back
  
  
  //right wall
  cube(15,-1.3,-3,  18,2,0.16,theta[1],100,0.9,0.9,0.9,0.9);//bottom blue back
  cube(15,4.7,-3,   18,4,0.16,theta[1],100,0.9,0.9,0.9,0.9);//top blue back

   
  glPopMatrix();
}

static void sinks(){
  
  glPushMatrix();
  glTranslated(-7-3,-1,-25);
  glRotatef(90,0,1,0);
  cube(-5,0,-6.5,0.7,0.8,0.01,0,4,0.50,0.50,0.50,1);
  cube(-5,0,-5,0.7,0.8,0.01,0,4,0.50,0.50,0.50,1);
  cube(-5,0,-3.5,0.7,0.8,0.01,0,4,0.50,0.50,0.50,1);
  cube(-5,0,-2,0.7,0.8,0.01,0,4,0.50,0.50,0.50,1);
  glPopMatrix();
  
  glPushMatrix();
  glTranslated(-7-3,-1,-25);
  glRotatef(90,1,0,0);
  cube(-4.25,5,0.8,2.25,0.7,0.01,0,4,0.50,0.50,0.50,1);//bottom
  cube(-1,5.2,-0.7,1,0.9,0.1,0,4,0.50,0.50,0.50,1);//right top
  cube(-1,6.0,-0.8,1.1,0.1,0.2,0,4,0.50,0.50,0.50,1);//right top bottom
  cube(0.1,5.2,-0.8,0.1,0.9,0.2,0,4,0.50,0.50,0.50,1);//right top right
  cube(-2,5.9,-0.8,0.1,0.2,0.2,0,4,0.50,0.50,0.50,1);//right top left
  cube(-7.5,5.2,-0.7,1,0.9,0.1,0,4,0.50,0.50,0.50,1);//left top
  cube(-1-6.5,6.0,-0.8,1.1,0.1,0.2,0,4,0.50,0.50,0.50,1);//left top bottom
  cube(-8.5,5.2,-0.8,0.1,0.9,0.2,0,4,0.50,0.50,0.50,1);//left top right
  cube(-6.5,5.9,-0.8,0.1,0.2,0.2,0,4,0.50,0.50,0.50,1);//left top left
  glPopMatrix();

  glPushMatrix();
  glTranslated(-7-3,-1,-25);
  cube(-4.25,0,4.3,2.25,0.8,0.01,0,4,0.50,0.50,0.50,1);//back
  cube(-4.25,0,5.7,2.25,0.8,0.01,0,4,0.50,0.50,0.50,1);//front
  glPopMatrix();

}

static void tables(double x,double z, int size,int rott){
     
     glPushMatrix();
 
    if (size==1){
        if(rott!=0){
            glRotated(90,0,1,0);
        }

        //table legs
        drawCylinder(x,   -2.1, z-1, 0.4, 1.4, 0.4, 0, 0, 360,0,0,0);
        drawCylinder(x,   -2.1, z+3, 0.4, 1.4, 0.4, 0, 0, 360,0,0,0);
        drawCylinder(x+2, -2.1, z-1, 0.4, 1.4, 0.4, 0, 0, 360,0,0,0);
        drawCylinder(x+2, -2.1, z+3, 0.4, 1.4, 0.4, 0, 0, 360,0,0,0);
        
        //top
        cube(x+1,-1+.2,z+1, 1.2,0.1,2.4, 0,4, 0.50,0.50,0.50,1);
        
        //lower tabletop
        cube(x+1,-2.7+.2,z+1,1.1,0.07,2.2,0,4,0.50,0.50,0.50,1);
    }

    else if (size==2){
        //table legs
        drawCylinder(x-1.5,-2.1,z,     0.8,1.2,0.8, 0,  0,360,0,0,0);
        drawCylinder(x-1.5,-2.1,z+3.5,   0.8,1.2,0.8, 0,  0,360,0,0,0);
        drawCylinder(x+2.5,-2.1,z,   0.8,1.2,0.8, 0,  0,360,0,0,0);
        drawCylinder(x+2.5,-2.1,z+3.5, 0.8,1.2,0.8, 0,  0,360,0,0,0);
        //tabletop
        cube(x+0.5,-0.9,z+1,    2.7,0.1,2.7,   0,8,0.1, 0.5,0.8,1);
        
    }

    else{
        //table legs
        drawCylinder(x,-2.1,z,0.4,1.4,0.4,0,0,360,0,0,0);
        drawCylinder(x,-2.1,z+2,0.4,1.4,0.4,0,0,360,0,0,0);
        drawCylinder(x+2,-2.1,z,0.4,1.4,0.4,0,0,360,0,0,0);
        drawCylinder(x+2,-2.1,z+2,0.4,1.4,0.4,0,0,360,0,0,0);
        //tabletop
        cube(x+1,-1+.2,z+1,1.2,0.1,1.2,0,4,0.50,0.50,0.50,1);
        //lower tabletop
        cube(x+1,-2.7+.2,z+1,1.1,0.07,1.1,0,4,0.50,0.50,0.50,1);
    }
  
    glPopMatrix();
}


static void handrail(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,int top)
{

     glPushMatrix();
     glTranslated(x,y,z);
     if (top==1){
         glRotated(th,0,0,1);
         glRotated(th,1,0,0);
         glRotated(th,0,1,0);
     }
     else {
         glRotated(th,0,1,0);}
         glScaled(dx,dy,dz);
         double i;
  
         //bottom
        glColor3f(0.1,0.1,0.1);
        glNormal3d(0,-1,0);
        glBegin(GL_TRIANGLE_FAN);

        glTexCoord2f(0.5,0.5);
        glVertex3d(0.0, 0, 0.0);
        
        for(i = 0.0; i <= 360; i+=10) {
            glVertex3d(0.5 *0.5 *Cos(i), 0, 0.5*0.5 * Sin(i));
        }
        
        glEnd();
        
        //side
        double a = (.75 - 1) / 0.66*0.5;
        double b = 1;
        glBegin(GL_TRIANGLE_STRIP);
        
        for(i = 0.0; i <= 360; i+=10) {
            glColor3f(0.1,0.1,0.1);
            glNormal3d(Cos(i), 0, Sin(i));
            double x = 0.5 *0.5* Cos(i);
            double z = 0.5 *0.5* Sin(i);
            double height2 = a * x + b;
            glVertex3d(x, 0.0,z);
            glVertex3d(x, height2,z);}
            glEnd();
        
        //top 
        glBegin(GL_TRIANGLE_FAN);
        glVertex3d(0.0, 1, 0); // center
        for(i = 0.0; i <= 360; i+=10) {
            double x = 0.5* 0.5 * Cos(i);
            double z = 0.5*0.5 * Sin(i);
            double height3 = a * x + b;
            glVertex3d(x,height3,z);
        }

        glEnd();
         //  Undo transformations and textures
             glPopMatrix();
             glDisable(GL_TEXTURE_2D);
}


static void floor_mat(double x, double z, int th){ 

    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ZERO);
    glRotated(th,0,1,0);
    if (th == 0){
      cube(x,-3.1,z,  3.5,0.05,1.1,0,5,1,1,1,1);//base
    }
    else { //smaller ones
      cube(x,-3.1,z,  1.1,0.05,2,0,19,1,1,1,1);//base


    }
    glDisable(GL_BLEND);
    glPopMatrix();
}

static void wine_wardrobe(){
  float offset= 5.5;//moving everything back towards back wall
  glPushMatrix();
  // glTranslated(x,y,z);

  cube(13,0.05,-14-offset,1.7,3.3,0.1,0,4,0.50,0.50,0.50,1);//back
  cube(11.25,0.05,-12.7-offset,1.4,3.3,0.1,90,4,0.50,0.50,0.50,1);//left
  cube(12.9,-0.35,-11.4-offset,0.1,2.8,0.1,90,4,0.50,0.50,0.50,1);//middle
  cube(14.6,0.05,-12.7-offset,1.4,3.3,0.1,270,4,0.50,0.50,0.50,1);//right
  cube(12.91,-2.7,-12.69-offset,1.39,0.5,1.59,270,4,0.50,0.50,0.50,1);//bottom
  cube(12.925,3.3,-12.7-offset,1.77,0.1,1.4,0,4,0.50,0.50,0.50,1);//top
  cube(12.925,2.85,-12.51-offset,1.7,0.45,1.2,0,6,0.8,0.8,0.8,1);//top

  cube(12.1,-2.2,-11.29-offset,0.71,0.05,0.01,0,0,0.1,0.1,0.1,0.4);//l bot
  cube(11.358,0.045,-11.29-offset,0.05,2.3,0.01,0,0,0.1,0.1,0.1,0.4);//l l
  cube(12.85,0.045,-11.29-offset,0.05,2.3,0.01,0,0,0.1,0.1,0.1,0.4);//l r
  cube(12.1,2.3,-11.29-offset,0.71,0.05,0.01,0,0,0.1,0.1,0.1,0.4);//l top

  cube(13,0.045,-11.29-offset,0.05,2.3,0.01,0,0,0.1,0.1,0.1,0.4);//r l
  cube(14.51,0.045,-11.29-offset,0.05,2.3,0.01,0,0,0.1,0.1,0.1,0.4);//r r
  cube(13.758,-2.2,-11.29-offset,0.71,0.05,0.01,0,0,0.1,0.1,0.1,0.4);//r bot
  cube(13.758,2.3,-11.29-offset,0.71,0.05,0.01,0,0,0.1,0.1,0.1,0.4);//r top

  cube(12.1,-2.2,-11.235-offset,0.73,0.09,0.04,0,4,0.50,0.50,0.50,1);//l door frame bot
  cube(12.1,-2.2,-11.2-offset,0.75,0.02,0.04,0,4,0.50,0.50,0.50,1);//l door frame bot inner
  cube(11.358,0.045,-11.235-offset,0.055,2.35,0.04,0,4,0.50,0.50,0.50,1);//l door frame l
  cube(11.358,0.045,-11.2-offset,0.02,2.25,0.04,0,4,0.50,0.50,0.50,1);//l door frame l inner
  cube(12.1,2.3,-11.235-offset,0.73,0.09,0.04,0,4,0.50,0.50,0.50,1);//l door frame top
  cube(12.1,2.3,-11.2-offset,0.75,0.02,0.04,0,4,0.50,0.50,0.50,1);//l door frame top inner
  cube(12.845,0.045,-11.235-offset,0.055,2.345,0.04,0,4,0.50,0.50,0.50,1);//l door frame r
  cube(12.845,0.045,-11.2-offset,0.02,2.25,0.04,0,4,0.50,0.50,0.50,1);//l door frame r inner

  cube(13.772,-2.2,-11.235-offset,0.73,0.09,0.04,0,4,0.50,0.50,0.50,1);//r door frame bot
  cube(13.7715,-2.2,-11.2-offset,0.77,0.02,0.04,0,4,0.50,0.50,0.50,1);//r door frame bot inner
  cube(13,0.045,-11.235-offset,0.055,2.35,0.04,0,4,0.50,0.50,0.50,1);//r door frame l
  cube(13,0.045,-11.2-offset,0.02,2.25,0.04,0,4,0.50,0.50,0.50,1);//r door frame l inner
  cube(13.76,2.3,-11.235-offset,0.73,0.09,0.04,0,4,0.50,0.50,0.50,1);//r door frame top
  cube(13.755,2.3,-11.2-offset,0.77,0.02,0.04,0,4,0.50,0.50,0.50,1);//r door frame top inner
  cube(14.51,0.045,-11.235-offset,0.055,2.35,0.04,0,4,0.50,0.50,0.50,1);//r door frame r
  cube(14.51,0.045,-11.2-offset,0.02,2.25,0.04,0,4,0.50,0.50,0.50,1);//r door frame r inner

  cube(12.842,0.045,-11-offset,0.04,0.6,0.02,0,4,0.50,0.50,0.50,1);//left door handle
  cube(12.842,-0.4,-11.1-offset,0.02,0.02,0.08,0,4,0.50,0.50,0.50,1);//left door handle attach
  cube(12.842,0.45,-11.1-offset,0.02,0.02,0.08,0,4,0.50,0.50,0.50,1);//left door handle attach
  cube(12.997,0.045,-11-offset,0.04,0.6,0.02,0,4,0.50,0.50,0.50,1);//right door handle
  cube(12.997,-0.4,-11.1-offset,0.02,0.02,0.08,0,4,0.50,0.50,0.50,1);//left door handle attach
  cube(12.997,0.45,-11.1-offset,0.02,0.02,0.08,0,4,0.50,0.50,0.50,1);//left door handle attach
  
  glPopMatrix();
  glPushMatrix();
    glRotated(-30,1,0,0);
    cube(12.91,5.4+0.8,-11-offset+.75,0.01,0.05,1.59,90,4,0.50,0.50,0.50,1);//vents
    cube(12.91,5.3+0.8,-11.1-offset+.75,0.01,0.05,1.59,90,4,0.50,0.50,0.50,1);//vents
    cube(12.91,5.2+0.8,-11.15-offset+.75,0.01,0.05,1.59,90,4,0.50,0.50,0.50,1);//vents
    cube(12.91,5.1+0.8,-11.2-offset+.75,0.01,0.05,1.59,90,4,0.50,0.50,0.50,1);//vents
    cube(12.91,5+0.8,-11.25-offset+.75,0.01,0.05,1.59,90,4,0.50,0.50,0.50,1);//vents
  glPopMatrix();
  glPushMatrix();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  cube(12.1,0.05,-11.25-5.5,0.68,2.15,0.05,0,1000,1,1,1,0.4);//left door
  cube(13.74,0.05,-11.25-5.5,0.68,2.15,0.05,0,1000,1,1,1,0.4);//right door
  glPopMatrix();
  glDisable(GL_BLEND);

}
  
static void fullrail(int flip,int th){
     glPushMatrix();
     if (flip==0){
       handrail(10+2,-3.25,3,0.5,2.6,0.5,th,0);
       handrail(10+2,-.649,0.4,0.5,2.6,0.5,th,1);
       drawCylinder(10+2,-.649,-2.5,0.5,3,0.5,th,1,360,0,0,1);
     }
     else if (flip==1){
       glRotatef(180,0,1,0);
       handrail(-10-2,-3.25,7,0.5,2.6,0.5,th,0);
       handrail(-10-2,-.649,4.4,0.5,2.6,0.5,th,1);
     }
     glPopMatrix();

}

 static void windows(){
   glPushMatrix();
   glScaled(0.9,1,1);
   glTranslated(-2,0,0);

   //sides
   cube(2.55,2.7,15,    0.2,4,0.15,  0,  4,  0.5,0.5,0.5,1);//window divider
   cube(-5.9,2.7,15,   0.2,4,0.15,  0,  4,  0.5,0.5,0.5,1);//window divider
   cube(-15.2,2.7,15,  0.2,4,0.15,  0,  4,  0.5,0.5,0.5,1);//window divider
   
   //top/bottom
   cube(0+0.9-4.6-2.8,           2.7+4,       15.2, 10.9-1.9,  0.15, 0.7,0,4,0.5,0.5,0.5,1);//window divider
   cube(0+0.9-4.6-2.8,           2.7-4,       15.2, 10.9-1.85,  0.15, 0.7,0,4,0.5,0.5,0.5,1);//window divider

   //curtain 
   cube(-17,2.7,14.3,  2,4,0.1,  0,15,0.5,0.5,0.5,1);//window divider
   cube(2,2.7,14.3,  2,4,0.1,  0,15,0.5,0.5,0.5,1);//window divider
   cube(8.5,0.5,14.3,  1.1,3.27,0.1,  0,15,0.5,0.5,0.5,1);//window divider
   cube(16.3,0.5,14.3,  1.1,3.27,0.1,  0,15,0.5,0.5,0.5,1);//window divider


   cube(0+0.9+12,  2.7-4-2,   16,  4,        0.15, 1,0,4,0.5,0.5,0.5,1);//forward lane
    
   glPushMatrix();
   cube(14.6-.05,3.435,15.2,  1.85,0.09,0.7,  0,4,0.5,0.5,0.5,1);//door top
   cube(14.6-.05,-3.1,15,   1.85,0.035,0.05,0,4,0.5,0.5,0.5,1);//door bottom
   cube(14.6+1.82-.05,0.18,15,  0.1,3.27,0.07,0,4,0.5,0.5,0.5,1);//door
   cube(14.6-1.82+.05,0.18,15,  0.1,3.27,0.07,0,4,0.5,0.5,0.5,1);//door
   cube(14.6-.07,0,14.6,1.3,0.1,0.07,0,4,0.5,0.5,0.5,1);//door inside handle
   cube(14.6-.07,0,14.7,1.2,0.09,0.03,0,4,0.1,0.1,0.1,1);//door inside handle
   
   cube(14.6-.05-1.85*2,3.435,15.2,  1.85,0.09,0.7,0,4,0.5,0.5,0.5,1);//door top
   cube(14.6-.05-1.85*2,-3.1,15,1.85,0.035,0.05,0,4,0.5,0.5,0.5,1);//door bottom
   cube(14.6+1.82-.05-1.85*2,0.18,15,0.1,3.27,0.07,0,4,0.5,0.5,0.5,1);//door
   cube(14.6-1.82+.05-1.85*2,0.18,15,0.1,3.27,0.07,0,4,0.5,0.5,0.5,1);//door
   cube(14.6-.07-1.85*2,0,14.6,1.3,0.1,0.07,0,4,0.5,0.5,0.5,1);//door inside handle
   cube(14.6-.07-1.85*2,0,14.7,1.2,0.09,0.03,0,4,0.1,0.1,0.1,1);//door inside handle

   glPopMatrix();
   glPushMatrix();
   glRotated(90,0,1,0);
   glRotated(90,1,0,0);
   // drawCylinder(-15.2,14.7,-1.5,4,0.01,4,90,0,360,45,0,2);//door sticker outside
   glPopMatrix();

   glPushMatrix();
   glRotated(270,0,1,0);
   glRotated(90,1,0,0);
   glPopMatrix();

   glPushMatrix();
   glTranslated(10.4,0,15.3);//door handle
   glRotated(90,1,0,0);
   glScaled(0.5,0.05,0.08);
   fullrail(0,90);
   fullrail(1,90);
   glPopMatrix();
  
 
    glPopMatrix();
 
 }
 
/*
 *  Set light
 *    light>0 bright
 *    light<0 dim
 *    light=0 off
 */

static void floorOutliner() {

  float offset= 6;

  //back right
  for(double k=7.7;k<13;k+=2){
    for(double i=0;i<1;i+=2){
      for(double j=0;j<1.5;j+=0.5){
        cube(0+j+k,-1.75-1.25,-14.69-offset,  0.2,0.25,0.04,180,7,1, 1,1,1);//thick plank
      }
      cube(1.4+i+k,-1.75-1.25,-14.69-offset,  0.1,0.25,0.04,180,7,1, 1,1,1);//thin plank
    }
 
     cube(7.3+4,-2.5-0.5,-14.78-offset,  7.7-4,0.04,0.02,180,7,1, 1,1,1);//horiz plank
  }

  // front
  for(double k=-2;k<20;k+=2){
    for(double i=-2;i<1;i+=2){
      for(double j=-2;j<1.5;j+=0.5){
        cube(-14.8+j+k,-1.75-1.25,14.69,  0.2,0.25,0.04,180,7,1, 1,1,1);//thick plank
      }
      cube(-13.4+i+k,-1.75-1.25,14.69,  0.1,0.25,0.04,180,7,1, 1,1,1);//thin plank
    }
    
    cube(-2,-3,14.78, 8,0.04,0.02,180,7,1, 1,1,1);//horiz plank
  }

  //right
  glPushMatrix();
  glRotated(90,0,1,0);

  for(double k=0;k<36;k+=2){
    for(double i=0;i<1;i+=2){
      for(double j=0;j<1.5;j+=0.5){
        cube(-14.8+j+k,-1.75-1.25,14.69,  0.13,0.25,0.04,180,7,1, 1,1,1);//thick plank
      }
      cube(-13.4+i+k,-1.75-1.25,14.69,  0.1,0.25,0.04,180,7,1, 1,1,1);//thin plank
    }
    
    cube(3,-2.5-0.5,14.78,18,0.04,0.02,180,7,1, 1,1,1);//horiz plank
  }

  //left
  for(double k=0;k<36;k+=2){
    for(double i=0;i<1;i+=2){
      for(double j=0;j<1.5;j+=0.5){
        cube(-14.8+j+k,-1.75-1.25,-18.6,  0.2,0.25,0.04,180,7,1, 1,1,1);//thick plank
      }
      cube(-13.4+i+k,-1.75-1.25,-18.6,  0.1,0.25,0.04,180,7,1, 1,1,1);//thin plank
    }
    
    cube(3,-2.5-0.5,-18.6, 18,0.04,0.02,180,7,1, 1,1,1);//horiz plank
  }
  glPopMatrix();

  // back left        cube(-14+4,2.7,-21,  5+4,6,0.16,0,100,0.9,0.9,0.9,0.9);//back left wall

  for(double k=-10;k<0;k+=2){
    for(double i=-10;i<0;i+=2){
      for(double j=-9;j<1.5;j+=0.5){
        cube(0+j+k,-1.75-1.25,-14.69-offset,  0.2,0.25,0.04,180,7,1, 1,1,1);//thick plank
      }
      cube(1.4+i+k,-1.75-1.25,-14.69-offset,  0.1,0.25,0.04,180,7,1, 1,1,1);//thin plank
    }
 
     cube(-10,-2.5-0.5,-14.78-offset,  8,0.04,0.02, 180,7,1, 1,1,1);//horiz plank
  }
  
}

static void Light(int light)
{
   //  Set light position
   Lpos[0] = 2*Cos(zh);
   Lpos[1] = Ylight;
   Lpos[2] = -2*Sin(zh);
   Lpos[3] = 1;
   Lpos2[0] = -13;
   Lpos2[1] = Ylight-25;
   Lpos2[2] = -5;
   Lpos2[3] = 1;

   //  Enable lighting
   if (light) {
      float Med[]  = {0.3,0.3,0.3,1.0};
      float High[] = {1.0,1.0,1.0,1.0};
      //  Enable lighting with normalization
      glEnable(GL_LIGHTING);
      glEnable(GL_NORMALIZE);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0,GL_POSITION,Lpos);
      glLightfv(GL_LIGHT0,GL_AMBIENT,Med);
      glLightfv(GL_LIGHT0,GL_DIFFUSE,High);
      // glEnable(GL_LIGHT2);
      // glLightfv(GL_LIGHT2,GL_POSITION,Lpos2);
      // glLightfv(GL_LIGHT2,GL_AMBIENT,Med);
      // glLightfv(GL_LIGHT2,GL_DIFFUSE,High);
   }
   else {
      glDisable(GL_LIGHTING);
      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_NORMALIZE);
   }
}

/*
 *  Draw scene
 *    light (true enables lighting)
 */
void Scene(int light)
{

     //  Set light position and properties
     Light(light);

     //Draw scene
     glPushMatrix();
     
     // 4 yellow outliners in total 
     floorOutliner();
     
     sinks();

 
     //3 at back     
     storage(1+2.5-12,-2,-2-17.5,        2.5,1.5,1,    0,2);
     storage(1+5-12,-2,-2-17.5,        2.5,1.5,1,    0,2);
     storage(1+7.5-12,-2,-2-17.5,        2.5,1.5,1,    0,2);

     //3 on top
     storage(1-2.5-12,5,-2-17.5,        2.5,1.5,1,    0,2);
     storage(1-12,5,-2-17.5,        2.5,1.5,1,    0,2);
     storage(1+2.5-12,5,-2-17.5,        2.5,1.5,1,    0,2);
     storage(1+5-12,5,-2-17.5,        2.5,1.5,1,    0,2);
     storage(1+7.5-12,5,-2-17.5,        2.5,1.5,1,    0,2);

     //6 on left 
     storage(13+2.5,-2,-17.5,    2.5,1.5,1,    180,1);
     storage(13,-2,-17.5,    2.5,1.5,1,    180,1);
     storage(13-2.5,-2,-17.5,    2.5,1.5,1,    180,1);
     storage(13-5,-2,-17.5,    2.5,1.5,1,    180,1);
     storage(13-7.5,-2,-17.5,    2.5,1.5,1,    180,1);
     storage(13-10,-2,-17.5,    2.5,1.5,1,    180,1);

     //4 top left
     storage(13-2.5,5,-17.5,    2.5,1.5,1,    180,1);
     storage(13-5,5,-17.5,    2.5,1.5,1,    180,1);
     storage(13-7.5,5,-17.5,    2.5,1.5,1,    180,1);
     storage(13-10,5,-17.5,    2.5,1.5,1,    180,1);

     //kitchen island
     island(6.5+2-12,0,-4-5,1,1,4,0,1,0.5, 0.35, 0.05,1,0);//top serve top
     island(6.2+2-12,-0.16,-4-5,0.5,4,4,0,4,0.5, 0.5, 0.5,1,0);//top serve middle
     island(4.54+2-12,-0.651,0.38-5,  0.8,1,1.6,   0,  4,  0.50,0.50,0.50,1,0);//bottom triangle 1
     island(5.5+2-12,-0.65,0.38-5,    0.4,1,0.4,   0,  4,  0.50,0.50,0.50,1,0);//bottom triangle 2
     island(5.16+2-12,-0.65,-4-5,     1.4,1,3.99,  0,  4,  0.50,0.50,0.50,1,0);//top 
     
     island(6+2-12,-3,-4-5,1,1,4,0,1,0.5, 0.35, 0.05,0,0);//bottom 
     island(5.5+2-12,-3,0-5,0.5,1,0.8,0,1,0.5, 0.35, 0.05,0,0);//bottom 
     island(6.84+2-12,-0.16,-4-5,0.16,18.75,4,0,1,0.5, 0.35, 0.05,0,0);//back 
     island(5.99+2-12,-0.7,-5.9-5,1,14.4,0.1,0,1,0.5, 0.35, 0.05,0,1);//mid divider
     
     // L shape island 
     Lcounter(3+2-12,0,3-5, 1,19.75,1,  0);
     
     island(5.99+2-12,-0.80,-7.9-5,1,14.75,0.1,0,1,0.5, 0.35, 0.05,1,0);//end divider 
     

     wine_wardrobe();

  

     // shelf on right wall
     cube(14.5,5.5,4+1,      3,0.16,0.58,  90,  3,  0.5,0.35,0.05,1); 
     cube(14.5,4,4-1.1,      2.5,0.16,0.58,  90,  3,  0.5,0.35,0.05,1);
     cube(14.5,4,4+3.4,      2,0.16,0.58,  90,  3,  0.5,0.35,0.05,1);

     cube(14.5,5.5,2.75+1.5,      0.16,1.5,0.58,  90,  3,  0.5,0.35,0.05,1);
     cube(14.5,5.5,2.74+3,  0.16,1.5,0.58,  90,  3,  0.5,0.35,0.05,1);
 
     //tv & tv shelf
     cube(14.5,-2,4-1.1,      2.5,0.16,0.58,  90,  3,  0.5,0.35,0.05,1);
     cube(14.5,-2,4+3.4,      2,0.16,0.58,  90,  3,  0.5,0.35,0.05,1);
     cube(14.5-0.5,1.1,4.63,      3,2.3,0.15,  90,  14,  0.5,0.35,0.05,1); //screen 
     cube(14.5-0.5,-0.85,4.63,      0.5,1,0.15,  90,  14,  0.5,0.35,0.05,1); //bottom
     
     //screen leg
     cube(14.5,-2.2,2.75-1,      0.16,0.5,0.58,  90,  3,  0.5,0.35,0.05,1);
     cube(14.5,-2.2,2.74+4.8,  0.16,0.5,0.58,  90,  3,  0.5,0.35,0.05,1);

     //dinner table
     cube(-8,0,8,  4,0.1,1.8,  0,  3,  0.5f,0.35f,0.05f,1); // top 
     
     //table legs
     cube(-9.5,-1.5,7, 0.1,1.5,0.05,0,4,0.1, 0.1,0.1,1); 
     cube(-9.5,-1.5,9, 0.1,1.5,0.05,0,4,0.1, 0.1,0.1,1);
     cube(-6.5,-1.5,7, 0.1,1.5,0.05,0,4,0.1, 0.1,0.1,1);
     cube(-6.5,-1.5,9, 0.1,1.5,0.05,0,4,0.1, 0.1,0.1,1);
     
     //table chairs
     chairs(-11,5,1);
     chairs(-9,5,1);
     chairs(-7,5,1);
     chairs(-5,5,1);

     chairs(-11,11,0);
     chairs(-9,11,0);
     chairs(-7,11,0);
     chairs(-5,11,0);

     
     seats(-7,0);
     
     glPopMatrix();
     
     //table at sofa place     
     tables(7,3,2,0);
     //sofa 
     cube(6.7,-2,-2,   5,0.6,1.6,  0,911,  0.5f,0.35f,0.05f,1);//buffer parallel with door
     cube(1.5,-2,4.5-1.5,   1.6,0.6,6.6,  0,911,  0.5f,0.35f,0.05f,1);//buffer
     cube(3.5,-0.3,-2-1.05,   3,1.3,0.2,  0,1,0.1,0.3,0.8,1);//back
     cube(0.77,-0.3,2.5,   0.2,1.3,5.7,  0,1,0.1,0.3,0.8,1);//back

     cube(2.7,-0.3,-2.2,       1.5,1.6,0.4,  0,6,  0.6,0.3,0.3,1);//pillow
     cube(2.7+3.3,-0.3,-2.2,   1.5,1.6,0.4,  0,6,  0.6,0.3,0.3,1);//pillow

     
     glPushMatrix();
     glRotatef(90,0,1,0);
     cube(3,2.7,-19,  18,6,0.16,  0, 100,  0.9,0.9,0.9,0.9);//left farthest wall
     glPopMatrix();
     
     windows();
     

     if (!light) return;  //  The floor, ceiling and walls don't cast a shadow
      
      glPushMatrix();
      glRotatef(180,0,1,0);
      cube(2,-3.32,3,17,0.16,18,0,2,0.5, 0.5, 0.5,1);//floor
      glPopMatrix();

      cube(-14+4,2.7,-21,  5+4,6,0.16,0,100,0.9,0.9,0.9,0.9);//back left wall
      
      pictureFrame(-4.25+7,11);
      pictureFrame(-1.4+7,12);
      pictureFrame(1.4+7,13);
      
      walls();

      floor_mat(9.7,13,0);
      floor_mat(3,-19,0);
      floor_mat(-14.2,-11,1);
      floor_mat(-14.2,-6,1);


      //front facet
      cube(-15.45-1.87,2.7,15.1,  3.8-2,6.1,0.16,  0,  100,  0.9,0.9,0.9,0.9);//tall brick
      cube(1.68-2,7.8,15.1,  13.38+2,1,0.16,  0,  100,  0.9,0.9,0.9,0.9);//long brick top
      cube(-1-2-3,-2.45,15.1,    10.65-1,1,0.16,0,100,0.9,0.9,0.9,0.9); //long bottom 
      cube(3.7-0.23,2.7,15.1,  3-0.15,6.1,0.16,  0,  100,  0.9,0.9,0.9,0.9);//tall brick r
      
      cube(8,5.3,15.1,  5,1.75,0.16,  0,  100,  0.9,0.9,0.9,0.9);//door top
      cube(13.95,2.7,15.1,  0.95,6.1,0.16,  0,  100,  0.9,0.9,0.9,0.9);//door right side 
            
      

}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display() {
  
   int id;
   const double len=1.5;
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   //  Erase the window and the depth buffers
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Disable lighting
   glDisable(GL_LIGHTING);
   
   //  Eye position
  if (fp) {
   Cx = +2*dim*Sin(rot); //Ajust the camera vector based on rot
   Cz = -2*dim*Cos(rot);
   gluLookAt(Ex,Ey,Ez, Cx+Ex,Ey,Cz+Ez, 0,1,0); //  Use gluLookAt, y is the up-axis
 }
 else {
    //  Perspective - set eye position
    if (mode)
    {
      //glUseProgram(0);
       double px = -2*dim*Sin(viewth)*Cos(viewph);
       double py = +2*dim        *Sin(viewph);
       double pz = +2*dim*Cos(viewth)*Cos(viewph);
       gluLookAt(px,py,pz , 0,0,0 , 0,Cos(viewph),0);
      //  Project(fov,asp,dim);
      //  glViewport(0,0,Width,Height);
    }
    //  Orthogonal - set world orientation
    else
    {
       glRotatef(viewph,1,0,0);
       glRotatef(viewth,0,1,0);
       //glViewport(0,0,Width,Height);
    }

 }

      glViewport(0,0,Width,Height);

   //  Draw light position as sphere (still no lighting here)
   glColor3f(1,1,1);
   glPushMatrix();
   glTranslated(Lpos[0],Lpos[1],Lpos[2]);
   glutSolidSphere(0.03,10,10);
   glPopMatrix();
   //  Enable shader program
   if (shadows){
   glUseProgram(shader);
   id = glGetUniformLocation(shader,"tex");
   if (id>=0) glUniform1i(id,0);
   id = glGetUniformLocation(shader,"depth");
   if (id>=0) glUniform1i(id,1);

   // Set up the eye plane for projecting the shadow map on the scene
   glActiveTexture(GL_TEXTURE1);
   glTexGendv(GL_S,GL_EYE_PLANE,Svec);
   glTexGendv(GL_T,GL_EYE_PLANE,Tvec);
   glTexGendv(GL_R,GL_EYE_PLANE,Rvec);
   glTexGendv(GL_Q,GL_EYE_PLANE,Qvec);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
   glActiveTexture(GL_TEXTURE0);}

   // Draw objects in the scene (including walls)
   Scene(1);

   //  Disable shader program
   glUseProgram(0);
   
   // Draw windows
   glPushMatrix();
   glScaled(0.9,1,1);
   glTranslated(-2,0,0);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    cube(0,2.7,15,3.55,4,0.05,0,1000,0.5,0.5,0.5,0.5);//middle big window
    cube(7.2,2.7,15,3.5,4,0.05,0,1000,0.5,0.5,0.5,0.5);//right big window
    cube(-7.2,2.7,15,3.5,4,0.05,0,1000,0.5,0.5,0.5,0.5);//left big window
    cube(11.6,1.9,15,0.9,4.9,0.05,0,1000,0.5,0.5,0.5,0.5);//left small window
    cube(17.5,1.9,15,0.9,4.9,0.05,0,1000,0.5,0.5,0.5,0.5);//right small window
    cube(14.5,5.1,15,2,1.5,0.05,0,1000,0.5,0.5,0.5,0.5);//top small window
    
    //back door 
    cube(14.6,0.15,15,1.8,3.225,0.05,0,1000,0.5,0.5,0.5,0.5);//door
   
    glPopMatrix();
    glDisable(GL_BLEND);
   //  Draw axes (white)
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

  glWindowPos2i(5,5);
  if (fp) {
      Print("FP=On View Angle=%d",rot);
      glWindowPos2i(5,25);
   }

  else{
  Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s" ,
    viewth,viewph,dim,fov,mode?"Perpective":"Orthogonal");
}

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  Build Shadow Map
 */
void ShadowMap(void)
{
   double Lmodel[16];  //  Light modelview matrix
   double Lproj[16];   //  Light projection matrix
   double Tproj[16];   //  Texture projection matrix
   double Dim=20.0;     //  Bounding radius of scene
   double Ldist;       //  Distance from light to scene center

   //  Save transforms and modes
   glPushMatrix();
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  No write to color buffer and no smoothing
   glShadeModel(GL_FLAT);
   glColorMask(0,0,0,0);
   // Overcome imprecision
   glEnable(GL_POLYGON_OFFSET_FILL);

   //  Turn off lighting and set light position
   Light(0);

   //  Light distance
   Ldist = sqrt(Lpos[0]*Lpos[0] + Lpos[1]*Lpos[1] + Lpos[2]*Lpos[2]);
   if (Ldist<1.1*Dim) Ldist = 1.1*Dim;

   //  Set perspective view from light position
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(114.6*atan(Dim/Ldist),1,Ldist-Dim,Ldist+Dim);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(Lpos[0],Lpos[1],Lpos[2] , 0,0,0 , 0,1,0);
   //  Size viewport to desired dimensions
   glViewport(0,0,shadowdim,shadowdim);

   // Redirect traffic to the frame buffer
   glBindFramebuffer(GL_FRAMEBUFFER,framebuf);

   // Clear the depth buffer
   glClear(GL_DEPTH_BUFFER_BIT);
   // Draw all objects that can cast a shadow
   Scene(0);

   //  Retrieve light projection and modelview matrices
   glGetDoublev(GL_PROJECTION_MATRIX,Lproj);
   glGetDoublev(GL_MODELVIEW_MATRIX,Lmodel);

   // Set up texture matrix for shadow map projection,
   // which will be rolled into the eye linear
   // texture coordinate generation plane equations
   glLoadIdentity();
   glTranslated(0.5,0.5,0.5);
   glScaled(0.5,0.5,0.5);
   glMultMatrixd(Lproj);
   glMultMatrixd(Lmodel);

   // Retrieve result and transpose to get the s, t, r, and q rows for plane equations
   glGetDoublev(GL_MODELVIEW_MATRIX,Tproj);
   Svec[0] = Tproj[0];    Tvec[0] = Tproj[1];    Rvec[0] = Tproj[2];    Qvec[0] = Tproj[3];
   Svec[1] = Tproj[4];    Tvec[1] = Tproj[5];    Rvec[1] = Tproj[6];    Qvec[1] = Tproj[7];
   Svec[2] = Tproj[8];    Tvec[2] = Tproj[9];    Rvec[2] = Tproj[10];   Qvec[2] = Tproj[11];
   Svec[3] = Tproj[12];   Tvec[3] = Tproj[13];   Rvec[3] = Tproj[14];   Qvec[3] = Tproj[15];

   // Restore normal drawing state
   glShadeModel(GL_SMOOTH);
   glColorMask(1,1,1,1);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glPopAttrib();
   glPopMatrix();
   glBindFramebuffer(GL_FRAMEBUFFER,0);

   //  Check if something went wrong
   ErrCheck("ShadowMap");
}

/*
 *
 */
void InitMap()
{
   unsigned int shadowtex; //  Shadow buffer texture id
   int n;

   //  Make sure multi-textures are supported
   glGetIntegerv(GL_MAX_TEXTURE_UNITS,&n);
   if (n<2) Fatal("Multiple textures not supported\n");

   //  Get maximum texture buffer size
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&shadowdim);
   //  Limit texture size to maximum buffer size
   glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&n);
   if (shadowdim>n) shadowdim = n;
   //  Limit texture size to 2048 for performance
   if (shadowdim>2048) shadowdim = 2048;
   if (shadowdim<512) Fatal("Shadow map dimensions too small %d\n",shadowdim);

   //  Do Shadow textures in MultiTexture 1
   glActiveTexture(GL_TEXTURE1);

   //  Allocate and bind shadow texture
   glGenTextures(1,&shadowtex);
   glBindTexture(GL_TEXTURE_2D,shadowtex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowdim, shadowdim, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   //  Map single depth value to RGBA (this is called intensity)
   glTexParameteri(GL_TEXTURE_2D,GL_DEPTH_TEXTURE_MODE,GL_INTENSITY);

   //  Set texture mapping to clamp and linear interpolation
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

   //  Set automatic texture generation mode to Eye Linear
   glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);

   // Switch back to default textures
   glActiveTexture(GL_TEXTURE0);

   // Attach shadow texture to frame buffer
   glGenFramebuffers(1,&framebuf);
   glBindFramebuffer(GL_FRAMEBUFFER,framebuf);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowtex, 0);
   //  Don't write or read to visible color buffer
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);
   //  Make sure this all worked
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) Fatal("Error setting up frame buffer\n");
   glBindFramebuffer(GL_FRAMEBUFFER,0);

   //  Check if something went wrong
   ErrCheck("InitMap");

   //  Create shadow map
   ShadowMap();
}

/*
 *  GLUT calls this routine when nothing else is going on
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,1440.0);
   //  Update shadow map
   ShadowMap();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
   //  Schedule update
  //if (move) glutTimerFunc(dt,idle,0);
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
  if(!fp) {
    //  Right arrow key - increase angle by 5 degrees
    if (key == GLUT_KEY_RIGHT)
       viewth += 5;
    //  Left arrow key - decrease angle by 5 degrees
    else if (key == GLUT_KEY_LEFT)
       viewth -= 5;
    //  Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP)
       viewph += 5;
    //  Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN)
       viewph -= 5;
    //  Keep angles to +/-360 degrees
     //  PageUp key - increase dim
     else if (key == GLUT_KEY_PAGE_UP && dim>1)
        dim -= 0.1;
     //  PageDown key - decrease dim
     else if (key == GLUT_KEY_PAGE_DOWN)
        dim += 0.1;
     //  Keep angles to +/-360 degrees
     viewth %= 360;
     viewph %= 360;
  }
  //  Update projection
  myProject();
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0'){
      viewth = 15;
      viewph = 20;

    }
    
     else if (ch == 'f' || ch == 'F')
     {
        fp = 1-fp;
     }
     else if(ch=='b'||ch=='B'){
       Ex=-15;
       Ez=-18;
       rot=-180;
       shadows=1-shadows;
     }
     else if (ch==' '){
     shadows=1-shadows;}
     if (fp) {
        double dt = 0.05;
        if (ch == '0'){
           Ey = 1.5;
           Ex = 13;
           Ez= 24;
           rot= -15;

         }
        else if (ch == 'w' || ch == 'W'){
           Ex += Cx*dt; //Update the eye vector based on the camera vector
           Ez += Cz*dt;
        }
        else if (ch == 'a' || ch == 'A'){
           rot -= 5;
        }
        else if (ch == 's' || ch == 'S'){
           Ex -= Cx*dt;
           Ez -= Cz*dt;
        }
        else if (ch == 'd' || ch == 'D'){
           rot += 5;
        }

        //  Keep angles to +/-360 degrees
        rot %= 360;
     }
     else {
        //  Reset view angle
        if (ch == '0'){
          viewth=15;
          viewph=20;
        }
        //  Switch display mode
        else if (ch == 'm' || ch == 'M')
           mode = 1-mode;
           //  Change field of view angle
           else if (ch == '-' && ch>1){
              fov--;}
           else if (ch == '+' && ch<179) {
              fov++;}


     }
   //  Update screen size when mode changes
  //  if (ch == 'm' || ch == 'M') glutReshapeWindow(mode?2*Width:Width/2,Height);
   //  Update shadow map if light position or objects changed
   if (strchr("<>oO-+[]",ch)) ShadowMap();
   //  Animate if requested
  glutIdleFunc(move?NULL:NULL);
   //  Update projection
   myProject();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Store window dimensions
   Width  = width;
   Height = height;
   //  Set the viewport to the entire window
   glViewport(0,0, Width,Height);

   //  Set projection
   myProject();
}

//
//  Read text file
//
static char* ReadText(const char *file)
{
   int   n;
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

//
//  Print Shader Log
//
static void PrintShaderLog(int obj,const char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

//
//  Print Program Log
//
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

//
//  Create Shader
//
void CreateShader(int prog,const GLenum type,const char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Attach to shader program
   glAttachShader(prog,shader);
}

//
//  Create Shader Program
//
int CreateShaderProg(const char* VertFile,const char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   if (VertFile) CreateShader(prog,GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   if (FragFile) CreateShader(prog,GL_FRAGMENT_SHADER,FragFile);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Final Project");
   #ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
   if (!GLEW_VERSION_2_0) Fatal("OpenGL 2.0 not supported\n");
   #endif
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(NULL);

   //  Load textures
   tex2d[0] = LoadTexBMP("whitewall1.bmp");
   tex2d[1] = LoadTexBMP("mattress.bmp");
   tex2d[2] = LoadTexBMP("floor.bmp");
   tex2d[3] = LoadTexBMP("counter.bmp");
   tex2d[4] = LoadTexBMP("metal.bmp");
   tex2d[5] = LoadTexBMP("floormat2.bmp");
   tex2d[6] = LoadTexBMP("buffer.bmp");
   tex2d[7] = LoadTexBMP("wood_panel.bmp");
   tex2d[8] = LoadTexBMP("walk_in.bmp");
   tex2d[9] = LoadTexBMP("white.bmp");
   tex2d[10] = LoadTexBMP("door.bmp");
   tex2d[11] = LoadTexBMP("pic1.bmp");
   tex2d[12] = LoadTexBMP("pic3.bmp");
   tex2d[13] = LoadTexBMP("pic2.bmp");
   tex2d[14] = LoadTexBMP("tvscreen.bmp");
   tex2d[15] = LoadTexBMP("curtain.bmp");

   // Enable Z-buffer
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glPolygonOffset(4,0);
   //  Initialize texture map
   shader = CreateShaderProg("shadow.vert","shadow.frag");
   //  Initialize texture map
   InitMap();
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
