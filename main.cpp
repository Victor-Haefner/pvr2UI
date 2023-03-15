
/*#include "testGLFW.h"
int main() { return runGLFW(); }*/

#include "editor/editor.h"

int main(int argc, char** argv) {
    Editor editor;
    editor.runWindow(argc, argv);
    return 0;
}
