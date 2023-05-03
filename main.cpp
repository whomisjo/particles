#include <SFML/Graphics.hpp>
#include <iostream>
#include "ComplexPlane.h"
using namespace std;
using namespace sf;

int main()
{
 
    VideoMode vm(1920, 1080);
    RenderWindow window(vm, "Mandelbrot", Style::Default);
    
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const float aspectRatio = static_cast<float>(desktop.height) / static_cast<float>(desktop.width);
    cout << aspectRatio;

    sf::Font font;										
    font.loadFromFile("fonts/KOMIKAP_.ttf");
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    
    enum class State
    {
        CALCULATING,
        DISPLAYING
    };
    State state = State::CALCULATING;

    ComplexPlane complexPlane;

    sf::VertexArray vArray(sf::Points, desktop.width* desktop.height);

    while (window.isOpen())
    {
        window.display();


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (Keyboard::isKeyPressed(Keyboard::Escape)) window.close();

            else if (event.type == sf::Event::MouseButtonPressed)
            {
                // Use mapPixelToCoords to find the Vector2f coordinate in the ComplexPlane View that corresponds to the screen mouse click
                const sf::Vector2f complexPlaneClick = complexPlane.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), window);

                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    // Left click zooms in
                    complexPlane.zoomIn(complexPlaneClick);
                }
                else if (event.mouseButton.button == sf::Mouse::Right)
                {
                    // Right click zooms out
                    complexPlane.zoomOut(complexPlaneClick);
                }
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                // Use mapPixelToCoords to find the Vector2f coordinate in the ComplexPlane View that corresponds to the screen mouse location
                const sf::Vector2f complexPlaneMouse = complexPlane.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), window);

                // Set the mouse location on the ComplexPlane object to store this coordinate
                complexPlane.setMouseLocation(complexPlaneMouse);
            }
        }

        if (state == State::CALCULATING)
        {
            int pixelWidth = desktop.width;
            int pixelHeight = desktop.height;

            for (int i = 0; i < pixelHeight; i++)
            {
                for (int j = 0; j < pixelWidth; j++)
                {
                    vArray[j + i * pixelWidth].position = { (float)j,(float)i };

                    sf::Vector2f coord = window.mapPixelToCoords(sf::Vector2i(j, i), complexPlane.getView());
                    int iterations = complexPlane.countIterations(coord);
                    Uint8 r, g, b;
                    complexPlane.iterationsToRGB(iterations, r, g, b);

                    vArray[j + i * pixelWidth].color = { r,g,b };
                }
            }

            state = State::DISPLAYING;
            complexPlane.loadText(text);
        }

        // Draw Scene segment
        window.clear();
        window.draw(vArray);
        window.draw(text);
        window.display();


        
    }
}




