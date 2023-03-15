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



void Glut::updateGLWindow(Surface surface) { // glArea.surface
    if (winGL < 0) return;
    glutSetWindow(winGL);
    //cout << "updateGLWindow " << glArea.surface << endl;
    glutReshapeWindow(surface.width, surface.height);
    glutPositionWindow(surface.x, surface.y);
    glutSetWindow(winUI);
}

void Glut::on_resize_window(int w, int h) {
    topWinSize[0] = w;
    topWinSize[1] = h;

    glutSetWindow(winUI);
    glutReshapeWindow(topWinSize[0], topWinSize[1]);
    resizeSignal("glutResize", {0,0,topWinSize[0], topWinSize[1]});
}

void Glut::on_gl_display() {
    glutSetWindow(winGL);
    signal( "glutRenderGL", {} );
    glutSwapBuffers();
    glutPostRedisplay();
}

void Glut::on_ui_display() {
    glutSetWindow(winUI);
    signal( "glutRenderUI", {} );
    glutSwapBuffers();
    glutPostRedisplay();
}

Glut::Glut() {}

Glut* glutInstance = 0;

void onMainReshape(int w, int h) { glutInstance->on_resize_window(w,h); }
void onUIDisplay() { glutInstance->on_ui_display(); }
void onGLDisplay() { glutInstance->on_gl_display(); }

void Glut::init(int argc, char** argv, Signal signal, ResizeSignal resizeSignal) {
    cout << "Glut::init" << endl;
    this->signal = signal;
    this->resizeSignal = resizeSignal;
    glutInstance = this;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);

    topWinSize[0] = glutGet(GLUT_SCREEN_WIDTH);
    topWinSize[1] = glutGet(GLUT_SCREEN_HEIGHT);
    resizeSignal("glutResize", {0,0,topWinSize[0], topWinSize[1]});

    glutInitWindowSize(topWinSize[0], topWinSize[1]);
    topWin = glutCreateWindow("PolyVR");

    glutReshapeFunc( onMainReshape );

    winUI = glutCreateSubWindow(topWin, 0, 0, topWinSize[0], topWinSize[1]);
    glutDisplayFunc( onUIDisplay );

    /*glutSetWindow(topWin);
    winGL = glutCreateSubWindow(topWin, 0,0,800,800);
    glutDisplayFunc( onGLDisplay );*/
}

void Glut::run() {
    cout << "Glut::run" << endl;
    glutMainLoop();
    signal("glutCleanup", {});

    // Cleanup
    //ImGui_ImplGLUT_Shutdown();
    //cleanupUI();
}
