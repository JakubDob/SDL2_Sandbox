#include "HealthBar.h"
#include "../../Types.h"
#include "Transform2D.h"
#include <iostream>
#include "../../opengl/VertexBufferLayout.h"

HealthBar::HealthBar(scalar_t maxValue, float floatHeight)
    : maxValue(maxValue), currentValue(maxValue) {
    mesh =
        QuadMesh(QuadMesh::center, {-width * 0.5f, floatHeight, width, height});
}

void HealthBar::changeHp(scalar_t amount) {
    std::cout << "current hp: " << currentValue << "\n";
    currentValue += amount;
    if (currentValue > maxValue) {
        currentValue = maxValue;
    } else if (currentValue < 0) {
        currentValue = 0;
    }
}

void HealthBar::reset() { currentValue = maxValue; }

scalar_t HealthBar::getCurrentValue() const { return currentValue; }

scalar_t HealthBar::getMaxValue() const { return maxValue; }

QuadMesh const& HealthBar::getMesh(bool flipY) {
    for (size_t i = 0; i < 4; ++i) {
        /*
        use empty vertex attribute slots to set scale and hp percentage for the
        shader shift -w/2..w/2 -> 0..w and normalize -> 0..w -> 0..1
        interpolated in the shader functions as a scale to compare current hp
        percentage
        */
        mesh[i].setRed((mesh[i].getX() + width * 0.5f) / width);
        mesh[i].setGreen(currentValue / maxValue);
    }
    if (flipY) {
        mesh.flipY();
    }
    return mesh;
}