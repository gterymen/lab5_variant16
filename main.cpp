#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdio>
#include <optional>
#include <string>
#include "Physics.hpp"

namespace
{
constexpr float kScale = 70.f;
constexpr float kSurfaceY = 250.f;
constexpr float kBodyLeft = 560.f;
constexpr float kBodyWidth = 100.f;
}

struct AppState
{
    BuoyancyModel model;
    bool running = true;
};

bool loadFont(sf::Font& font)
{
    const char* paths[] = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    for (const char* path : paths)
    {
        if (font.openFromFile(path))
        {
            return true;
        }
    }
    return false;
}

void handleInput(sf::RenderWindow& window, AppState& state)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            BuoyancyModel& m = state.model;
            switch (key->code)
            {
            case sf::Keyboard::Key::Space:
                state.running = !state.running;
                break;
            case sf::Keyboard::Key::R:
                m.reset();
                state.running = true;
                break;
            case sf::Keyboard::Key::Up:
                m.bodyDensity = std::min(m.bodyDensity + 50.0, 3000.0);
                break;
            case sf::Keyboard::Key::Down:
                m.bodyDensity = std::max(m.bodyDensity - 50.0, 50.0);
                break;
            case sf::Keyboard::Key::Right:
                m.liquidDensity = std::min(m.liquidDensity + 100.0, 14000.0);
                break;
            case sf::Keyboard::Key::Left:
                m.liquidDensity = std::max(m.liquidDensity - 100.0, 200.0);
                break;
            case sf::Keyboard::Key::Escape:
                window.close();
                break;
            default:
                break;
            }
        }
    }
}

void updatePhysics(AppState& state, float dt)
{
    if (state.running)
    {
        state.model.advance(dt);
    }
}

std::string buildInfo(const AppState& state)
{
    const BuoyancyModel& m = state.model;
    const char* verdict = "floats / oscillates";
    if (m.bodyDensity > m.liquidDensity)
    {
        verdict = "heavier than liquid: SINKS";
    }
    else if (m.bodyDensity == m.liquidDensity)
    {
        verdict = "neutral buoyancy";
    }
    char buffer[768];
    std::snprintf(buffer, sizeof(buffer),
        "BUOYANCY (variant 16)   [%s]\n"
        "Body density: %.0f kg/m^3   (Up/Down)\n"
        "Liquid density: %.0f kg/m^3   (Left/Right)\n"
        "Ratio body/liquid: %.2f -> %s\n"
        "Height of body bottom: %.2f m\n"
        "Velocity (up is +): %.2f m/s\n"
        "Submerged: %.0f %% (equilibrium %.0f %%)\n"
        "Weight: %.0f N, Archimedes: %.0f N\n"
        "Time: %.2f s\n"
        "Space - pause/run, R - restart",
        state.running ? "RUNNING" : "PAUSED",
        m.bodyDensity, m.liquidDensity,
        m.bodyDensity / m.liquidDensity, verdict,
        m.y, m.velocity,
        m.submergedFraction() * 100.0, m.equilibriumFraction() * 100.0,
        m.weight(), m.archimedesForce(), m.time);
    return buffer;
}

void drawScene(sf::RenderWindow& window, const sf::Font& font, const AppState& state)
{
    const BuoyancyModel& m = state.model;
    window.clear(sf::Color(25, 28, 38));

    float poolBottom = kSurfaceY + static_cast<float>(m.poolDepth) * kScale;
    sf::RectangleShape water({800.f, poolBottom - kSurfaceY});
    water.setFillColor(sf::Color(40, 110, 190, 170));
    water.setPosition({0.f, kSurfaceY});
    window.draw(water);

    sf::RectangleShape floor({800.f, 600.f - poolBottom});
    floor.setFillColor(sf::Color(70, 70, 80));
    floor.setPosition({0.f, poolBottom});
    window.draw(floor);
float bodyH = static_cast<float>(m.bodyHeight) * kScale;
    float top = kSurfaceY - static_cast<float>(m.y + m.bodyHeight) * kScale;
    sf::RectangleShape body({kBodyWidth, bodyH});
    body.setFillColor(sf::Color(220, 150, 60));
    body.setOutlineColor(sf::Color::White);
    body.setOutlineThickness(1.f);
    body.setPosition({kBodyLeft, top});
    window.draw(body);

    sf::RectangleShape line({800.f, 2.f});
    line.setFillColor(sf::Color(200, 230, 255));
    line.setPosition({0.f, kSurfaceY});
    window.draw(line);

    sf::Text info(font, buildInfo(state), 16);
    info.setFillColor(sf::Color::White);
    info.setPosition({20.f, 12.f});
    window.draw(info);

    window.display();
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML Physics Demo - Buoyancy (variant 16)");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!loadFont(font))
    {
        return 1;
    }

    AppState state;
    sf::Clock clock;

    while (window.isOpen())
    {
        handleInput(window, state);
        float dt = std::min(clock.restart().asSeconds(), 0.05f);
        updatePhysics(state, dt);
        drawScene(window, font, state);
    }

    return 0;
}
