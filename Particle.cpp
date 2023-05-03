Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition) : m_A(2, numPoints) {
    m_ttl = TTL;
    m_numPoints = numPoints;
    m_radiansPerSec = ((float)rand() / RAND_MAX) * M_PI;
    
    // Set up the Cartesian plane for mapping between monitor coordinates and Cartesian coordinates
    m_cartesianPlane.setCenter(0, 0);
    m_cartesianPlane.setSize(target.getSize().x, (-1.0) * target.getSize().y);

    // Map mouse click position to the Cartesian plane and store it as the center coordinate
    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    // Set random initial pixel velocities
    m_vx = (rand() % 2 == 0 ? 1 : -1) * (100 + rand() % 401);
    m_vy = -(100 + rand() % 401);

    // Set the initial color of the particle
    m_color1 = Color::White;
    m_color2 = Color(rand() % 256, rand() % 256, rand() % 256);

    // Generate the vertices of the particle
    double angleIncrement = 2.0 * M_PI / numPoints;
    double radius, angle;
    for (int i = 0; i < numPoints; i++) {
        radius = ((float)rand() / RAND_MAX) * 20.0;
        angle = i * angleIncrement + ((float)rand() / RAND_MAX) * angleIncrement;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        m_A(0, i) = x;
        m_A(1, i) = y;
    }
}
