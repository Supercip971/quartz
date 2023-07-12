
#pragma once
namespace plt {

struct QueueFamilyIndices {
    bool has_value = false;
    int graphicsFamily = -1;

    QueueFamilyIndices() = default;
    QueueFamilyIndices(int graphicsFamily) : has_value(true), graphicsFamily(graphicsFamily) {}

    operator bool() const {
        return has_value;
    }

    bool isComplete() const {
        return has_value;
    }
};

} // namespace plt
