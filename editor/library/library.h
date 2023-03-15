#ifndef LIBRARY_H_INCLUDED
#define LIBRARY_H_INCLUDED

class Library {
    private:
        map<string, Launcher> launcher;

    public:
        Library();
        ~Library();

        void addLauncher();
};

#endif // LIBRARY_H_INCLUDED
