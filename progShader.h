#ifndef PROGSHADER_H_INCLUDED
#define PROGSHADER_H_INCLUDED


//-----------------------------------------------------------------------------
// multi-platform includes for the GLUT/OpenGL libraries:
#if defined(_WIN32)
#include <GL\glew.h>    // Open-source GL extension wrangler: for GLSL shaders.
#include <GL\freeglut.h>// open-source GLUT functions for windows, keybd, mouse
/*---*OR*--- use:
 // #include <GL/glut.h> // for 100% backwards-compatibility with Mark Kilgard's
 // SGI-copyrighted GLUT library. Careful! this excludes
 // freeglut's callbacks for mouse wheel, etc.
*/
/*
 // freeglut.h already contains these other includes that were necessary for
 // using Kilgard's original GLUT library:
 #include <GL/GL.h>      // OpenGL 3D graphics rendering functions
 #include <GL/Glu.h>     // OpenGL utilties functions
 #include <GL/glext.h>   // #defines for all OpenGL features on your machine.
 */

#elif defined(__APPLE__)
#include <GLUT/glut.h>  // GLUT functions for windows, keybd, mouse
#include <OpenGL/GL.h>  // OpenGL 3D graphics rendering functions
#include <OpenGL/Glu.h> // OpenGL utilties functions
#include <OpenGL/GLext.h>   // #defines for all OpenGL features on your machine.
// for UNIX, Linux of all flavors;
#else
#include <unistd.h>         // defines symbolic constants, POSIX versions, etc.
#include <GL/glew.h>        // OpenGL 'extensions wrangler' for mesa & GLSL use.
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif
//------------------------------------------------------------------------------

#include <string>
using namespace std;

/*==============================================================================
    CShader Class: GLSL source-code objects, for OpenGL's GLSL shader language.
	Create one CShader object for each one of your:
            --vertex shaders,
            --fragment shaders,
            --and (eventually) geometry shaders.

 Each object of this type holds source code and misc descriptors for one
 complete 'shader' written in openGL's GLSL (openGL Shading Language); either
 a 'vertex shader' or a 'fragment shader'.

 Requires openGL 2.0 or better; on Windows we require open-source 'glew' library.
 Each object stores the shader's source as a single string ASCII text, held in
 a dynamically-allocated buffer at pointer pText.

    2011.02.26  -- J. Tumblin. Created
    2012.01.22  -- P. Olczak; modified for use with CProgGLSL class.
    2012.02.29  -- J. Tumblin; revised naming, comments, multiplatform linking
================================================================================
*/
#include <iostream>
#include <fstream>
#include <string>


//===================shaderType===================
// Code to identify shader contents--useful as debugging tool.
#define INVALID_SHADER      0       // default for 'shaderType' member var.
//---VERTEX SHADERS-------
#define V_SHADER_BARE     100       // simplest possible vertex shader:
#define V_SHADER_FILENAME 101       // src code read in from 'fileName' member
// ADD YOUR OWN VERTEX SHADERS!

//---FRAGMENT SHADERS-----
#define F_SHADER_BARE     200       // simplest possible fragment shader:
#define F_SHADER_FILENAME 201       // src code read in from 'fileName' member
// ADD YOUR OWN FRAGMENT SHADERS!

//---GEOMETRY SHADERS-----
#define G_SHADER_BARE     300

using namespace std;

class CShader
{
public:
    int shaderType;                 // see #define statements above.
    string fileName;                // shader source-code filename to read/write
    string pathName;                // path to 'thisPrograms.exe' (this program)
    // (We need this for codeBlocks on Apple;
    // fstream's 'current working dir' is *NOT*
    // not your CodeBlocks project directory,
    // but the filesystem root (C:\)).
    string srcCode;                 // C++ string obj. holds shader source code
    // as the the char pointer srcCode.c_Str
    const GLchar *src;              // The NULL-terminated text string that
    // holds this shader's GLSL source code.
    // (seems redundant, but we MUST have it
    // to load the src code into GLSL).

    GLuint shaderObjID;             // GLSL identifies each shader object (held
    // within a program object) by a unique
    // 'shader ID'. Calling glCreateShader()
    // creates and empty shader object, and
    // returns its ID#.

public:
    CShader(void);                // default constructor,destructor.
    ~CShader(void);

    void setPath(const char *pPath);// save pathName to executable's directory;
    // for Apple Mac OS X: setPath(argv[0]);
    void vertBareShader(void);      // Make bare 'pass-thru' vertex shader
    void fragBareShader(void);      // Make bare 'pass-thru' fragment shader
    void print(void);               // print shader's src code in console window
    void readShaderFile(const char *fName); // read the named shader file

};

/*==============================================================================

  CProgGLSL Class: Complete GLSL program, with create/load/compile/unload for
                    one vertex shader and one fragment shader.


  2012.01.20  Created --paul olczak
  2012.01.30  fix missing GLEW library includes; fix CProgGLSL constructor
              filename errors, short/meaningful names,comments--Jack Tumblin

================================================================================
*/

class CProgGLSL
//------------------------------------------------------------------------------
// Holds a complete GLSL 'program' consisting of vertex shaders and fragment
// shaders.  The CProgGLSL object loads/compiles/unloads all shaders as needed
// into the GPU graphics hardware. Each shader's source code is stored in a
// 'CShader' object that read it from a plain-text file.
{
private:
    CShader   vSh,fSh;      // the vertex and fragment shader objects

    GLuint  programID;      // GLSL keeps shaders (& other objects too)
    // in a 'Program Object' container. When you call
    // glCreateProgram() it makes an empty Program
    // Object on the GPU and records its OpenGL ID#.
public:
    CProgGLSL(void);    // default constructor,destructor.
    ~CProgGLSL(void);

    CProgGLSL(const char *myArgv0,
              string vertexShFileName, string fragmentShFileName);
    // shader-path-and-filename-setting constructor;
    // use this to create a program object.

#if !defined(__APPLE__)
    /* 'hardwareCheck()' member function is not available (and not necessary) on
     Apple MacOS, because it doesn't use the GLEW library this function requires.
     Apple Xcode supplies its own method (perhaps in the Xcode 'OpenGL' framework?)
     for linking with OpenGL's extended functions such as glCreateShaderObject().

     Windows and Linux systems need GLEW to simplify access to openGL extensions.
     Link your program to the GLEW library; after you've established your openGL
     rendering context (after your glutCreateWindow() command), call glewInit().
     After this, you can 'call-by-name' the openGL extended functions, just like any
     other OpenGL function.

     GLEW performs the messy process of accessing OpenGL's extended functions. Each
     time you call an extended function, GLEW checks availability on your hardware;
     if it exists, GLEW then requests a function pointer to it, and calls the
     functions with the arguments you supplied in your call-by-name usage.
     */
    int hardwareCheck(void);        // returns FALSE if this machine's openGL
    // implementation DOES NOT support GLSL;
#endif

    void setVertexShFileName(string name);
    void setFragmentShFileName(string name);

    // Shaders' default path: the directory that holds your executable (.exe)
    // (e.g. the path found in argv[0]). YOU PROBABLY DON'T NEED THESE FCNS!
    void setVertexShPath(string path);
    void setFragmentShPath(string path);

    void loadShaders(void);    // read shaders src-code from plain-text files

    void compileProgram(void); // convert source code into GPU executables

    void useProgram(void);     // load program into GPU, make it an active part
    // of the current openGL rendering method/state.

    GLuint getProgramID(void);      // return GPU's 'program' ID# that
    // identifies our the GLSL program object.
    GLuint getVertShaderID(void);   // return GPU's 'shader' ID# that identifies
    // our vertex-shader object.
    GLuint getFragShaderID(void);   // return GPU's 'shader' ID# that identifies
    // our vertex-shader object.
};

// Utility functions to support the programable pipeline
void initializeShaders(void);

#endif  // _NU_PROGSHADER_H_

