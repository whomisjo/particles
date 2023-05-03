#include "Engine.h"

Engine::Engine()
{
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	int screenWidth = desktop.width;
	int screenHeight = desktop.height;
	m_Window.create(sf::VideoMode(screenWidth, screenHeight), "Particles");
}


void Engine::run()
{
    sf::Clock clock;

    // local Particle to be used for the unit tests
    std::cout << "Starting Particle unit tests..." << std::endl;
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    std::cout << "Unit tests complete. Starting engine..." << std::endl;


    while (m_Window.isOpen())
    {
        // Restart the clock
        sf::Time deltaTime = clock.restart();
        // Convert the clock time to seconds
        float dtAsSeconds = deltaTime.asSeconds();

        input();
        update(dtAsSeconds);
        draw();
    }
}


void Engine::input()
{
    sf::Event event;
    while (m_Window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_Window.close();
        }
        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        {
            m_Window.close();
        }
        else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {
            for (int i = 0; i < 5; i++)
            {
                int numPoints = rand() % 26 + 25;
                Particle p(m_Window, numPoints, { event.mouseButton.x, event.mouseButton.y });
                m_particles.push_back(p);
            }
        }
    }
}


void Engine::update(float dtAsSeconds)
{
    // iterator for loop for deleting old particles
    for (std::vector<Particle>::iterator it = m_particles.begin(); it != m_particles.end(); )
    {
        if (it->getTTL() > 0.0)
        {
            it->update(dtAsSeconds);
            ++it; 
        }
        else
        {
            it = m_particles.erase(it);
        }
    }
}


void Engine::draw()
{
    m_Window.clear();
    // for loop for drawing the particles
    for (size_t i = 0; i < m_particles.size(); ++i)
    {
        m_Window.draw(m_particles[i]);
    }
    m_Window.display();
}
