#ifndef WIDGET_H_INCLUDED
#define WIDGET_H_INCLUDED

#include <string>

using namespace std;

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

    void compute(const Surface& parent, const Rectangle& area);
};

class Widget {
    public:
        string name;
        Rectangle layout;
        Surface surface;
        Surface parentSurface;

        Widget(string n, Rectangle r) : name(n), layout(r) {}

        void updateLayout(const Surface& newSize);
};

#endif // WIDGET_H_INCLUDED
