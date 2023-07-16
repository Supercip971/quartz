
#include <iostream>
#include <llgraphics/gfx.hpp>
#include <llgraphics/vulkan/pipeline/shaders.hpp>
#include <stdlib.h>
#include <window/window.hpp>

#include "window/input.hpp"
int main(int argc, char **argv) {

    (void)argc;
    (void)argv;

    plt::Window::engine_init().unwrap();
    auto window = plt::Window::create("hello world", 1920, 1080).unwrap();

    window.set_root();

    plt::Gfx::init().unwrap();
    plt::Gfx::the()->attach(window).unwrap();

    window.add_callback(plt::WINDOW_CALLBACK_RESIZE, [](plt::Window &target, auto callback) {
        (void)callback;
        plt::Gfx::the()->resize(target).unwrap();
    });

    while (!window.should_close()) {
        window.update();

        plt::Gfx::the()->render().unwrap();
    }

    plt::Gfx::the()->cleanup();
    plt::Window::destroy(window);
    plt::Window::engine_cleanup();

    return 0;
}
