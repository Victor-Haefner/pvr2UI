#ifndef TESTUI_H_INCLUDED
#define TESTUI_H_INCLUDED

#include <string>
#include <iostream>
#include <math.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl2.h>

using namespace std;

class GLFWwindow;

void initUI() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplOpenGL2_Init();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

void cleanupUI() {
    ImGui_ImplOpenGL2_Shutdown();
    ImGui::DestroyContext();
}


bool show_main_pannel = true;
bool show_demo_window = false;

// https://gitlab.vci.rwth-aachen.de:9000/ptrettner/imgui-lean/-/blob/master/src/imgui/imgui_demo.cpp

struct Rectangle {
    float left = 0;
    float right = 1;
    float bottom = 0;
    float top = 1;
};

struct Surface {
    int x = 0;
    int y = 0;
    int width = 10;
    int height = 10;

    void compute(const Surface& parent, const Rectangle& area) {
        width  = round( parent.width * (area.right - area.left) );
        height = round( parent.height * (area.top - area.bottom) );
        x = round( parent.width * area.left );
        y = round( parent.height * (1.0 - area.top) );
    }
};

struct ResizeEvent {
    ImVec2 size;
    ImVec2 pos;

    char changed() {
        ImVec2 s = ImGui::GetWindowSize();
        ImVec2 p = ImGui::GetWindowPos();
        if (s.x == size.x && s.y == size.y) return 0;

        char edge = 0;
        if (p.x != pos.x && s.x != size.x) edge = 'L';
        if (p.x == pos.x && s.x != size.x) edge = 'R';
        if (p.y != pos.y && s.y != size.y) edge = 'T';
        if (p.y == pos.y && s.y != size.y) edge = 'B';

        size = s;
        pos = p;
        return edge;
    }
};

ostream& operator<<(ostream& os, const Rectangle& s) {
    os << "[" << s.left << ", " << s.right << ", " << s.top << ", " << s.bottom << "]";
    return os;
}

ostream& operator<<(ostream& os, const Surface& s) {
    os << "[" << s.x << ", " << s.y << ", " << s.width << ", " << s.height << "]";
    return os;
}

ostream& operator<<(ostream& os, const ResizeEvent& s) {
    os << "[" << s.pos.x << ", " << s.pos.y << ", " << s.size.x << ", " << s.size.y << "]";
    return os;
}

ostream& operator<<(ostream& os, const ImVec2& s) {
    os << "[" << s.x << ", " << s.y << "]";
    return os;
}

void resolveResize(const string& name, const ResizeEvent& resizer);

struct Widget {
    string name;
    Rectangle layout;
    Surface surface;
    Surface parentSurface;
    ResizeEvent resizer;
    ImGuiWindowFlags flags = 0;

    Widget(string n, Rectangle r) : name(n), layout(r) {
        resize({0,0,800,800});

        flags |= ImGuiWindowFlags_NoTitleBar;
        flags |= ImGuiWindowFlags_NoScrollbar;
        //flags |= ImGuiWindowFlags_MenuBar;
        flags |= ImGuiWindowFlags_NoMove;
        //flags |= ImGuiWindowFlags_NoResize;
        flags |= ImGuiWindowFlags_NoCollapse;
        flags |= ImGuiWindowFlags_NoNav;
        //flags |= ImGuiWindowFlags_NoBackground;
        //flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        //flags |= ImGuiWindowFlags_UnsavedDocument;
    }

    void begin() {
        //cout << " place widget " << surface << endl;
        ImGui::SetNextWindowPos(ImVec2(surface.x, surface.y)); // ImGuiCond_FirstUseEver
        ImGui::SetNextWindowSize(ImVec2(surface.width, surface.height));
        ImGui::Begin(name.c_str(), NULL, flags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigWindowsResizeFromEdges = true;

        char edge = resizer.changed();
        if (edge != 0) {
            //cout << " resize: " << resizer.size << ", " << resizer.pos << ", " << edge << endl;
            if (name == "Toolbar" && edge == 'B') resolveResize(name, resizer);
            if (name == "SidePannel" && (edge == 'T' || edge == 'R')) resolveResize(name, resizer);
            if (name == "Consoles" && (edge == 'T' || edge == 'L')) resolveResize(name, resizer);
        }

        // TODO: put the resizing in a ui manager and resize all layouts accordingly!
    }

    void end() {
        ImGui::End();
    }

    void resize(const Surface& parent) {
        parentSurface = parent;
        surface.compute(parent, layout);
        resizer.pos = ImVec2(surface.x, surface.y);
        resizer.size = ImVec2(surface.width, surface.height);
    }

    void updateLayout(const Surface& newSize) {
        //cout << " updateLayout " << newSize.y + newSize.height << "/800?   " << layout << ", parentSurface: " << parentSurface;
        layout.left  = float(newSize.x - parentSurface.x) / parentSurface.width;
        layout.right = float(newSize.x + newSize.width - parentSurface.x) / parentSurface.width;
        layout.top    = 1.0 - float(newSize.y - parentSurface.y) / parentSurface.height;
        layout.bottom = 1.0 - float(newSize.y + newSize.height - parentSurface.y) / parentSurface.height;
        surface.compute(parentSurface, layout);
        //cout << ", new size: " << newSize << " -> " << layout << endl;
    }
};

Widget toolbar("Toolbar", {0,1,0.9,1});
Widget sidePannel("SidePannel", {0,0.3,0,0.9});
Widget consoles("Consoles", {0.3,1.0,0,0.3});
Widget glArea("glArea", {0.3,1,0.3,0.9});

void updateGLWindow();

void resolveResize(const string& name, const ResizeEvent& resizer) {
    //cout << "resolveResize " << name << ", " << resizer << endl;
    if (name == "SidePannel") {
        sidePannel.updateLayout({ resizer.pos.x, resizer.pos.y, resizer.size.x, resizer.size.y });
        consoles.layout.left = sidePannel.layout.right;
        consoles.resize(consoles.parentSurface);
        toolbar.layout.bottom = sidePannel.layout.top;
        toolbar.resize(toolbar.parentSurface);
        glArea.layout.left = sidePannel.layout.right;
        glArea.layout.top = sidePannel.layout.top;
        glArea.resize(glArea.parentSurface);
        updateGLWindow();
        // TODO: resize widget
    }

    if (name == "Consoles") {
        consoles.updateLayout({ resizer.pos.x, resizer.pos.y, resizer.size.x, resizer.size.y });
        sidePannel.layout.right = consoles.layout.left;
        sidePannel.resize(sidePannel.parentSurface);
        glArea.layout.left = consoles.layout.left;
        glArea.layout.bottom = consoles.layout.top;
        glArea.resize(glArea.parentSurface);
        updateGLWindow();
        // TODO: resize widget
    }
}

void renderUI() {

    //ImGui::ShowDemoWindow(&show_demo_window);

    //toolbar.flags |= ImGuiWindowFlags_MenuBar;

    toolbar.begin();
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Test"));
    toolbar.end();

    sidePannel.begin();
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Test"));
    sidePannel.end();

    consoles.begin();
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Test"));
    consoles.end();

    // Render dear imgui into screen
    ImGui::Render();
}

void resizeUI(const Surface& parent) {
    toolbar.resize(parent);
    sidePannel.resize(parent);
    consoles.resize(parent);
}

#endif // TESTUI_H_INCLUDED
