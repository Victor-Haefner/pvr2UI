#include "editor.h"
#include "window/testGL.h"

#include <iostream>

Editor::Editor() {}

Editor::~Editor() {}

void Editor::runWindow(int argc, char** argv) {
    auto sig = [&](string name, map<string,string> opts) -> bool { return trigger(name,opts); };
    auto sig2 = [&](string name, Surface s) -> bool { return triggerResize(name,s); };
    glut.init(argc, argv, sig, sig2);
    initTestScene();
    imgui.init(sig, sig2);

    addCallback("glutRenderUI", [&](Options){ imgui.render(); return true; } );
    addCallback("glutRenderGL", [&](Options){ imgui.renderGLArea(); return true; } );
    addCallback("widgetResize", [&](Options o){ imgui.onWidgetResize(o); return true; } );
    addResizeCallback("glutResize", [&](Surface s){ imgui.resizeUI(s); return true; } );
    addResizeCallback("glAreaResize", [&](Surface s){ glut.updateGLWindow(s); return true; } );

    verbose = true;

    glut.run();
}

void Editor::setVerbose(bool b) { verbose = b; }

void Editor::addCallback(string name, Callback callback) {
    cout << "Editor::addCallback " << name << endl;
    callbacks[name].push_back( callback );
}

void Editor::addResizeCallback(string name, ResizeCallback callback) {
    cout << "Editor::addResizeCallback " << name << endl;
    resizeCallbacks[name].push_back( callback );
}

bool Editor::trigger(string name, Options options) {
    if (!callbacks.count(name)) {
        if (verbose) cout << " ..no callbacks, skip " << name << endl;
        return false;
    }

    for (auto& callback : callbacks[name]) {
        bool b = callback(options);
        if (!b) break;
    }

    return true;
}

bool Editor::triggerResize(string name, Surface surface) {
    if (!resizeCallbacks.count(name)) {
        if (verbose) cout << " ..no resize callbacks, skip " << name << endl;
        return false;
    }

    for (auto& callback : resizeCallbacks[name]) {
        bool b = callback(surface);
        if (!b) break;
    }

    return true;
}

void Editor::runUI() {
}
