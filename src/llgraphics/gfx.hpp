#pragma once

#include <utils/result.hpp>
#include <window/window.hpp>
namespace plt {

class Gfx {
public:
    static Result<> init();
    static Result<> attach(plt::Window &target);

    static Result<> render();

    static void cleanup(void);
};

} // namespace plt