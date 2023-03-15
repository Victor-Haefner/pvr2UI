#ifndef LAUNCHER_H_INCLUDED
#define LAUNCHER_H_INCLUDED

#include <string>

using namespace std;

class Launcher {
    private:
        string label;
        string path;
        bool running = false;
        bool locked = false;

    public:
        Launcher(string label, string path);
        ~Launcher();

        void start();
        void stop();
        bool state();

        void lock(bool b);
};

#endif // LAUNCHER_H_INCLUDED
