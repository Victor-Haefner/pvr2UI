#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

#include <map>
#include <string>
#include <functional>

#include "window/glut.h"
#include "imgui/imgui.h"

using namespace std;

class Editor {
    private:
        typedef map<string, string> Options;
        typedef function< bool(Options) > Callback;
        typedef function< bool(Surface) > ResizeCallback;
        map<string, vector<Callback>> callbacks;
        map<string, vector<ResizeCallback>> resizeCallbacks;

        Glut glut;
        Imgui imgui;

        bool verbose = false;

        void runUI();

    public:
        Editor();
        ~Editor();

        void runWindow(int argc, char** argv);

        void addCallback(string name, Callback callback);
        void addResizeCallback(string name, ResizeCallback callback);
        bool trigger(string name, Options options = {});
        bool triggerResize(string name, Surface surface);

        void setVerbose(bool b);
};

#endif // EDITOR_H_INCLUDED
