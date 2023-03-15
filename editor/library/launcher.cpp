#include "launcher.h"

Launcher::Launcher(string label, string path) : label(label), path(path) {}
Launcher::~Launcher() {}

void Launcher::start() { running = true; }
void Launcher::stop() { running = false; }
bool Launcher::state() { return running; }

void Launcher::lock(bool b) { locked = b; }
