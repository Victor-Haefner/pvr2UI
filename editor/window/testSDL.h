#ifndef TESTSDL_H_INCLUDED
#define TESTSDL_H_INCLUDED

// TODO: this is from chatgpt, test it!
/*
In this code, we first initialize GLFW and open a Wayland display connection using the wl_display_connect function. We then get the Wayland wl_compositor interface using the wl_display_get_compositor function, and create a new wl_surface using the wl_compositor_create_surface function.

We then create a GLFW window using the glfwCreateWindow function, and get the native Wayland window handle using the glfwGetWaylandWindow function. We attach the Wayland surface to the native window using the wl_egl_window_resize, wl_egl_window_set_title, wl_surface_attach, and wl_surface_commit functions.

Finally, we enter the main event loop using glfwPollEvents, render OpenGL content to the GLFW window using glfwSwapBuffers, and clean up the GLFW and Wayland resources using glfwTerminate, wl_surface_destroy, and wl_display_disconnect.
*/

#include <SDL.h>

int main(int argc, char* argv[]) {
    // Initialize SDL2
    SDL_Init(SDL_INIT_VIDEO);

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Main Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        // Handle error
    }

    // Create a renderer for the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        // Handle error
    }

    // Create a subwindow as a render target
    SDL_Rect subWindowRect = { 100, 100, 200, 200 };
    SDL_Renderer* subRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderTarget(renderer, subRenderer);

    // Clear the subwindow to red
    SDL_SetRenderDrawColor(subRenderer, 255, 0, 0, 255);
    SDL_RenderClear(subRenderer);

    // Present the subwindow to the main window
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, SDL_GetRendererTarget(subRenderer), NULL, &subWindowRect);

    // Main event loop
    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Render content to the main window
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    // Clean up SDL2 resources
    SDL_DestroyRenderer(subRenderer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


#endif // TESTSDL_H_INCLUDED
