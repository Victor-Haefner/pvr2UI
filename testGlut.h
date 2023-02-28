#ifndef TESTGLUT_H_INCLUDED
#define TESTGLUT_H_INCLUDED

// Dear ImGui: standalone example application for GLUT/FreeGLUT + OpenGL2, using legacy fixed pipeline
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// !!! GLUT/FreeGLUT IS OBSOLETE PREHISTORIC SOFTWARE. Using GLUT is not recommended unless you really miss the 90's. !!!
// !!! If someone or something is teaching you GLUT today, you are being abused. Please show some resistance. !!!
// !!! Nowadays, prefer using GLFW or SDL instead!

// On Windows, you can install Freeglut using vcpkg:
//   git clone https://github.com/Microsoft/vcpkg
//   cd vcpkg
//   bootstrap - vcpkg.bat
//   vcpkg install freeglut --triplet=x86-windows   ; for win32
//   vcpkg install freeglut --triplet=x64-windows   ; for win64
//   vcpkg integrate install                        ; register include and libs in Visual Studio

#include "testGL.h"
#include "testUI.h"

#include <algorithm>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_glut.h"
#include "backends/imgui_impl_opengl2.h"
#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#endif

using namespace std;

// Forward declarations of helper functions
void MainLoopStepUI();
void MainLoopStepGL();

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static int topWinSize[] = { 800, 800 };
static int topWin = -1;
static int winGL = -1;
static int winUI = -1;

void updateGLWindow() {
    cout << "updateGLWindow " << glArea.surface << endl;
    glutSetWindow(winGL);
    glutReshapeWindow(glArea.surface.width, glArea.surface.height);
    glutPositionWindow(glArea.surface.x, glArea.surface.y);
    glutSetWindow(winUI);
}

void on_resize_window(int w, int h) {
    topWinSize[0] = w;
    topWinSize[1] = h;

    glutSetWindow(winUI);
    glutReshapeWindow(topWinSize[0], topWinSize[1]);
    resizeUI({0,0,topWinSize[0], topWinSize[1]});

    glArea.surface.compute({ 0,0, topWinSize[0], topWinSize[1] }, glArea.layout);
    updateGLWindow();
}

int runGlut(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(topWinSize[0], topWinSize[1]);
    topWin = glutCreateWindow("Dear ImGui GLUT+OpenGL2 Example");

    glutReshapeFunc(on_resize_window);

    winUI = glutCreateSubWindow(topWin, 0, 0, topWinSize[0], topWinSize[1]);
    glutDisplayFunc(MainLoopStepUI);
    initUI();
    // Install GLUT handlers (glutReshapeFunc(), glutMotionFunc(), glutPassiveMotionFunc(), glutMouseFunc(), glutKeyboardFunc() etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    // FIXME: Consider reworking this example to install our own GLUT funcs + forward calls ImGui_ImplGLUT_XXX ones, instead of using ImGui_ImplGLUT_InstallFuncs().
    ImGui_ImplGLUT_Init();
    ImGui_ImplGLUT_InstallFuncs();


    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    glutSetWindow(topWin);
    winGL = glutCreateSubWindow(topWin, glArea.surface.x, glArea.surface.y, glArea.surface.width, glArea.surface.height);
    //winGL = glutCreateSubWindow(topWin, 0.5*800, 0, 0.7*800, 800);
    initScene();
    glutDisplayFunc(MainLoopStepGL);

    glutMainLoop();

    // Cleanup
    ImGui_ImplGLUT_Shutdown();
    cleanupUI();
    return 0;
}

void MainLoopStepGL() {
    glViewport(0, 0, glArea.surface.width, glArea.surface.height);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    renderScene();

    glutSwapBuffers();
    glutPostRedisplay();
}

void MainLoopStepUI() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();

    renderUI();

    // Rendering
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
    glutPostRedisplay();
}

#endif // TESTGLUT_H_INCLUDED
