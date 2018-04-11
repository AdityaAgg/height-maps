/*
  CSCI 420 Computer Graphics, USC
  Assignment 1: Height Fields
  C++ starter code
  Summary: Extension of Height fields starter code to take an image and generate a height field. Check README.md for more details
  Student username: agga140@usc.edu
*/

#include <iostream>
#include <cstring>
#include "openGLHeader.h"
#include "glutHeader.h"

#include "imageIO.h"
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"
#include <vector>
#include <array>
#include <time.h>
#include <sstream>
#include <string>

#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#ifdef WIN32
  char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
  char shaderBasePath[1024] = "../openGLHelper-starterCode";
#endif

  using namespace std;

int mousePos[2]; // x,y coordinate of the mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

// state of the world
float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { 0.0f, 0.0f, 0.0f };
float landScale[3] = { 1.0f, 1.0f, 1.0f };

int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework I";
int screenshot_counter = 118;

enum RenderType{ points, wireframe, triangles};
RenderType global_render_type = triangles;

ImageIO * heightmapImage;

//declared variables added by Aditya Aggarwal (as part of assignment)
BasicPipelineProgram * pipelineProgram = new BasicPipelineProgram(); //pipeline program

GLuint vao; //vao for vertices
GLuint vao_lines; //vao for lines

GLuint buffer; //buffer for vertices
GLuint buffer_two; //buffer for lines

OpenGLMatrix * openGLMatrix = new OpenGLMatrix(); //matrix class used for projection and model view matrices

//timer for animation images
time_t early_timer;
time_t late_timer;



vector<array<float, 3> > vertices;
vector<array<float, 3> > vertices_lines;

//color not it's own vector because managed by shader


//self defined functions (rather than callback functions) - at the top so they can be recognized
void renderTriangle(){
  




  
  //draw
    GLint first = 0;
    int width = heightmapImage->getWidth();
    GLsizei numberOfVerticesPerRender = 2*width;

  
    if(global_render_type==points){ //points
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, first, vertices.size());
    } else if (global_render_type == wireframe) { //wireframe
        glBindVertexArray(vao_lines);
        glDrawArrays(GL_LINES, first ,vertices_lines.size());
    } else { //triangle strip as default
        glBindVertexArray(vao);
        for(int i=0; i<width-1; ++i) {
            glDrawArrays(GL_TRIANGLE_STRIP, (i*numberOfVerticesPerRender), numberOfVerticesPerRender);
        }
    }
    
  glutSwapBuffers();
  glBindVertexArray(0); //unbind vao
}


void createHeightField() {




    int width = heightmapImage->getWidth();
    int height = heightmapImage->getHeight();
    float res_factor = 256.0/(width+1);
    
    for(int i=0; i<width-1; ++i) { // (width - 1) * (height - 1) rendering units (squares)
        for(int j=0; j<height-1; ++j) {
            
            
           
            //define four vertices in redering unit
            char pixel = heightmapImage->getPixel(i, j, 0);
            float pixel_float = pixel + 128.0;
            float pixel_height = 0.2* pixel;
            array<float, 3> vertex = {res_factor*i -128, pixel_height, res_factor*(-j) +128};
            
            char pixel_two = heightmapImage->getPixel(i+1, j, 0);
            float pixel_two_float = pixel_two+128.0;
            float pixel_two_height = 0.2*pixel_two;
            array<float, 3> vertex_two = {res_factor*(i+1) -128, (pixel_two_height), res_factor*(-j) + 128};
            
            char pixel_three = heightmapImage->getPixel(i, j+1, 0);
            float pixel_three_float = pixel_three+128.0;
            float pixel_three_height = 0.2*pixel_three;
            array<float, 3> vertex_three = {res_factor*(i) -128, (pixel_three_height), res_factor*(-j-1) +128};
            
            char pixel_four = heightmapImage->getPixel(i+1, j+1, 0);
            float pixel_four_float = pixel_four+128.0;
            float pixel_four_height = 0.2*pixel_four;
            array<float, 3> vertex_four = {res_factor*(i+1)-128, (pixel_four_height), res_factor*(-j-1) +128};
            
            
            if(j==0)
                vertices.push_back(vertex); //for triangles and points
            
            
            //add vertices to vertex vector for triangles and points
            vertices.push_back(vertex_three);
            vertices.push_back(vertex_two);
            
            
            
            //for lines
            vertices_lines.push_back(vertex);
            vertices_lines.push_back(vertex_two);
            vertices_lines.push_back(vertex_two);
            vertices_lines.push_back(vertex_four);
            vertices_lines.push_back(vertex_four);
            vertices_lines.push_back(vertex_three);
            vertices_lines.push_back(vertex_three);
            vertices_lines.push_back(vertex);
            
            
            //i and j alternating mods for wireframe pattern
            if(i%2 ==0) {
            if(j%2 ==0) {
                vertices_lines.push_back(vertex);
                vertices_lines.push_back(vertex_four);
            } else {
                vertices_lines.push_back(vertex_two);
                vertices_lines.push_back(vertex_three);
            }
            } else {
                if(j%2 ==1) {
                    vertices_lines.push_back(vertex);
                    vertices_lines.push_back(vertex_four);
                } else {
                    vertices_lines.push_back(vertex_two);
                    vertices_lines.push_back(vertex_three);
                }
            }
            
            
            
            //for triangles and points at the end of a row
            if(j==height-2) {
                vertices.push_back(vertex_four);
            }
            
            
            
            
            
            
            
            
            
            
            
     

            
        }
    }

    
}

void initVBO(){
  
    //buffer for triangles and points
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()  * 3 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    //buffer for lines
    glGenBuffers(1, &buffer_two);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_two);
    glBufferData(GL_ARRAY_BUFFER, vertices_lines.size()  * 3 * sizeof(float), &vertices_lines[0], GL_STATIC_DRAW);
  

}

// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
    unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
    glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

    ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

    if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
      cout << "File " << filename << " saved successfully." << endl;
    else
        cout << "Failed to save file " << filename << '.' << endl;

    delete [] screenshotData;
}


void displayFunc()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //z - buffer
    

    
    

    //model view
    openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);
    float m[16];
    openGLMatrix->GetMatrix(m);
    pipelineProgram->SetModelViewMatrix(m);
    

    //projection view
    openGLMatrix->SetMatrixMode(OpenGLMatrix::Projection);
    float p[16]; // column-major
    openGLMatrix->GetMatrix(p);
    pipelineProgram->SetProjectionMatrix(p);


    pipelineProgram->Bind(); // bind the pipeline program


    renderTriangle(); //render the triangle
}




void bindProgram(){
    
    
    GLuint program = pipelineProgram->GetProgramHandle();
    
    //create VAO one
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glGenVertexArrays(1, &vao);
    
    glBindVertexArray(vao);

    
    //bind layout locations from the shader
    GLuint loc = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc);
    const void * offset = (const void*) 0;
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, offset);
    
    
    
    //create and bind VAO two
    glBindBuffer(GL_ARRAY_BUFFER, buffer_two);
    glGenVertexArrays(1, &vao_lines);
    glBindVertexArray(vao_lines);
    
    GLuint loc2 = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, offset);
 

}




void idleFunc()
{
  
    openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);
    openGLMatrix->Rotate(0.01, 0, 0, 1); //default animation for something moving/rather than completely static

    
    
    
    
    //my own screen shot code for creating an animation -- commented out
    
    //time(&late_timer); //timer for frame rate when taking screenshots

    /*if(difftime(late_timer, early_timer)>=(1.0/720.0) && screenshot_counter<300){
        if(screenshot_counter<10) {
            std::stringstream ss;
            ss << "00" << screenshot_counter << ".jpg";
            saveScreenshot(ss.str().c_str());
        } else if (screenshot_counter<100) {
            std::stringstream ss;
            ss << "0" << screenshot_counter << ".jpg";
            saveScreenshot(ss.str().c_str());
        } else {
            std::stringstream ss;
            ss << screenshot_counter << ".jpg";
            saveScreenshot(ss.str().c_str());
        }
        time(&early_timer);
        ++screenshot_counter;
    }*/


  // make the screen update
  glutPostRedisplay();
}

void reshapeFunc(int w, int h)
{

 
  // setup perspective matrix
  glViewport(0, 0, w, h);
 
  openGLMatrix->SetMatrixMode(OpenGLMatrix::Projection);
  openGLMatrix->LoadIdentity();

  
  

  float aspectRatio = 16.0/9;
  openGLMatrix->Perspective(45.0, aspectRatio, 0.01, 5000);
  
    
  //switch back to model view for safety
  openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);


  
}



void mouseMotionDragFunc(int x, int y)
{
  // mouse has moved and one of the mouse buttons is pressed (dragging)

  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

  switch (controlState)
  {
    // translate the landscape
    case TRANSLATE:
    if (leftMouseButton)
    {
        // control x,y translation via the left mouse button
      landTranslate[0] += mousePosDelta[0];
      landTranslate[1] -= mousePosDelta[1];
    }
    if (middleMouseButton)
    {
        // control z translation via the middle mouse button
      landTranslate[2] += mousePosDelta[1];
        
    }
         
    break;

    // rotate the landscape
    case ROTATE:
    if (leftMouseButton)
    {
        // control x,y rotation via the left mouse button
      landRotate[0] += mousePosDelta[1];
      landRotate[1] += mousePosDelta[0];
    }
    if (middleMouseButton)
    {
        // control z rotation via the middle mouse button
      landRotate[2] += mousePosDelta[1];
    }
    break;

    // scale the landscape
    case SCALE:
    if (leftMouseButton)
    {
        // control x,y scaling via the left mouse button
      landScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
      landScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
    }
    if (middleMouseButton)
    {
        // control z scaling via the middle mouse button
      landScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
    }
    break;
  }
    
    
    //rotate
    openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);
    openGLMatrix->Rotate(landRotate[0], 1, 0, 0);
    openGLMatrix->Rotate(landRotate[1], 0, 1, 0);
    openGLMatrix->Rotate(landRotate[2], 0, 0, 1);
    
    //reset
    landRotate[0]=0.0;
    landRotate[1]=0.0;
    landRotate[2]=0.0;
    
    
    //scale
    openGLMatrix->Scale(landScale[0], landScale[1], landScale[2]);
    
    //reset
    landScale[0] = 1.0;
    landScale[1] = 1.0;
    landScale[2] = 1.0;
    
    
    //translate
    openGLMatrix->Translate(landTranslate[0], landTranslate[1], landTranslate[2]);
    
    //reset
    landTranslate[0] =0.0;
    landTranslate[1] = 0.0;
    landTranslate[2] = 0.0;
    

    

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
  // mouse has moved
  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
  // a mouse button has has been pressed or depressed

  // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
    leftMouseButton = (state == GLUT_DOWN);
  
    break;

    case GLUT_MIDDLE_BUTTON:
    middleMouseButton = (state == GLUT_DOWN);
 
    break;

    case GLUT_RIGHT_BUTTON:
    rightMouseButton = (state == GLUT_DOWN);
  
    break;
  }

 
  // keep track of whether CTRL and SHIFT keys are pressed
    switch (glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
    controlState = TRANSLATE;
    break;
          
    case GLUT_ACTIVE_SHIFT:
    controlState = SCALE;
    break;

    // if CTRL and SHIFT are not pressed, we are in rotate mode
    default:
    controlState = ROTATE;
    break;
  }
    
    

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void keyboardFunc(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
      break;

      case ' ':
      cout << "You pressed the spacebar." << endl;
      break;

      case 'x':
      // take a screenshot
      saveScreenshot("screenshot.jpg");
      break;
          
      case '0':
          global_render_type = points;
          break;
      case '1':
          global_render_type = wireframe;
          break;
      case '2':
          global_render_type = triangles;
          break;
        
          
          
    }
  }

  void initPipelineProgram() {
    //compiles pipeline program
    pipelineProgram = new BasicPipelineProgram();
    pipelineProgram->Init(shaderBasePath);
  }

void initScene(int argc, char *argv[])
{
  // load the image from a jpeg disk file to main memory
  heightmapImage = new ImageIO();
  if (heightmapImage->loadJPEG(argv[1]) != ImageIO::OK)
  {
    cout << "Error reading image " << argv[1] << "." << endl;
    exit(EXIT_FAILURE);
  }



  createHeightField(); //create the heigh field
    
    //calculate z translation for camera -- code from assignment 1 milestone
    /*const double uscConstant = 4483470008;
    double maxVal = 10000000000;
    float zTranslation = 3 + uscConstant/(maxVal);*/
    
  openGLMatrix->LoadIdentity();
  //translate camera
  openGLMatrix->LookAt(0, 256 , 256, 0, 0 , 0 , 0.0, 1, 0.0);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  initVBO();
  
  initPipelineProgram(); //for shader compiling

  bindProgram(); //for VAOs binding to pipeline program

  //time(&early_timer); -- starting timer for animation frames
}




int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cout << "The arguments are incorrect." << endl;
    cout << "usage: ./hw1 <heightmap file>" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
  glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #else
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);  
  glutCreateWindow(windowTitle);

  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  // tells glut to use a particular display function to redraw 
  glutDisplayFunc(displayFunc);
  // perform animation inside idleFunc
  glutIdleFunc(idleFunc);
  // callback for mouse drags
  glutMotionFunc(mouseMotionDragFunc);
  // callback for idle mouse movement
  glutPassiveMotionFunc(mouseMotionFunc);
  // callback for mouse button changes
  glutMouseFunc(mouseButtonFunc);
  // callback for resizing the window
  glutReshapeFunc(reshapeFunc);
  // callback for pressing the keys on the keyboard
  glutKeyboardFunc(keyboardFunc);

  // init glew
  #ifdef __APPLE__
    // nothing is needed on Apple
  #else
    // Windows, Linux
  GLint result = glewInit();
  if (result != GLEW_OK)
  {
    cout << "error: " << glewGetErrorString(result) << endl;
    exit(EXIT_FAILURE);
  }
  #endif

  // do initialization
  initScene(argc, argv);

  // sink forever into the glut loop
  glutMainLoop();
}


