#include "imgui.h"

#include "../window/testGL.h"

#include <iostream>
#include <GL/glew.h>

#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl2.h>

ostream& operator<<(ostream& os, const ResizeEvent& s) {
    os << "[" << s.pos.x << ", " << s.pos.y << ", " << s.size.x << ", " << s.size.y << "]";
    return os;
}

ostream& operator<<(ostream& os, const ImVec2& s) {
    os << "[" << s.x << ", " << s.y << "]";
    return os;
}

ostream& operator<<(ostream& os, const Surface& s);

void Imgui::init(Signal signal, ResizeSignal resizeSignal) {
    this->signal = signal;
    this->resizeSignal = resizeSignal;

    cout << "Imgui::init" << endl;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplOpenGL2_Init();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();
    ImGui_ImplGLUT_InstallFuncs();

    toolbar.signal = signal;
    sidePannel.signal = signal;
    consoles.signal = signal;
    glArea.signal = signal;
}

void Imgui::close() {
    cout << "Imgui::close" << endl;
    ImGui_ImplOpenGL2_Shutdown();
    ImGui::DestroyContext();
}

void Imgui::resolveResize(const string& name, const ResizeEvent& resizer) {
    cout << "     resolveResize " << name << ", " << resizer << endl;
    if (name == "SidePannel") {
        sidePannel.updateLayout({ resizer.pos.x, resizer.pos.y, resizer.size.x, resizer.size.y });
        consoles.layout.left = sidePannel.layout.right;
        consoles.resize(consoles.parentSurface);
        toolbar.layout.bottom = sidePannel.layout.top;
        toolbar.resize(toolbar.parentSurface);
        glArea.layout.left = sidePannel.layout.right;
        glArea.layout.top = sidePannel.layout.top;
        glArea.resize(glArea.parentSurface);
        resizeSignal("glAreaResize", glArea.surface);
    }

    if (name == "Consoles") {
        consoles.updateLayout({ resizer.pos.x, resizer.pos.y, resizer.size.x, resizer.size.y });
        sidePannel.layout.right = consoles.layout.left;
        sidePannel.resize(sidePannel.parentSurface);
        glArea.layout.left = consoles.layout.left;
        glArea.layout.bottom = consoles.layout.top;
        glArea.resize(glArea.parentSurface);
        resizeSignal("glAreaResize", glArea.surface);
    }
}

void Imgui::renderSidePannel() {
    sidePannel.begin();
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
        if (ImGui::BeginTabItem("Apps")) {
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Setup")) {
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Scene")) {
            editor.Render("Editor");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    sidePannel.end();
}

void Imgui::renderToolbar() {
    toolbar.begin();
    if (ImGui::Button("New"));
    ImGui::SameLine();
    if (ImGui::Button("Open"));
    ImGui::SameLine();
    if (ImGui::Button("Save"));
    ImGui::SameLine();
    if (ImGui::Button("Save.."));
    ImGui::SameLine();
    if (ImGui::Button("Close"));
    ImGui::SameLine();
    if (ImGui::Button("Exit"));
    ImGui::SameLine();
    if (ImGui::Button("About"));
    ImGui::SameLine();
    if (ImGui::Button("Stats"));
    toolbar.end();
}

void Imgui::renderConsoles() {
    consoles.begin();
    consoles.end();
}

void Imgui::resizeUI(const Surface& parent) {
    cout << "    Imgui::resizeUI " << endl;
    toolbar.resize(parent);
    sidePannel.resize(parent);
    consoles.resize(parent);
    glArea.resize(parent);
    if (resizeSignal) resizeSignal("glAreaResize", glArea.surface);
}

void Imgui::resizeGL(const Surface& parent) { // on resize
    cout << "    Imgui::resizeGL " << endl;
    glAreaWindow = parent;
}

void Imgui::onWidgetResize(map<string,string> options) {
    string name = options["name"];
    char edge = options["edge"][0];
    cout << "    Imgui::onWidgetResize " << name << ", " << edge << endl;
    if (name == "Toolbar" && edge == 'B') resolveResize(name, toolbar.resizer);
    if (name == "SidePannel" && (edge == 'T' || edge == 'R')) resolveResize(name, sidePannel.resizer);
    if (name == "Consoles" && (edge == 'T' || edge == 'L')) resolveResize(name, consoles.resizer);
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void Imgui::render() {
    ImGuiIO& io = ImGui::GetIO();
    if (io.DisplaySize.x < 0 || io.DisplaySize.y < 0) return;
    cout << "   Imgui::render " << endl;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();

    renderToolbar();
    renderSidePannel();
    renderConsoles();

    //ImGui::ShowDemoWindow(0);

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);

    cout << "    Imgui::render draw" << endl;
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    //cout << " Imgui::render " << (GLsizei)io.DisplaySize.x << ", " << (GLsizei)io.DisplaySize.y << endl;
}

void Imgui::renderGLArea() {
    tmpC += 0.1;
    //cout << "   Imgui::renderGLArea " << w << ", " << h << " ... " << glAreaWindow << endl;
    glViewport(0, 0, glAreaWindow.width, glAreaWindow.height);
    //glViewport(0, glAreaWindow.height-300, glAreaWindow.width, 300);
    //glViewport(0, h-300, w, 300);
    glClearColor(tmpC * clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    renderScene();
}


