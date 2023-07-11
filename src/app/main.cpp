
#include <iostream>
#include <stdlib.h>
#include <window/window.hpp>
int main(void) {

    plt::Window::engine_init().unwrap();
    auto window = plt::Window::create("hello world", 1920, 1080).unwrap();

    window.set_root();

    while (!window.should_close()) {
        window.update();
    }

    plt::Window::destroy(window);
    plt::Window::engine_cleanup();

    return 0;
}
