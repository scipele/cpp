/* Compile in Windows:
g++ -std=c++17 -o draw_lines_with_eyeball_pattern draw_lines_with_eyeball_pattern.cpp -lsfml-graphics -lsfml-window -lsfml-system

Compile in Linux:
g++ -std=c++17 -o draw_lines_with_eyeball_pattern draw_lines_with_eyeball_pattern.cpp -lsfml-graphics -lsfml-window -lsfml-system
*/
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

int main() {
    // 1. Create the window using sf::Vector2u for dimensions (SFML 3 requirement)
    sf::RenderWindow window(sf::VideoMode({1920, 1080}), "My Window", sf::Style::Default);
    window.setFramerateLimit(60);

    // 2. Define border coordinates
    const float xmin = 10.f;
    const float ymin = 10.f;
    const float xmax = 1890.f;
    const float ymax = 990.f;

    // Helper lambda to draw basic lines using updated Vertex initializing & PrimitiveType
    auto drawLine = [&](sf::RenderWindow& win, float x1, float y1, float x2, float y2, sf::Color color) {
        sf::Vertex line[] = {
            sf::Vertex{.position = {x1, y1}, .color = color},
            sf::Vertex{.position = {x2, y2}, .color = color}
        };
        win.draw(line, 2, sf::PrimitiveType::Lines);
    };

    // Main application loop
    while (window.isOpen()) {
        // Updated SFML 3 type-safe event polling system
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Clear screen with a background color
        window.clear(sf::Color::White);

        // --- DRAW BORDER ---
        sf::Color borderColor = sf::Color::Black;
        drawLine(window, xmin, ymin, xmax, ymin, borderColor);
        drawLine(window, xmax, ymin, xmax, ymax, borderColor);
        drawLine(window, xmax, ymax, xmin, ymax, borderColor);
        drawLine(window, xmin, ymax, xmin, ymin, borderColor);

        // --- DRAW MATH/LINE ART ---
        int n = 100;
        const float x_incr = (xmax - xmin) / n;
        const float y_incr = (ymax - ymin) / n;

        for (int i = 0; i < n; i++) {
            float y = 10.f + y_incr * i;
            float x = 10.f + x_incr * i;

            // Line 1: Red gradient
            drawLine(window, xmin, y, x, ymax, sf::Color(i * 255 / n, 0, 0));

            // Line 2: Green gradient
            drawLine(window, x, ymax, xmax, (ymax - y), sf::Color(0, i * 255 / n, 0));

            // Line 3: Blue gradient
            drawLine(window, xmin, (ymax - y), x, ymin, sf::Color(0, 0, i * 255 / n));

            // Line 4: Custom gradient
            drawLine(window, x, ymin, xmax, y, sf::Color(i * 255 / n, 255 - i * 255 / n, 255 - i * 255 / n));
        }

        // --- DRAW CIRCLES ---
        float xcen = (xmax - xmin) / 2.f;
        float ycen = (ymax - ymin) / 2.f;
        float max_dia = 750.f;

        // Non-filled concentric circles
        for (int i = static_cast<int>(max_dia); i > 225; i -= 1) {
            float radius = i / 2.f;
            sf::CircleShape circle(radius);
            
            // Set position with vector syntax (SFML 3 requirement)
            circle.setPosition({xcen - radius, ycen - radius});
            
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineThickness(1.0f);
            circle.setOutlineColor(sf::Color(27, 50 + i * 100 / max_dia, 155 + i * 100 / max_dia));
            
            window.draw(circle);
        }

        // Center filled black circle
        float blackRadius = 225.f / 2.f;
        sf::CircleShape blackCircle(blackRadius);
        blackCircle.setPosition({xcen - blackRadius, ycen - blackRadius});
        blackCircle.setFillColor(sf::Color::Black);
        
        window.draw(blackCircle);

        // Display everything on screen
        window.display();
    }

    return 0;
}