#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

// Function to create a bowling ball swirl texture
sf::Texture createSwirlTexture(int size) {
    sf::Image image;
    image.create(size, size, sf::Color::Black);
    
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            float dx = x - size / 2.0f;
            float dy = y - size / 2.0f;
            float distance = std::sqrt(dx * dx + dy * dy);
            float angle = std::atan2(dy, dx);
            
            // Adjust color calculations
            int blue = static_cast<int>(150 + 100 * std::sin(angle * 8 + distance / 5.0)); // Strong blue
            int red = static_cast<int>(100 + 80 * std::cos(angle * 4 + distance / 7.0));  // Red tint for purple
            int green = 5; // Keep green VERY low to remove unwanted tints
 
            image.setPixel(x, y, sf::Color(red, green, blue));
        }
    }

    sf::Texture swirlTexture;
    swirlTexture.loadFromImage(image);
    swirlTexture.setSmooth(true);
    return swirlTexture;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(3600, 1200), "Bowling Ball with Tilted Spin", sf::Style::Close);
    window.setFramerateLimit(60);
    
    int ballRadius = 150;
    sf::Texture ballTexture = createSwirlTexture(ballRadius * 2);

    // Use CircleShape to ensure roundness
    sf::CircleShape ball(ballRadius);
    ball.setTexture(&ballTexture);
    ball.setOrigin(ballRadius, ballRadius);
    ball.setPosition(200, 300);

    float velocity = 5.0f;  // Forward motion
    float spinSpeed = 5.0f; // Base spin speed
    float spinAngle = 20.0f; // Rotation angle

    float tiltAngle = 30.0f * (3.14159f / 180.0f);  // Convert tilt angle to radians
    float spinFactor = 1.2f; // Controls wobble effect

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Move the ball forward
        ball.move(velocity, 0);

        // Create the tilted spin effect by modifying the rotation dynamically
        spinAngle += spinSpeed;
        float adjustedAngle = spinAngle + spinFactor * std::sin(spinAngle * 0.1f) * std::cos(tiltAngle);
        ball.setRotation(adjustedAngle);

        // Clear, draw, and display
        window.clear(sf::Color::White);
        window.draw(ball);
        window.display();
    }

    return 0;
}
