#pragma once

#include <utils/result.hpp>
#include <window/window.hpp>
namespace plt {

class Gfx {
public:
    static Result<> init();
    Result<> attach(plt::Window &target);

    Result<> render();

    Result<> resize(plt::Window &target);

    void cleanup(void);

    static Gfx *the();
};

} // namespace plt
