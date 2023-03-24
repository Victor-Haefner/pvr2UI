#include "glut.h"

#include <iostream>

#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#endif

ostream& operator<<(ostream& os, const Surface& s);


void Glut::updateGLWindow(Surface surface) { // glArea.surface
    cout << "     Glut::updateGLWindow " << surface << endl;
    if (winGL < 0) return;
    glutSetWindow(winGL);
    glutPositionWindow(surface.x, surface.y);
    glutReshapeWindow(surface.width, surface.height);
}

void Glut::on_resize_window(int w, int h) { // resize top window
    cout << "  Glut::on_resize_window " << endl;
    topWinSize[0] = w;
    topWinSize[1] = h;

    glutSetWindow(winUI);
    glutReshapeWindow(topWinSize[0], topWinSize[1]);
    resizeSignal("glutResize", {0,0,topWinSize[0], topWinSize[1]});
}

void Glut::on_gl_display() {
    cout << "  Glut::on_gl_display " << endl;
    if (winGL < 0) return;
    glutSetWindow(winGL);
    int w = glutGet(GLUT_WINDOW_WIDTH); // calling glutGet somehow magically fixes the resize glitches..
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    signal( "glutRenderGL", {} );
}

void Glut::on_ui_display() {
    cout << "  Glut::on_ui_display " << endl;
    if (winUI < 0) return;
    glutSetWindow(winUI);
    signal( "glutRenderUI", {} );
}

void Glut::on_gl_resize(int w, int h) {
    cout << "  Glut::on_gl_resize " << w << ", " << h << endl;
    if (winGL < 0) return;
    glutSetWindow(winGL);
    resizeSignal( "glutResizeGL", {0,0,w,h} );
}

void Glut::on_ui_resize(int w, int h) {
    cout << "  Glut::on_ui_resize " << w << ", " << h << endl;
    if (winUI < 0) return;
    glutSetWindow(winUI);
    resizeSignal( "glutResizeUI", {0,0,w,h} );
}

Glut::Glut() {}

Glut* glutInstance = 0;

void onMainReshape(int w, int h) { glutInstance->on_resize_window(w,h); }
void onUIDisplay() { glutInstance->on_ui_display(); }
void onGLDisplay() { glutInstance->on_gl_display(); }

void onUIReshape(int w, int h) { glutInstance->on_ui_resize(w, h); }
void onGLReshape(int w, int h) { glutInstance->on_gl_resize(w, h); }

void Glut::init(int argc, char** argv, Signal signal, ResizeSignal resizeSignal) {
    cout << "Glut::init" << endl;
    this->signal = signal;
    this->resizeSignal = resizeSignal;
    glutInstance = this;

    glutInit(&argc, argv);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    unsigned int mode = (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL | GLUT_MULTISAMPLE);
    //mode |= GLUT_STEREO; // active stereo
    glutInitDisplayMode(mode);


    topWinSize[0] = glutGet(GLUT_SCREEN_WIDTH);
    topWinSize[1] = glutGet(GLUT_SCREEN_HEIGHT);
    resizeSignal("glutResize", {0,0,topWinSize[0], topWinSize[1]});

    glutInitWindowSize(topWinSize[0], topWinSize[1]);
    topWin = glutCreateWindow("PolyVR");
    glutReshapeFunc( onMainReshape );

    winUI = glutCreateSubWindow(topWin, 0, 0, topWinSize[0], topWinSize[1]);
    glutDisplayFunc( onUIDisplay );
    glutReshapeFunc( onUIReshape );

    glutSetWindow(topWin);
    winGL = glutCreateSubWindow(topWin, 0,0,800,800);
    glutDisplayFunc( onGLDisplay );
    glutReshapeFunc( onGLReshape );
}

void Glut::activateWindow(int i) {
    cout << "  Glut::activateWindow " << i << endl;
    if (i == 1 && topWin > 0) glutSetWindow(topWin);
    if (i == 2 && winUI > 0) glutSetWindow(winUI);
    if (i == 3 && winGL > 0) glutSetWindow(winGL);
}

void Glut::render() {
    //glutMainLoopEvent();
    cout << " Glut render" << endl;
    glutMainLoopEvent();
    glutMainLoopEvent();
    glutMainLoopEvent();

    glutSetWindow(winGL);
    glutPostRedisplay();
    glutSetWindow(winUI);
    glutPostRedisplay();

    cout << " Glut process events" << endl;
    glutMainLoopEvent();
    glutMainLoopEvent();
    glutMainLoopEvent();
    glutMainLoopEvent(); // render again after window reshapes

    glutSetWindow(winGL);
    cout << " Glut swap win GL" << endl;
    glutSwapBuffers();
    glutSetWindow(winUI);
    cout << " Glut swap win UI" << endl;
    glutSwapBuffers();
}

void Glut::run() {
    cout << "Glut::run" << endl;
    glutMainLoop();
    signal("glutCleanup", {});

    // Cleanup
    //ImGui_ImplGLUT_Shutdown();
    //cleanupUI();
}
