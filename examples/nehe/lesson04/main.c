/* NeHe Lesson 04 */

/* ES 1.0 version adapted from Insanity Design                */
/* http://insanitydesign.com/wp/projects/nehe-android-ports/  */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/RegalGLEW.h>
#include <GL/Regal.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/RegalGLUT.h>
#endif

static void init()
{
  glShadeModel(GL_SMOOTH); 			          // Enable Smooth Shading
  glClearColor(0.0f, 0.0f, 0.0f, 0.5f); 	// Black Background
  glClearDepth(1.0f); 					          // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST); 			          // Enables Depth Testing
  glDepthFunc(GL_LEQUAL); 			          // The Type Of Depth Testing To Do
  glDisable(GL_CULL_FACE);
  
  // Really Nice Perspective Calculations
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 
}

static void reshape(int width, int height)
{
  if (height<1) { 						// Prevent A Divide By Zero By
    height = 1; 						  // Making Height Equal One
  }
  
  glViewport(0, 0, width, height); 	// Reset The Current Viewport
  glMatrixMode(GL_PROJECTION); 	    // Select The Projection Matrix
  glLoadIdentity(); 					      // Reset The Projection Matrix
  
  //Calculate The Aspect Ratio Of The Window
  gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
  
  glMatrixMode(GL_MODELVIEW); 	    // Select The Modelview Matrix
  glLoadIdentity(); 					      // Reset The Modelview Matrix
}

static void drawSquare()
{		
	/** The initial vertex definition */
	const float vertices[] = { 
						-1.0f, -1.0f, 0.0f, 	// Bottom Left
						 1.0f, -1.0f, 0.0f, 	// Bottom Right
						-1.0f,  1.0f, 0.0f,   // Top Left
						 1.0f,  1.0f, 0.0f 		// Top Right
										};

	const float colors[] = {
    					0.0f, 0.0f, 1.0f, 1.0f, // Blue
    					0.0f, 0.0f, 1.0f, 1.0f, // Blue
    					0.0f, 0.0f, 1.0f, 1.0f 	// Blue
			    							};

		// Set the face rotation
		glFrontFace(GL_CCW);
		
		// Point to our vertex buffer
		glVertexPointer(3, GL_FLOAT, 0, vertices);
//  glColorPointer(4, GL_FLOAT, 0, colors);

#if 1
		// Set The Color To Blue
		glColor4f(0.5f, 0.5f, 1.0f, 1.0f);	
#endif

		// Enable vertex buffer
		glEnableClientState(GL_VERTEX_ARRAY);
//  glEnableClientState(GL_COLOR_ARRAY);
		
		// Draw the vertices as triangle strip
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		// Disable the client state before leaving
		glDisableClientState(GL_VERTEX_ARRAY);	
//  glDisableClientState(GL_COLOR_ARRAY);		
}

static void drawTriangle()
{
	/** The initial vertex definition */
	const float vertices[] = { 
						0.0f, 1.0f, 0.0f, 	//Top
						-1.0f, -1.0f, 0.0f, //Bottom Left
						1.0f, -1.0f, 0.0f 	//Bottom Right
										};
	/** The initial color definition */
	const float colors[] = {
    					1.0f, 0.0f, 0.0f, 1.0f, // Set The Color To Red with full luminance
    					0.0f, 1.0f, 0.0f, 1.0f, // Set The Color To Green with full luminance
    					0.0f, 0.0f, 1.0f, 1.0f 	// Set The Color To Blue with full luminance
			    							};
			    							
  //Set the face rotation
  glFrontFace(GL_CW);
	
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

  //Point to our buffers
  glVertexPointer(3, GL_FLOAT, 0, vertices);
  glColorPointer(4, GL_FLOAT, 0, colors);
  
  //Enable the vertex and color state
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  
  //Draw the vertices as triangles
  glDrawArrays(GL_TRIANGLES, 0, 3);
		
  //Disable the client state before leaving
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);		
}

float rquad = 0.0f;
float rtri = 0.0f;

static void display(void)
{
  // Clear Screen And Depth Buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
  
  // Reset The Current Modelview Matrix
  
  glLoadIdentity();					
  
  // Drawing

  glTranslatef(0.0f, -1.2f, -6.0f);	  // Move down 1.0 Unit And Into The Screen 6.0
  glRotatef(rquad, 1.0f, 0.0f, 0.0f);	// Rotate The Square On The X axis ( NEW )
  drawSquare();	  					          // Draw the square
  
  // Reset The Current Modelview Matrix

  glLoadIdentity(); 					
  
  glTranslatef(0.0f, 1.3f, -6.0f);		// Move up 1.3 Units and -6.0 into the Screen
  glRotatef(rtri, 0.0f, 1.0f, 0.0f);	// Rotate The Triangle On The Y axis ( NEW )
  drawTriangle();	     					      // Draw the triangle		
  
  // Rotation
  rtri += 0.2f; 							// Increase The Rotation Variable For The Triangle ( NEW )
  rquad -= 0.15f; 						// Decrease The Rotation Variable For The Quad ( NEW )
  
  glutSwapBuffers();
}

static void idle(void)
{
  glutPostRedisplay();
}

int main(int argc, char *argv[])
{
  int status;

  glutInitWindowSize(512,512);
  glutInit(&argc, argv);
  
  glutCreateWindow("NeHe Lesson04 for ES 1.0");

  // Regal workaround for OSX GLUT

  #ifdef __APPLE__
  extern void *CGLGetCurrentContext(void);
  RegalMakeCurrent(CGLGetCurrentContext());
  #endif

  status = glewInit();
  if (status != GLEW_OK)
  {
    printf("OpenGL Extension Wrangler (GLEW) failed to initialize");
    return EXIT_FAILURE;
  }

  printf("vendor: %s\n", glGetString(GL_VENDOR));
  printf("version: %s\n", glGetString(GL_VERSION));
  printf("renderer: %s\n", glGetString(GL_RENDERER));

  if (!glewIsSupported("GL_REGAL_ES1_0_compatibility"))
  {
    printf("GL_REGAL_ES1_0_compatibility is not supported.\n");
    return EXIT_FAILURE;
  }

  if (!glewIsSupported("GL_REGAL_ES1_1_compatibility"))
  {
    printf("GL_REGAL_ES1_1_compatibility is not supported.\n");
    return EXIT_FAILURE;
  }

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);

  init();

  glutMainLoop();

  return EXIT_SUCCESS;
}
