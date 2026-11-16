#pragma once

#include <algorithm>
#include <cmath>

// Body: box with height H and base area A. Axis y points UP, liquid surface at y = 0,
// pool floor at y = -poolDepth. 'y' is the height of the body's bottom face.
struct BuoyancyModel
{
    double gravity = 9.81;
    double bodyDensity = 600.0;
    double liquidDensity = 1000.0;
    double bodyHeight = 1.0;
    double bodyArea = 1.0;
    double poolDepth = 4.0;
    double startY = 1.5;
    double startVelocity = 0.0;

    double y = 1.5;
    double velocity = 0.0;
    double time = 0.0;
    bool onFloor = false;

    BuoyancyModel()
    {
        reset();
    }

    void reset()
    {
        y = startY;
        velocity = startVelocity;
        time = 0.0;
        onFloor = false;
    }

    double mass() const
    {
        return bodyDensity * bodyArea * bodyHeight;
    }

    double submergedDepth() const
    {
        double depth = std::min(0.0, y + bodyHeight) - std::min(0.0, y);
        return std::clamp(depth, 0.0, bodyHeight);
    }

    double submergedFraction() const
    {
        return submergedDepth() / bodyHeight;
    }

    double archimedesForce() const
    {
        return liquidDensity * gravity * bodyArea * submergedDepth();
    }

    double weight() const
    {
        return mass() * gravity;
    }

    double equilibriumFraction() const
    {
        return std::min(1.0, bodyDensity / liquidDensity);
    }

    void step(double dt)
    {
        if (onFloor)
        {
            if (archimedesForce() > weight())
            {
                onFloor = false;
            }
            else
            {
                return;
            }
        }
        double acceleration = (archimedesForce() - weight()) / mass();
        velocity += acceleration * dt;
        y += velocity * dt;
        time += dt;
        if (y <= -poolDepth)
        {
            y = -poolDepth;
            velocity = 0.0;
            onFloor = true;
        }
    }

    void advance(double frameDt, double maxStep = 0.001)
    {
        int n = std::max(1, static_cast<int>(std::ceil(frameDt / maxStep)));
        double h = frameDt / n;
        for (int i = 0; i < n; ++i)
        {
            step(h);
        }
    }
};
