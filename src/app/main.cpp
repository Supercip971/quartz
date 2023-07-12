
#include <iostream>
#include <llgraphics/gfx.hpp>
#include <stdlib.h>
#include <window/window.hpp>
int main(void) {

    plt::Window::engine_init().unwrap();
    auto window = plt::Window::create("hello world", 1920, 1080).unwrap();

    window.set_root();

    plt::Gfx::init().unwrap();
    plt::Gfx::the()->attach(window).unwrap();

    while (!window.should_close()) {
        window.update();
        plt::Gfx::the()->render().unwrap();
    }

    plt::Gfx::the()->cleanup();
    plt::Window::destroy(window);
    plt::Window::engine_cleanup();

    return 0;
}
