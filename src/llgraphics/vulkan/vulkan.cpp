
#include <llgraphics/gfx.hpp>

#include "utils/log.hpp"
#include "window/window.hpp"

namespace plt {

Result<> Gfx::init() {

    debug$("initializing gfx");
    return {};
}

Result<> Gfx::attach(plt::Window &target) {
    debug$("attaching to window: {}", target.handle());

    (void)target;
    return {};
}
void Gfx::cleanup(void) {
    debug$("cleaning up gfx");
}
Result<> Gfx::render(void) {
    return {};
}
}; // namespace plt
