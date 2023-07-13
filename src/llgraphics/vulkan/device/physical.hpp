
#pragma once
namespace plt {

struct QueueFamilyIndice {
    bool has_value = false;
    int index = -1;

    QueueFamilyIndice() = default;
    QueueFamilyIndice(int idx) : has_value(true), index(idx) {}

    operator bool() const {
        return has_value;
    }

    bool isComplete() const {
        return has_value;
    }
};

struct QueueFamilyIndices {
    QueueFamilyIndice graphicsFamily;
    QueueFamilyIndice presentFamily;

    QueueFamilyIndices() = default;
    QueueFamilyIndices(QueueFamilyIndice graphicsFamily) : graphicsFamily(graphicsFamily) {}

    QueueFamilyIndices(QueueFamilyIndice graphicsFamily, QueueFamilyIndice presentFamily) : graphicsFamily(graphicsFamily), presentFamily(presentFamily) {}

    QueueFamilyIndices withPresentFamily(QueueFamilyIndice presentFamily) {
        return QueueFamilyIndices(graphicsFamily, presentFamily);
    }

    QueueFamilyIndices withGraphicsFamily(QueueFamilyIndice graphicsFamily) {
        return QueueFamilyIndices(graphicsFamily, presentFamily);
    }

    operator bool() const {
        return graphicsFamily && presentFamily;
    }

    bool isComplete() const {
        return graphicsFamily && presentFamily;
    }
};

} // namespace plt
