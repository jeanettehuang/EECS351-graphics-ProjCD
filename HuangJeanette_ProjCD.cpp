//
// EECS 351-1 Project CD: An Animated Robot, Sun, + Bush with Programmable Shaders
// Winter 2012
// Jeanette Huang, netid:jyh286
//

#include "HuangJeanette_ProjCD.h"

using namespace std;

//=====================
// GLOBAL VARIABLES: bad idea, but forced by GLUT's callback fcns.
//                   Do your best to keep the number of global vars low; e.g.
//                   Make a 'CTheApp' class to hold them all, and make just
//                   one global CTheApp object  (e.g. 'myApp').
//=====================
static int nu_display_width  = 640;	// global variables that hold current
static int nu_display_height = 640;	// display window position and size;
static int nu_display_xpos = 200;	// See getDisplayXXX() functions.
static int nu_display_ypos = 100;
static int nu_Anim_isOn = 1;        // ==1 to run animation, ==0 to pause.
// !DON'T MESS WITH nu_Anim_isOn; call runAnimTimer().
//========================
// Create global vars for our application:
GLdouble xclik=0.0, yclik=0.0;  // mouse button down position, in pixels
GLdouble xtheta=5.0, ytheta=5.0;// mouse-driven view-rotation angles in degrees.

int isCleared = 1;                  // toggle animation's screen-clear
int isSolid = 0;                    // toggle solid/wireframe drawing.
int isDistortion = 0;

// Boolean values for turning world and headlights off
int isHeadLight = 1;
int isWorldLight = 0;

// Variables for distortion
float xs = 0.0, zs = 0.0, progtime = 0.0;
int xs_loc = 0, zs_loc = 0, timer = 0;

// Animated rotational angles
GLdouble    theta1=0.0,
            theta2=-35.0,
            theta3=0.0,
            theta4=0.0,
            theta5=0.0,
            theta6=0.0,
            theta7=0.0;
GLdouble    thetaStep1=0.05,
            thetaStep2=0.25,
            thetaStep3=0.25,
            thetaStep4=0.15,
            thetaStep5=0.15,
            thetaStep6=0.15,
            thetaStep7=0.00;

// Variables to allow user to move camera
GLdouble    xPos = 0.0,
            yPos = 0.0,
            zPos = 0.0,
            xPosTilt = 0.0,
            yPosTilt = 0.0,
            zPosTilt = 0.0;

// Variables to allow user to adjust position of world light
GLdouble    worldLightL = 0.0,
            worldLightR = 0.0;

// Variable to allow user to adjust speed of distortion
GLdouble    changeDistortSpeed = 0.0;

int start_l = 0, end_l = 1;

GLUquadricObj *pQuad0;		//ptr to the openGL quadric object(s) we'll draw
CMatl  stuff[9];            // Three material-describing objects.
CLight lamps[1];            // Two light source objects.
CLight lamps2[1];

CProgGLSL *d_myGLSL;
CProgGLSL *p_myGLSL;                // pointer to our one 'GLSL program' object


// VALUES TO SEND/RECEIVE FROM OUR GLSL PROGRAMMABLE SHADERS:
//-------------------------------------------------------------
GLdouble r_app=0.5, g_app=0.5, b_app = 0.5;   // rgb/RGB keys adjust up/down.

// We will copy r_app, g_app, and b_app values to the GPU-resident programmable
// shader's 'uniform' variables named r_shad, g_shad, b_shad.
// As explained in 'Beginning OpenGL Game Programming', Chap 6, pg 145-6,
// transferring data from our application to 'uniform' variables on the GPU
// takes several steps.  When we load, compile, and then 'link' our shader
// programs, the GLSL linker decides where to store each 'uniform'  variable in
// the GPU's memory.  To access any uniform, you must first obtain its location
// ID# by calling
//      GLuint glGetUniformLocation(GLuint programID, const GLchar* nameString);
//              (progID? get it from CProgGLSL::getProgramID().
//              (nameString? shader uniform variable name , e.g. "r_shad".
// Then use this location ID# to transfer data to/from the shader variable with
// one of the many 'glUniform()' functions (see page 146).
//

GLuint r_shadID, g_shadID, b_shadID; // the GPU's location-ID# for the shader

// NOTE: getting 'uniform' locations can be very slow!
// Do it just once and keep the result for re-use.  I recommend getting ALL
// the uniform variable location ID#s right after you load,compile,link, and
// begin use of your shaders.  In this starter code, we get them all at the
// end of main(), just before we (irreversibly) enter glutMainLoop().



//==============================================================================
int main( int argc, char *argv[] )
//==============================================================================
{
    //--GLUT/openGL Setup-------------------
    glutInit( &argc, argv );	    // init GLUT user-interface library;
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH  );
    // set rendering mode to RGB (not RGBA) color,
    // single-buffering GLUT_SINGLE, or
    // double-buffering GLUT_DOUBLE, where you'd need to call
    // glutSwapBuffers() after drawing operations to show
    // on-screen what you draw)
    //  enable depth testing for 3D drawing ('Z-buffering')
    glutInitWindowSize(getDisplayWidth(), getDisplayHeight() );
    glutInitWindowPosition(getDisplayXpos(),getDisplayYpos() );
    // set display window size & position from global vars
    glutCreateWindow( "EECS 351-1 Project C: An Animated Robot, Sun, + Bush with Programmable Shaders" ); // set window title-bar name
    // And finally (must be AFTER the glutCreateWindow() call):
    glEnable( GL_DEPTH_TEST );			// enable hidden surface removal
// STUDENTS: what happens if you disable GL_DEPTH_TEST?
//	And what happens if display() callback clears the color buffer,
// 	but not the depth buffer? (e.g. what happens if you change
//  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); to
//  glClear( GL_COLOR_BUFFER_BIT)'; ?)

    glClearColor( 0, 0, 0, 1 );	    // 'cleared-screen' color: black (0,0,0)
    // Register our callback functions in GLUT:
    glutDisplayFunc( myDisplay );	// callback for display window redraw
    glutReshapeFunc( myReshape);    // callback to create or re-size window
    glutVisibilityFunc(myHidden);   // callback for display window cover/uncover
    glutKeyboardFunc( myKeyboard);  // callback for alpha-numeric keystroke
    glutSpecialFunc ( myKeySpecial);// callback for all others: arrow keys,etc.
    glutMouseFunc( myMouseClik );   // callback for mouse button events
    glutMotionFunc( myMouseMove );  // callback for mouse dragging events

    glClearColor(0.2, 0.2, 0.2, 0.0);	// Display-screen-clearing color;
    // acts as 'background color'

    pQuad0 = gluNewQuadric();	        // create a quadric object

    //***Create our materials.

    // Robot materials
    stuff[0].createMatl(MATL_SILVER_DULL);
    stuff[0].isFaceted = false;
    stuff[0].isTwoSided = false;

    stuff[1].createMatl(MATL_BLACK_PLASTIC);
    stuff[1].isFaceted = false;
    stuff[1].isTwoSided = false;

    stuff[2].createMatl(MATL_WHITE);
    stuff[2].isFaceted = false;
    stuff[2].isTwoSided = false;

    stuff[3].createMatl(MATL_RED_PLASTIC);
    stuff[3].isFaceted = false;
    stuff[3].isTwoSided = false;

    stuff[4].createMatl(MATL_OBSIDIAN);
    stuff[4].isFaceted = false;
    stuff[4].isTwoSided = false;

    // Tree materials
    stuff[5].createMatl(MATL_EMERALD);
    stuff[5].isFaceted = false;
    stuff[5].isTwoSided = false;

    stuff[6].createMatl(MATL_COPPER_DULL);
    stuff[6].isFaceted = false;
    stuff[6].isTwoSided = false;

    // Sun materials
    stuff[7].createMatl(MATL_GOLD_DULL);
    stuff[7].isFaceted = false;
    stuff[7].isTwoSided = false;

    stuff[8].createMatl(MATL_ORANGE);
    stuff[8].isFaceted = false;
    stuff[8].isTwoSided = false;

    // Create our light sources
    lamps[0].createLamp(LAMP_POINT_KEY, GL_LIGHT0);
    lamps2[0].createLamp(LAMP_POINT_KEY, GL_LIGHT0);


    #if !defined(__APPLE__)
        glewInit();                                 // if we use GLEW (Apple won't).
        // we must start its library;
    #endif

//===================DON'T TRY ANY GLSL WORK ABOVE THIS LINE====================

    // CAREFUL! You MUST complete all the GLUT calls that establish your openGl
    // 'rendering context' BEFORE you begin any GLSL-related activities.
    //
    // In other words, be sure your program has completed glutCreateWindow(),
    // that you've registered your callback functions, and started any OpenGL
    // extension helpers you may need, such as calling glewInit().
    //==============Create GLSL programmable shaders============================
    // Always AFTER 'glutCreateWindow()' because some GLSL commands rely on the
    // openGL 'rendering context' (all the state variables that tie openGL to
    // your particular OS and graphics card). The glutCreateWindow() call
    // forces creation of that 'rendering context' we need.

    #if !defined(__APPLE__)
        glewInit();                                 // if we use GLEW (Apple won't).
        // we must start its library;
    #endif

    // Programmable shaders (normal)
    p_myGLSL = new CProgGLSL(argv[0],   "normal_shader.vsh",
                                        "normal_shader.fsh");
    p_myGLSL->loadShaders();    // read in the shader files' contents
    p_myGLSL->compileProgram(); // compile and link the program for the GPU,

    // Programmable shaders (distorted)
    d_myGLSL = new CProgGLSL(argv[0],   "distort_shader.vsh",
                                        "normal_shader.fsh");
    d_myGLSL->loadShaders();    // read in the shader files' contents
    d_myGLSL->compileProgram(); // compile and link the program for the GPU,

    // Get GLSL uniforms
    start_l = glGetUniformLocation(p_myGLSL->getProgramID(), "start_l");
    end_l = glGetUniformLocation(p_myGLSL->getProgramID(), "end_l");

    glUniform1i(start_l, 0);
    glUniform1i(end_l, 1);

// =============================================================================
// Next, a rare and wonderful bit of help on debugging your shaders, found here:
//http://stackoverflow.com/questions/4783912/how-can-i-find-a-list-of-all-the-uniforms-in-opengl-es-2-0-vertex-shader-pro
//
// This code snippet asks the server-side (graphics card) to report the number
// of active 'uniform' vars it is using.  ONLY THESE will successfully take your
// the values you set on client-side (CPU, CodeBlocks project) into your vertex
// and fragment shader programs on the server-side (GPU, graphics card)
//
// WHY DID MY 'UNIFORM' VARS VANISH!?
// Some graphics card manufacturers (NVidia, etc) apply AGGRESIVE optimization
// in their GLSL compilers, which will discard any uniforms from your program
// that it decides will not affect your GLSL program's on-screen result.  If you
// create a uniform, make sure it DOES SOMETHING in your vertex and/or fragment
// shader: changes a vertex color, position, normal, etc, or it will disappear!
//

    if (isDistortion) {
        int total = -1;             // count the number of 'uniforms' received by our shader.
        glGetProgramiv( d_myGLSL->getProgramID(), GL_ACTIVE_UNIFORMS, &total );
        cout << endl << "Server side (graphics card) reports these ";
        cout << total << " uniforms defined" <<endl;
        for(int i=0; i<total; ++i)  //  print them on-screen.
        {
            int name_len=-1, num=-1;
            GLenum type = GL_ZERO;
            char name[100];
            glGetActiveUniform( d_myGLSL->getProgramID(), GLuint(i), sizeof(name)-1, &name_len, &num, &type, name );
            name[name_len] = 0;
            GLuint location = glGetUniformLocation( d_myGLSL->getProgramID(), name );
            cout << name << "@" << location << endl;
        }

        //- FIND GLSL uniforms------------------------------------------------------
        // Next, obtain the location ID# for each and every 'uniform' variable in
        // the in the vertex and fragment shaders we just loaded.
        r_shadID = glGetUniformLocation(d_myGLSL->getProgramID(), "r_shad");
        cout << "your r_shadID is: " << r_shadID << endl;
        g_shadID = glGetUniformLocation(d_myGLSL->getProgramID(), "g_shad");
        cout << "your g_shadID is: " << g_shadID << endl;
        b_shadID = glGetUniformLocation(d_myGLSL->getProgramID(), "b_shad");
        cout << "your b_shadID is: " << b_shadID << endl;
        //-INIT GLSL uniforms-------------------------------------------------------
        // Next, set initial values for all your uniforms. You can use these same
        // commands elsewhere to change the values of these uniforms.
        glUniform1f(r_shadID, 0.5);
        glUniform1f(g_shadID, 0.5);
        glUniform1f(b_shadID, 0.5);
        //--------------------------------------------------------------------------
        runAnimTimer(1);                // start our animation loop.
        glutMainLoop();	                // enter GLUT's event-handler; NEVER EXITS.

        delete d_myGLSL;                // orderly exit; delete the object we made.
    }
    else {
        int total = -1;             // count the number of 'uniforms' received by our shader.
        glGetProgramiv( p_myGLSL->getProgramID(), GL_ACTIVE_UNIFORMS, &total );
        cout << endl << "Server side (graphics card) reports these ";
        cout << total << " uniforms defined" <<endl;
        for(int i=0; i<total; ++i)  //  print them on-screen.
        {
            int name_len=-1, num=-1;
            GLenum type = GL_ZERO;
            char name[100];
            glGetActiveUniform( p_myGLSL->getProgramID(), GLuint(i), sizeof(name)-1, &name_len, &num, &type, name );
            name[name_len] = 0;
            GLuint location = glGetUniformLocation( p_myGLSL->getProgramID(), name );
            cout << name << "@" << location << endl;
        }

        //- FIND GLSL uniforms------------------------------------------------------
        // Next, obtain the location ID# for each and every 'uniform' variable in
        // the in the vertex and fragment shaders we just loaded.
        r_shadID = glGetUniformLocation(p_myGLSL->getProgramID(), "r_shad");
        cout << "your r_shadID is: " << r_shadID << endl;
        g_shadID = glGetUniformLocation(p_myGLSL->getProgramID(), "g_shad");
        cout << "your g_shadID is: " << g_shadID << endl;
        b_shadID = glGetUniformLocation(p_myGLSL->getProgramID(), "b_shad");
        cout << "your b_shadID is: " << b_shadID << endl;
        //-INIT GLSL uniforms-------------------------------------------------------
        // Next, set initial values for all your uniforms. You can use these same
        // commands elsewhere to change the values of these uniforms.
        glUniform1f(r_shadID, 0.5);
        glUniform1f(g_shadID, 0.5);
        glUniform1f(b_shadID, 0.5);
        //--------------------------------------------------------------------------
        runAnimTimer(1);                // start our animation loop.
        glutMainLoop();	                // enter GLUT's event-handler; NEVER EXITS.

        delete p_myGLSL;                // orderly exit; delete the object we made.
    }
}


void myReshape( int width, int height )
//------------------------------------------------------------------------------
{
// Called by GLUT on initial creation of the window, using the width,height
// arguments given in glutInitWindowSize(); see main() fcn. ALSO called by GLUT
// whenever you re-size the display window to a different width, height.
    nu_display_width    = width;    // save the current display size
    nu_display_height   = height;   // in these global vars.

//-------------------- glViewport() usage:
//
//   ?What coordinates glVertex() calls will actually show up on-screen?
//
// BACKGROUND:
// OpenGL transforms glVertex() three times before they reach the screen, first
//  by the GL_MODELVIEW matrix (used to build 3D shapes and place your camera),
//  then the GL_PROJECTION matrix (used to make a 2D image from 3D vertices),
//  then by the 'Viewport' matrix (used to fit the 2D image onto the screen).
//  !CHAOS!: ALL THREE matrices change a glVertex call's on-screen location,
//          and the windowing system may change display window size & location.
//
//      What goes where? How do we start?
//
// Fortunately, we can always rely on the CVV ('canonical view volume') as a
// our reference, our anchor for all the coordinate systems we chain together
// with these 3 matrices.
//
// The 'Canonical View Volume' is a simple axis-centered cube with corners at
// (+/-1, +/-1, +/-1), defined at the output of the GL_PROJECTION matrix.
// OpenGL 'clips' to this volume; it simply discards any part of any drawing
// primitive that comes out of the GL_PROJECTION matrix with values outside
// this CVV cube.  Any openGL drawing you see passed through this CVV.
//
// Initially, GL_MODELVIEW and GL_PROJECTION both hold 4x4 indentity matrices,
// and (thanks to GLUT) the Viewport fills your display window with the CVV.
// If your glVertex() calls specify (x,y,z) values within (+/-1, +/-1, +/-1)
// (inside the CVV) you can make on-screen pictures with them.  If you mess up,
// you can always return to this state by re-initializing the 3 matrices:
//          glMatrixMode(GL_MODELVIEW); // select and
//          glLoadIdentity();           // set to identity matrix.
//          glMatrixMode(GL_PROJECTION);// select and
//          glLoadIdentity();           // set to identity matrix.
//          glViewport(0,0,screenWidth,screenHeight); // fill screen with CVV.
//
// The glViewport() command specifies the mapping from CVV to display-window
// pixels, but its method is rather odd -- remember its arguments as:
//
//          glViewport(int xLL, int yLL, int width, int height);
//
// The (xLL,yLL) arguments specify the display window pixel address for the
// CVV's (-1,-1) (lower-left) corner; any vertex that leaves the
// GL_PROJECTION matrix with value (-1,-1) will appear at (xLL,yLL) on-screen.
// The 'width' and 'height' specify how many pixels the entire CVV would
// cover on-screen, and we see all that falls within the display window.
//
// The CVV is always square, but our display windows are not; remember that
// contents of the always-square CVV don't look their best if squashed or
// stretched to fill an entire non-square display window. Also, you're not
// required to fill every pixel of the display window with openGL drawings,
// and/or you don't have to show the entire CVV in your display window.  You'll
// get the best looking results if your glViewport() call keeps width=height.
// Try one of these (comment out all but one):
//-------------------------------
//  glViewport(0,0, width, width);	// viewport may extend past window TOP
//-------------------------------
//  glViewport(0,0,height,height);	// viewport may extent past window RIGHT
//-------------------------------
    // Draw CVV in window's largest centered inscribed square:
    if(width > height) {
        // WIDE window:  height sets viewport size, and
        glViewport((width-height)/2,0, height, height);
    }	// move origin rightwards to keep image centered in window.
    else {
        // TALL window:  width sets viewport size, and
        glViewport(0,(height-width)/2, width, width);
        // need to origin upwards to keep image centered in window
    }	// viewport stays SQUARE, but extends to left and right of window

    glViewport(0,0, (GLsizei) width, (GLsizei) height);

// or invent your own; Perhaps make square CVV fit around window?
// Or could you draw TWO viewports on-screen, side-by-side in the same window?
//
//--------------------------------
// Create a simple 3D orthographic camera that shows contents of CVV only:

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                   // basic 3D orthographic cam: show CVV
    gluPerspective(40, (double) width/(double) height, 0.1 ,50.0);
    glutPostRedisplay();			// request redraw--we changed window size!
}

void myDisplay(void)
//------------------------------------------------------------------------------
// Registered as our GLUT display callback function. GLUT calls us whenever we
// need to re-draw the screen.
// OPENGL HINTS:
//	glRotated(123, 1,-2,1);	// rotate by 170 degrees around a 3D axis vector;
//	glScaled(0.2, 0.3, 0.4);	// shrink along x,y,z axes
//  glTranslated(0.1,0.3,0.5);	// translate along x,y,z axes.
{

    if(isCleared==1) { glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); }

    //------------------------------ANIMATION:
    theta1 += thetaStep1;
    theta2 += thetaStep2;
    theta3 += thetaStep3;
    theta5 += thetaStep5;
    theta6 += thetaStep6;
    theta7 += thetaStep7;
    // angular wraparound; keep angles between +/-180 degrees.
    if(theta1 > 90.0) theta1 -= 360.0;
    if(theta1 <-90.0) theta1 += 360.0;
    if(theta2 > 90.0) theta2 -= 360.0;
    if(theta2 <-90.0) theta2 += 360.0;
    if(theta3 > 90.0) theta3 -= 360.0;
    if(theta3 <-90.0) theta3 += 360.0;
    if(theta5 > 90.0) theta5 -= 360.0;
    if(theta5 <-90.0) theta5 += 360.0;
    if(theta6 > 90.0) theta6 -= 360.0;
    if(theta6 <-90.0) theta6 += 360.0;
    if(theta7 > 90.0) theta7 -= 360.0;
    if(theta7 <-90.0) theta7 += 360.0;
    // reverse angular velocities at arm limits to avoid breaking arm's wiring
    if( (theta1 >  0.0 && thetaStep1 >0) ||
            (theta1 < -15.0 && thetaStep1 <0)) thetaStep1 = -thetaStep1;
    if( (theta2 > 5.0 && thetaStep2 >0) ||
            (theta2 < -55.0 && thetaStep2 <0 )) thetaStep2 = -thetaStep2;
    if( (theta3 > 5.0 && thetaStep3 >0) ||
            (theta3 < -65.0 && thetaStep3 <0 )) thetaStep3 = -thetaStep3;
    if( (theta5 > 5.0 && thetaStep5 >0) ||
            (theta5 < -10.0 && thetaStep5 <0 )) thetaStep5 = -thetaStep5;
    if( (theta6 > 5.0 && thetaStep6 >0) ||
            (theta6 < -30.0 && thetaStep6 <0 )) thetaStep6 = -thetaStep6;
    if( (theta7 > 5.0 && thetaStep7 >0) ||
            (theta7 < -8.0 && thetaStep7 <0 )) thetaStep7 = -thetaStep7;
    //----------------------------------------

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, (double) nu_display_width/(double) nu_display_height,  1 ,1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Allow user to turn on / off head light
    if(isHeadLight) {
        lamps[0].I_pos.row[0] = 0.0f;
        lamps[0].I_pos.row[1] = 2.0f;
        lamps[0].I_pos.row[2] = 3.0f;
        lamps[0].I_pos.row[3] = 1.0f;
        lamps[0].applyLamp();
    }

    // Use variables in gluLookAt to allow user to move and aim camera
    gluLookAt(3.0+xPos, 1.5+yPos, 0.0+zPos,
              0.0+xPosTilt, 0.0+yPosTilt, 0.0+zPosTilt,
              0.0, 0.0, 1.0);

    // Allow user to turn on / off world light
    if (isWorldLight) {
        lamps2[0].I_pos.row[0] = 0.0f+worldLightL+worldLightR;
        lamps2[0].I_pos.row[1] = 2.0f;
        lamps2[0].I_pos.row[2] = 3.0f;
        lamps2[0].I_pos.row[3] = 1.0f;
        lamps2[0].applyLamp();
    }

    glTranslated(-2.0,0.0,-2.0);
    // Draw ground plane
    drawGndPlane(50.0, 0.5);

    // Spin the world
    glRotated(0.0+xtheta, 1.0, 0.0, 0.0); // spin the world on x axis, then
    glRotated(0.0+ytheta, 0.0, 1.0, 0.0); // on y axis


    //== MODELING =========================================================

    glTranslated(0.0, 0.0, 0.5);
    glScaled(0.5, 0.5, 0.5);

    // Draw 3 jointed objects (robot, sun, bush)

    // Draw sun
    glPushMatrix();
        if (isDistortion) {
            d_myGLSL->useProgram();

            timer = glGetUniformLocation(d_myGLSL->getProgramID(), "time");
            xs_loc = glGetUniformLocation(d_myGLSL->getProgramID(), "xs");
            zs_loc = glGetUniformLocation(d_myGLSL->getProgramID(), "zs");

            glUniform1f(xs_loc, 1.0f);
            glUniform1f(zs_loc, 1.0f);
            glUniform1f(timer, 1.0f);

            // For distortion, use timer
            progtime+=0.05+changeDistortSpeed;      // advance the timestep
            glUniform1f(timer, progtime);           // send it to the shader as a uniform.
        }
        else {
            p_myGLSL->useProgram();
        }
        drawSun();
    glPopMatrix();

    // Draw robot
    glPushMatrix();
        p_myGLSL->useProgram();
        drawRobot();
    glPopMatrix();

    // Draw bush
    glPushMatrix();
        p_myGLSL->useProgram();
        drawBush();
    glPopMatrix();

    // Tell user to press `H` to print HELP instructions in console
    glPushMatrix();
        stuff[1].applyMatl();
        drawText2D(helv18, 1, -3.55, "Press the 'H' key to print HELP menu in the console");
    glPopMatrix();

    //===============DRAWING DONE.
    glFlush();	                // do any and all pending openGL rendering.
    glutSwapBuffers();			// For double-buffering: show what we drew.
}

void myKeyboard(unsigned char key, int xw, int yw)
//------------------------------------------------------------------------------
// GLUT CALLBACK: Don't call this function in your program--GLUT does it every
// time user presses an alphanumeric key on the keyboard.
//	xw,yw == mouse position in window-system pixels (origin at UPPER left)

{
    int xpos,ypos;  // mouse position in coords with origin at lower left.

    xpos = xw;
    ypos = getDisplayHeight() - yw; //(window system puts origin at UPPER left)
    int curMatl;

    switch(key)
    {
    // Toggle animation on/off.
    case 'p':
    case 'P':
        if(nu_Anim_isOn ==1) runAnimTimer(0);
        else runAnimTimer(1);
        break;
    case 'h':
    case 'H':
        cout << "\nPROGRAMMABLE SHADERS HELP MENU:"
        << "\n------------------------------------------------"
        << "\n`p`: Pause animation"
        << "\n`f`,`s`: Tilt camera right/left"
        << "\n`e`,`d`: Tilt camera up/down"
        << "\n`+`,`-`: Zoom camera in/out"
        << "\n`UP`, `DOWN`, `LEFT`, `RIGHT` arrow keys: Move camera up/down/left/right"
        << "\n`l`: Toggle head light"
        << "\n`w`: Toggle world light"
        << "\n`t`, `y`: Move world light"
        << "\n`z`: Toggle distortion"
        << "\n `1`, `2`: Increase/decrease speed of distortion"
        << "\n`Q`, `ENTER` or `SPACE BAR`: QUIT the program"
        << "\n------------------------------------------------"
        << endl << endl;
        break;
    // tilt camera right
    case 'F':
    case 'f':
        yPosTilt += 0.1;
        break;
    // tilt camera left
    case 'S':
    case 's':
        yPosTilt -= 0.1;
        break;
    // tilt camera up
    case 'e':
    case 'E':
        zPosTilt += 0.1;
        break;
    // tilt camera down
    case 'd':
    case 'D':
        zPosTilt -= 0.1;
        break;
    // zoom camera in
    case '=':
    case '+':
        xPos -= 0.1; //
        xPosTilt -= 0.1;
        break;
    // zoom camera out
    case '-':
    case '_':
        xPos += 0.1; //
        xPosTilt += 0.1;
        break;
    // toggle head light
    case 'l':
    case 'L':
        if(isHeadLight) {
            lamps[0].applyLamp();
            isHeadLight = !isHeadLight;
        }
        else {
            isHeadLight = !isHeadLight;
        }
        break;
    // toggle world light
    case 'w':
    case 'W':
        if(isWorldLight) {
            lamps2[0].applyLamp();
            isWorldLight = !isWorldLight;
        }
        else
            isWorldLight = !isWorldLight;
        break;
    // toggle distortion
    case 'z':
    case 'Z':
        if (isDistortion)
            isDistortion = !isDistortion;
        else
            isDistortion = !isDistortion;
        break;
    // Move world light left
    case 'T':
    case 't':
        worldLightL += 0.2;
        break;
    // Move world light right
    case 'Y':
    case 'y':
        worldLightR -= 0.2;
        break;
    case '1':
        changeDistortSpeed += 0.1;
        break;
    case '2':
        if (changeDistortSpeed >= 0.1) {
            changeDistortSpeed -= 0.1;
        }
        break;
    //======================================================================
    // User wants to exit
    case ' ':
    case 27:
    case 'Q':
    case 'q':
        exit(0);
        break;
    default:
        glutSwapBuffers();
        break;
    }
}

void myKeySpecial(int key, int xw, int yw)
//------------------------------------------------------------------------------
// Called by GLUT when users press 'arrow' keys, 'Fcn' keys, or any other
// non-alphanumeric key.  Search glut.h to find other #define literals for
// special keys, such as GLUT_KEY_F1, GLUT_KEY_UP, etc.
//	xw,yw == mouse position in window-system pixels (origin at UPPER left!)/
{
    int xpos,ypos;      // mouse position in coords with origin at lower left.

    xpos = xw;
    ypos = getDisplayHeight() - yw; //(window system puts origin at UPPER left)
    switch(key)
    {
    case GLUT_KEY_HOME:
        cout << "Home key." << endl;
        break;
    case GLUT_KEY_PAGE_DOWN:
        cout << "PgUp key." << endl;
        break;
    case GLUT_KEY_PAGE_UP:
        cout << "PgDn key."  << endl;
        break;
    // move cam left
    case GLUT_KEY_LEFT:
        yPos -= 0.1;
        yPosTilt -= 0.1;
        break;
    // move cam right
    case GLUT_KEY_RIGHT:
        yPos += 0.1;
        yPosTilt += 0.1;
        break;
    // move cam down
    case GLUT_KEY_DOWN:
        zPos -= 0.1;
        zPosTilt -= 0.1;
        break;
    // move cam up
    case GLUT_KEY_UP:
        zPos += 0.1;
        zPosTilt += 0.1;
        break;
    default:
        break;
    }
    //===============DRAWING DONE.
    glutSwapBuffers();	// For double-buffering: show what we drew.
}

void myMouseClik(int buttonID, int upDown, int xw, int yw)
//-----------------------------------------------------------------------------
// Called by GLUT on mouse button click or un-click
// When user clicks a mouse button,  buttonID== 0 for left mouse button,
//                              		    (== 1 for middle mouse button?)
//	                                 		 == 2 for right mouse button;
//	                        upDown  == 0 if mouse button was pressed down,
//	                                == 1 if mouse button released.
//	xw,yw == mouse position in window-system pixels (origin at UPPER left!)
{
    int xpos,ypos;  // mouse position in coords with origin at lower left.

    xpos = xw;
    ypos = getDisplayHeight() - yw; //(window system puts origin at UPPER left)

    xclik = xpos;   // save most-recent click in global var
    yclik = ypos;

    cout << "click; buttonID=" << buttonID <<", upDown=" << upDown;
    cout << ", at xpos,ypos=(" << xpos <<"," << ypos << ")\n";

    //===============DRAWING DONE.
//    glFlush();	        // do any and all pending openGL rendering.
    glutSwapBuffers();	// For double-buffering: show what we drew.
}

void myMouseMove(int xw, int yw)
//-----------------------------------------------------------------------------
// Called by GLUT when user moves mouse while pressing any mouse button.
//	xw,yw == mouse position in window-system pixels (origin at UPPER left)
{
    int xpos,ypos;  // mouse position in coords with origin at lower left.

    xpos = xw;
    ypos = getDisplayHeight() - yw; //(window system puts origin at UPPER left)

    ytheta += 0.3*(xpos-xclik);     // (drag in +X direction will spin the
    //  eyepoint vertex on world +Y axis)
    xtheta -= 0.3*(ypos-yclik);     // (drag in +Y direction will spin the
    //  eyepoint vertex on world -X axis)

//cout << "(xpos-xclik,ypos-yclik) " << (xpos-xclik) << ", " << (ypos-yclik) << endl;
    cout << "(xtheta,ytheta) " << xtheta << ", " << ytheta << endl;
    xclik = xpos;                   // update current mouse position.
    yclik = ypos;

    //===============DRAWING DONE.
//    glFlush();	        // do any and all pending openGL rendering.
    glutSwapBuffers();	// For double-buffering: show what we drew.
}

void runAnimTimer(int isOn)
//-----------------------------------------------------------------------------
// Call runAnimTimer(0) to STOP animation;
//      runAnimTimer(1) to START or CONTINUE it.
{
    nu_Anim_isOn = isOn;   // Update global var to the most-recent state;
    if(isOn == 1)          // Start or continue animation?
    {
        //YES. ANIMATE!   register a GLUT timer callback:
        glutTimerFunc(NU_PAUSE, myTimer, 1);
        // This registers 'myTimer()' as a timer callback function.
        // GLUT will now pause NU_PAUSE milliseconds (longer if busy drawing),
        // call myTimer(1), and then 'un-register' the callback; it won't call
        // myTimer() again unless we register it again.
        // myTimer(1) checks to be SURE no other 'stop animation' requests
        // arrived (e.g. keyboard input, mouse input, visibility change, etc.)
        // while we waited.  If global variable nu_Anim_isOn is STILL 1,
        // it calls runAnimTimer(1) to continue animation.
    }
    // NO.  STOP animation-- don't register a GLUT timer for myTimer().
}

void myTimer (int value)
//------------------------------------------------------------------------------
// Don't call this function yourself; use runAnimTimer() instead!
// runAnimTimer(1) causes GLUT to wait NU_PAUSE milliseconds (longer if busy)
// and then call this function.
// TRICKY: We allow users to choose very long NU_PAUSE times to see what happens
// in animation.  During GLUT's wait time, user events (keyboard, mouse, or
// window cover/uncover) may make runAnimTimer(0) calls intended to STOP
// animation.  GLUT can't cancel a previously-issued GLUT timer callback, so
// in the TimerCallback() function we check global variable nu_Anim_isON.
{
    // GLUT finished waiting.
    if(value != nu_Anim_isOn)
        cout << "\n(Anim cancelled during GLUT wait)\n";
    if(nu_Anim_isOn==1)         // Go on with animation?
    {
        // YES.
        glutPostRedisplay();    // tell GLUT to re-draw the screen, and
        runAnimTimer(1);        // continue with the next time interval.
    }
    // NO. Do nothing. No further animation.
}

void myHidden (int isVisible)
//------------------------------------------------------------------------------
// Called by GLUT when our on-screen window gets covered or un-covered. When
// called, it turns off animation and re-display when our display window is
// covered or stowed (not visible). SEE ALSO: runAnimTimer() and myTimer()
{
    if(isVisible == GLUT_VISIBLE)  runAnimTimer(1); // Visible! YES! Animate!
    else runAnimTimer(0);                           // No. Stop animating.
}


void drawText2D(void *pFont, double x0, double y0, const char *pString)
//-----------------------------------------------------------------------------
//  Using the current color and specified font, write the text in 'pString' to
// the display window.  Place the lower left corner of the first line of text
//  at screen position x0,y0 in the current coordinate system.
//      Example:  DrawText2D(helv18, 0.3,0.4 ,"Hello!");
//
// Available fonts:
// helv10 (HELVETICA size 10)   rom10 (TIMES_ROMAN size 10)
// helv12 (HELVETICA size 12)   rom24 (TIMES_ROMAN size 24)
// helv18 (HELVETICA size 18)
{
    int i, imax;					// character counters

    glRasterPos2d(x0, y0);		// set text's lower-left corner position
    imax = 1023;				// limit the number of chars we print.
    for(i=0; pString[i] != '\0' && i<imax; i++)	// for each char,
    {
        glutBitmapCharacter(pFont, pString[i]);
    }
}

void drawAxes(void)
//------------------------------------------------------------------------------
// Draw small +x,+y,+z axes in red, green, blue:
{
    glColor3d(1.0, 0.0, 0.0);	// red
    glBegin(GL_LINES);
    glVertex3d(0.0,0.0,0.0);	// +x-axis
    glVertex3d(0.4,0.0,0.0);
    glEnd();
    glColor3d(0.0, 1.0, 0.0);	// green
    glBegin(GL_LINES);
    glVertex3d(0.0,0.0,0.0);	// +y-axis
    glVertex3d(0.0,0.4,0.0);
    glEnd();
    glColor3d(0.0, 0.5, 1.0);	// bright blue
    glBegin(GL_LINES);
    glVertex3d(0.0,0.0,0.0);	// +z-axis
    glVertex3d(0.0,0.0,0.4);
    glEnd();
}

// Read the current size and position of the graphics display window
//------------------------------------------------------------------------------
int getDisplayHeight(void)
{
    return nu_display_height;
}
int getDisplayWidth(void)
{
    return nu_display_width;
}
int getDisplayXpos(void)
{
    return nu_display_xpos;
}
int getDisplayYpos(void)
{
    return nu_display_ypos;
}


void drawGndPlane(GLdouble siz, GLdouble xygap)
{
    int ij, ijmax;

    if(xygap <= 0.0)
    {
        xygap = 1.0;
        cout <<"\n\n!!! drawGndPlane() defaulted to xygap=1.0!!!\n\n" << endl;
    }
    ijmax = (int)(2.0*siz / xygap);    // how many lines to draw.

    // Draw lines parallel to x axis;
    glBegin(GL_LINES);
    for(ij = 0; ij <= ijmax; ij++)
    {
        glVertex3d(-siz, -siz + ij*xygap, 0.0);
        glVertex3d( siz, -siz + ij*xygap, 0.0);
    }
    glEnd();
    // Draw lines parallel to y axis;
    glBegin(GL_LINES);
    for(ij = 0; ij <= ijmax; ij++)
    {
        glVertex3d(-siz + ij*xygap, -siz, 0.0);
        glVertex3d(-siz + ij*xygap,  siz, 0.0);
    }
    glEnd();
}

void drawBush(void) {
//------------------------------------------------------------------------------
// Draw 3D jointed bush
//------------------------------------------------------------------------------
    // bush body
    glPushMatrix();
        stuff[5].applyMatl();
        glTranslated(-1.8, -4.5, 0.5);
        glScaled(0.7,0.7,0.7);
        glutSolidDodecahedron();
        glPushMatrix();
            glTranslated(0, 2.0, 0.3);
            glScaled(.5,0.5,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidDodecahedron();
        glPopMatrix();
        // branch
        glPushMatrix();
            stuff[6].applyMatl();
            glTranslated(-0.3,2.3,0.4);
            glRotated(-80,0,0,1);
            glRotated(30,0,1,0);
            glScaled(5,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // branch
        glPushMatrix();
            stuff[6].applyMatl();
            glTranslated(1,1.3,0.4);
            glRotated(-80,0,0,1);
            glRotated(50,0,1,0);
            glScaled(5,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // branch
        glPushMatrix();
            stuff[6].applyMatl();
            glTranslated(0.5,-1.2,1.7);
            glRotated(-80,0,0,1);
            glRotated(100,0,1,0);
            glScaled(5,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // branch
        glPushMatrix();
            stuff[6].applyMatl();
            glTranslated(1.5,-0.9,0.3);
            glRotated(-50,0,0,1);
            glRotated(20,0,1,0);
            glScaled(5,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // shrub
        glPushMatrix();
            stuff[5].applyMatl();
            glTranslated(0, 1.2, 1.2);
            glScaled(0.6,0.6,0.6);
            glRotated(-theta2, 0,1, 0);
            glutSolidDodecahedron();
        glPopMatrix();
        // shrub
        glPushMatrix();
            stuff[5].applyMatl();
            glTranslated(0, -0.3, 1.6);
            glScaled(0.6,0.6,0.6);
            glRotated(-theta2, 0,0,1);
            glutSolidDodecahedron();
        glPopMatrix();
        // shrub
        glPushMatrix();
            stuff[5].applyMatl();
            glTranslated(0, -1.3, 1.3);
            glScaled(0.8,0.8,0.8);
            glRotated(-theta2, 1,0,0);
            glutSolidDodecahedron();
        glPopMatrix();
        // shrub
        glPushMatrix();
        stuff[5].applyMatl();
            glTranslated(0, -1.7, 0.2);
            glScaled(0.7,0.7,0.7);
            glRotated(-theta2, 0,1,0);
            glutSolidDodecahedron();
        glPopMatrix();
        // shrub
        glPushMatrix();
            stuff[5].applyMatl();
            glTranslated(1.3, -1.0, 0.2);
            glScaled(0.6,0.6,0.6);
            glRotated(-theta2, 0,0,1);
            glutSolidDodecahedron();
        glPopMatrix();
        // shrub
        glPushMatrix();
            stuff[5].applyMatl();
            glTranslated(1.3, 0.2, 0.2);
            glScaled(0.5,0.5,0.5);
            glRotated(-theta2, 1,0,0);
            glutSolidDodecahedron();
        glPopMatrix();
        // shrub
        glPushMatrix();
            stuff[5].applyMatl();
            glTranslated(1.3, 1.4, 0.2);
            glScaled(0.6,0.6,0.6);
            glRotated(-theta2, 0,1,0);
            glutSolidDodecahedron();
        glPopMatrix();
        // shrub
        glPushMatrix();
            stuff[5].applyMatl();
            glTranslated(1.3, 1.0, -0.5);
            glScaled(0.3,0.3,0.3);
            glRotated(-theta2, 0,1,0);
            glutSolidDodecahedron();
        glPopMatrix();
    glPopMatrix();
}

void drawSun(void) {
//------------------------------------------------------------------------------
// Draw 3D jointed sun
//------------------------------------------------------------------------------
    glPushMatrix();
        stuff[7].applyMatl();
        // body of sun
        glTranslated(-6.5, -6.9, 7.0);
        glutSolidSphere(1.8,35,35);
        // first ray
        glPushMatrix();
            stuff[8].applyMatl();
            glTranslated(-0.3,2.3,0.4);
            glRotated(-100,0,0,1);
            glRotated(-10,0,1,0);
            glScaled(3.9,0.4,0.5);
            glRotated(-theta3, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // second ray
        glPushMatrix();
            glTranslated(-0.3,2.2,-1.35);
            glRotated(-100,0,0,1);
            glRotated(-40,0,1,0);
            glScaled(3.9,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        // third ray
        glPopMatrix();
        glPushMatrix();
            glTranslated(0,0.4,-2.25);
            glRotated(-100,0,0,1);
            glRotated(-70,0,1,0);
            glScaled(3.9,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // fourth ray
        glPushMatrix();
            glTranslated(0.7,-1.5,-1.65);
            glRotated(-100,0,0,1);
            glRotated(-140,0,1,0);
            glScaled(3.9,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // fifth ray
        glPushMatrix();
            glTranslated(1.0,-2.2,-0.35);
            glRotated(-80,0,0,1);
            glRotated(-170,0,1,0);
            glScaled(3.9,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // sixth ray
            glPushMatrix();
            glTranslated(1.0,-1.5,1.3);
            glRotated(-30,0,0,1);
            glRotated(-200,0,1,0);
            glScaled(3.9,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // seventh ray
        glPushMatrix();
            glTranslated(1.0,0,1.8);
            glRotated(-30,0,0,1);
            glRotated(-240,0,1,0);
            glScaled(3.9,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // eighth ray
        glPushMatrix();
            glTranslated(0,1,1.7);
            glRotated(-100,0,0,1);
            glRotated(20,0,1,0);
            glScaled(3.9,0.4,0.5);
            glRotated(-theta2, 1,0, 0);
            glutSolidCube(0.45);
        glPopMatrix();
        // draw left eye
        glPushMatrix();
            stuff[2].applyMatl();
            glTranslated(1.6,0.3,0);
            glutSolidSphere(0.35, 15, 85);
            glPushMatrix();
                stuff[1].applyMatl();
                glTranslated(0.3,0.1,-0.04);
                glutSolidSphere(0.13, 35, 85);
            glPopMatrix();
        glPopMatrix();
        // draw right eye
        glPushMatrix();
            stuff[2].applyMatl();
            glTranslated(1.1,1.3,0);
            glutSolidSphere(0.35, 15, 85);
            glPushMatrix();
                stuff[1].applyMatl();
                glTranslated(0.3,0.14,-0.04);
                glutSolidSphere(0.13, 35, 85);
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
}

void drawRobot(void) {
//------------------------------------------------------------------------------
// Draw 3D jointed robot
//------------------------------------------------------------------------------
    stuff[0].applyMatl();
    glPushMatrix();
        glTranslated(-6, -1, 2.0);
        glPushMatrix();
        glTranslated(0,-1.8,-0.0);
    glPopMatrix();
    glRotated(15, 0,0,1);
    glRotated(-10,0,1,0);

    //------------------------------------------------------------------------------
    // ARMS
    //------------------------------------------------------------------------------

    // Begin drawing right arm

    glPushMatrix();
        glTranslated(0.20, 1.25, -0.75);
        glRotated(60,0,1,0);
        glRotated(20,0,0,1);
        glScaled(3.9,0.4,0.5);
        glRotated(-theta2, 1,0, 0);
        glutSolidCube(0.45);
    glPopMatrix();
    // End drawing right arm

    // Begin drawing left arm
    glPushMatrix();
        glTranslated(0.20, -1.35, -0.75);
        glRotated(-60,0,1,0);
        glRotated(20,0,0,1);
        glScaled(3.9,0.4,0.5);
        glRotated(-theta2, 1,0, 0);
        glutSolidCube(0.45);
    glPopMatrix();
    // End drawing left arm

    //------------------------------------------------------------------------------
    // Body
    //------------------------------------------------------------------------------
    glPushMatrix();
        stuff[4].applyMatl();
        glTranslated(0, 0, -0.9);
        glScaled(1.3,1.6,1.4);
        glutSolidCube(1.4);
    glPopMatrix();

    // Begin drawing head
    glPushMatrix();
        stuff[0].applyMatl();
        glRotated(-theta7, 0,1,0);
        glTranslated(0, 0, 0.3);
        glutSolidCube(0.55);            // first neck joint
        glPushMatrix();
            glRotated(-theta6,0,1,0);
            glTranslated(0,0,0.15);
            glutSolidCube(0.55);        // second neck joint

            glPushMatrix();
                glRotated(-theta6, 1,0,0);
                glTranslated(0,0,.8);
                glutSolidCube(1.3);           // robot head
                // Begin drawing right eye
                glPushMatrix();
                    stuff[2].applyMatl();
                    glTranslated(0.45,0.30,0.25);
                    glutSolidSphere(0.35, 15, 85);
                    glPushMatrix();
                        stuff[1].applyMatl();
                        glTranslated(0.3,-0.02,0);
                        glutSolidSphere(0.12, 35, 85);
                    glPopMatrix();
                glPopMatrix();
                // End drawing right eye

                // Begin drawing left eye
                glPushMatrix();
                    stuff[2].applyMatl();
                    glTranslated(0.45,-0.30,0.25);
                    glutSolidSphere(0.35, 15, 85);
                    glPushMatrix();
                        stuff[1].applyMatl();
                        glTranslated(0.3,-0.02,0);
                        glutSolidSphere(0.12, 35, 85);
                    glPopMatrix();
                glPopMatrix();
                // End drawing left eye

                // Begin drawing nose
                glPushMatrix();
                    stuff[3].applyMatl();
                    glTranslated(0.45,0.0,-0.10);
                    glutSolidSphere(0.26, 15, 15);
                glPopMatrix();
                // End drawing nose

                // Begin drawing right antenna
                glPushMatrix();
                    glRotated(theta4, 0,1,0);
                    glTranslated(0,-0.25,0.85);
                    glScaled(1.0, 1.0, 0.4);
                    glPushMatrix();
                        stuff[4].applyMatl();
                        glTranslated(0.15,-0.15,1.1);
                        glRotated(-10,1,0,0);
                        glRotated(theta5, 1,0,0);
                        glRotated(90,0,1,0);
                        glutSolidCube(0.65);
                    glPopMatrix();
                    glPushMatrix();
                        stuff[0].applyMatl();
                        glRotated(theta4, 0,1,0);
                        glTranslated(0.3,-0.10,0.05);
                        glRotated(180, 1 , 0,0);
                        glScaled(0.1, 0.3,3.5);
                        glRotated(-5,0,1, 0);
                        glutSolidCube(0.45); // antenna
                    glPopMatrix();
                glPopMatrix();
                // End drawing right antenna

                // Begin drawing left antenna
                glPushMatrix();
                    glRotated(theta4, 0,1,0);
                    glTranslated(0,0.50,0.85);
                    glScaled(1.0, 1.0, 0.4);
                    glPushMatrix();
                        stuff[4].applyMatl();
                        glTranslated(0.15,-0.15,1.1);
                        glRotated(90,0,1,0);
                        glRotated(-10,1,0,0);
                        glRotated(theta5, 1,0,0);
                        glutSolidCube(0.65);
                    glPopMatrix();
                    glPushMatrix();
                        stuff[0].applyMatl();
                        glRotated(theta4, 0,1,0);
                        glTranslated(0.3,-0.10,0.05);
                        glRotated(180, 1 , 0,0);
                        glScaled(0.1, 0.3,3.5);
                        glRotated(-5,0,1, 0);
                        glutSolidCube(0.45); // antenna
                    glPopMatrix();
                glPopMatrix();
                // End drawing left antenna

            glPopMatrix();
        glPopMatrix();
    glPopMatrix();

    //------------------------------------------------------------------------------
    // Draw robot legs
    //------------------------------------------------------------------------------
    // Begin drawing right leg
    glPushMatrix();
        glRotated(theta5, 0,1,0);
        glTranslated(0,-0.25,-2.4);
        glScaled(1.0, 1.0, 0.6);
        glPushMatrix();
            stuff[0].applyMatl();
            glRotated(theta4, 0,1,0);
            glTranslated(0.0,-0.30,0.65);
            glRotated(180, 1 , 0,0);
            glRotated(-10,0,1, 0);
            glScaled(1.0, 1.0,1.2);
            glutSolidCube(0.45); // first leg joint
            glPushMatrix();
                stuff[0].applyMatl();
                glTranslated(0,0,0.4);
                glutSolidCube(0.45); // second leg joint
            glPopMatrix();
            glPushMatrix();
                stuff[1].applyMatl();
                glTranslated(-0.15,0.08,0.9);
                glRotated(90,0,1,0);
                glRotated(-10,1,0,0);
                glRotated(theta5, 1,0,0);
                glutSolidCube(0.65); // foot
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
    // End drawing right leg

    // Begin drawing left leg
    glPushMatrix();
        glRotated(-theta5, 0,1,0);
        glTranslated(0,0.65,-2.4);
        glScaled(1.0, 1.0, 0.6);
        glPushMatrix();
            stuff[0].applyMatl();
            glRotated(theta4, 0,1,0);
            glTranslated(0,-0.3,0.6);
            glRotated(180, 1 , 0,0);
            glRotated(-10,0,1, 0);
            glScaled(1.0, 1.0,1.2);
            glutSolidCube(0.45); // first leg joint
            glPushMatrix();
                stuff[0].applyMatl();
                glTranslated(0,0,0.4);
                glutSolidCube(0.45); // second leg joint
            glPopMatrix();
            glPushMatrix();
                stuff[1].applyMatl();
                glTranslated(-0.15,0,0.9);
                glRotated(90,0,1,0);
                glRotated(-10,1,0,0);
                glRotated(theta5, 1,0,0);
                glScaled(1.0, 1.0,1.2);
                glutSolidCube(0.65); // foot
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
    // End drawing left leg
    glPopMatrix();
}
