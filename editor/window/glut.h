#ifndef GLUT_H_INCLUDED
#define GLUT_H_INCLUDED

#include <functional>
#include <map>
#include <string>
#include "widget.h"

using namespace std;

class Glut {
    private:
        int topWinSize[2] = { 1200, 800 };
        int topWin = -1;
        int winGL = -1;
        int winUI = -1;

        typedef function<void(string, map<string, string>)> Signal;
        typedef function<void(string, Surface)> ResizeSignal;
        Signal signal;
        ResizeSignal resizeSignal;

    public:
        Glut();

        void init(int argc, char** argv, Signal signal, ResizeSignal resizeSignal);
        void run();

        void activateWindow(int i); // 1 top, 2 ui, 3 gl

        void on_resize_window(int w, int h);
        void on_ui_display();
        void on_gl_display();

        void updateGLWindow(Surface surface);
};

#endif // GLUT_H_INCLUDED
