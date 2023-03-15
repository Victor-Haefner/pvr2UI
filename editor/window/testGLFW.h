#ifndef TESTGLFW_H_INCLUDED
#define TESTGLFW_H_INCLUDED



#include "testGL.h"
#include "testUI.h"

#include <iostream>
#include <chrono>
#include <thread>

#include <wayland-client.h>
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


using namespace std;

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLFWwindow* setupWindow(string title, GLFWwindow* parent = NULL) {
    GLFWwindow* window = glfwCreateWindow(640, 480, title.c_str(), NULL, parent);
    if (!window) return 0;
    glfwSetKeyCallback(window, key_callback);
    return window;
}

struct wl_subcompositor* subcompositor = 0;
struct wl_compositor* compositor = 0;
uint32_t subcompositorName = 0;

static void registryListenerGlobal(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version)
{
    if (strcmp(interface, "wl_subcompositor") == 0)
    {
        subcompositor = (wl_subcompositor*)wl_registry_bind(registry, id, &wl_subcompositor_interface, 1);
        subcompositorName = id;
    }
    else if (strcmp(interface, "wl_compositor") == 0)
    {
        compositor = (wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 4);
    }
}


int runGLFW() {
    cout << "hello world!" << endl;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return 1;

    GLFWwindow* winUI = setupWindow("Editor");
    glfwMakeContextCurrent(winUI);
    glfwSwapInterval(1); // V-Sync

    initUI();
    ImGui_ImplGlfw_InitForOpenGL(winUI, true);

    // create editor window
    float view[] = {0.3,1.0,0.2,0.9};

    glfwWindowHint(GLFW_DECORATED, false);
    glfwWindowHint(GLFW_VISIBLE, false);
    //glfwWindowHint(GLFW_FLOATING, true); // will put it over all other apps as well.. maybe coupled with focus callback?
    GLFWwindow* winGL = setupWindow("PolyVR 2.0", winUI);

    wl_surface* winUInative = glfwGetWaylandWindow(winUI);
    wl_surface* winGLnative = glfwGetWaylandWindow(winGL);

    // Get the Wayland display and registry
    struct wl_display* display = wl_display_connect(NULL);
    struct wl_registry* registry = wl_display_get_registry(display);
    wl_registry_listener registryListener;
    registryListener.global = registryListenerGlobal;
    wl_registry_add_listener(registry, &registryListener, NULL);
    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    // Create a subcompositor object
    struct wl_subcompositor* subcompositor = (wl_subcompositor*)wl_registry_bind(registry, subcompositorName, &wl_subcompositor_interface, 1);

    // Create a sub-surface for the subwindow
    struct wl_surface* subSurfaceParent = wl_compositor_create_surface(compositor);
    struct wl_subsurface* subSurfaceSub = wl_subcompositor_get_subsurface(subcompositor, winGLnative, subSurfaceParent);
    wl_subsurface_set_position(subSurfaceSub, 100, 100);

    // Set the parent of the main window to the root window
    wl_surface_attach(winUInative, wl_compositor_create_surface(compositor), 0, 0);
    wl_surface_commit(winUInative);

    // Set the parent of the sub-surface to the main window's surface
    wl_surface_attach(subSurfaceParent, winUInative, 0, 0);
    wl_surface_commit(subSurfaceParent);

    //winGL->SetParent
    // Place the second window to the right of the first
    {
        int xpos, ypos, left, right, width, height;
        glfwGetWindowSize(winUI, &width, &height);
        glfwGetWindowFrameSize(winUI, &left, NULL, &right, NULL);
        glfwGetWindowPos(winUI, &xpos, &ypos);

        int eW = (view[1]-view[0])*width;
        int eH = (view[3]-view[2])*height;
        int eX = xpos + view[0]*width;
        int eY = ypos + view[2]*height;
        glfwSetWindowPos(winGL, eX, eY);
        glfwSetWindowSize(winGL, eW, eH);
    }
    glfwShowWindow(winGL);

    glfwMakeContextCurrent(winGL);
    initScene();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



    while (!glfwWindowShouldClose(winUI)) {
        glfwMakeContextCurrent(winUI);
        glfwPollEvents();

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        renderUI1();

        int display_w, display_h;
        glfwGetFramebufferSize(winUI, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(winUI);



        glfwMakeContextCurrent(winGL);
        glfwPollEvents();

        glfwGetFramebufferSize(winGL, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        renderScene();
        glfwSwapBuffers(winGL);



        this_thread::sleep_for(chrono::milliseconds(17));
    }

    glfwMakeContextCurrent(winUI);
    ImGui_ImplGlfw_Shutdown();
    cleanupUI();

    glfwDestroyWindow(winUI);
    glfwDestroyWindow(winGL);
    glfwTerminate();
}


#endif // TESTGLFW_H_INCLUDED
