#include "Particle.h"

Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition) : m_A(2, numPoints) {
    m_ttl = TTL;
    m_numPoints = numPoints;
    m_radiansPerSec = ((float)rand() / RAND_MAX) * PI;
    
    // Set up the Cartesian plane for mapping between monitor coordinates and Cartesian coordinates
    m_cartesianPlane.setCenter(0, 0);
    m_cartesianPlane.setSize(target.getSize().x, (-1.0) * target.getSize().y);

    // Map mouse click position to the Cartesian plane and store it as the center coordinate
    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    // Set random initial pixel velocities
    m_vx = 100 + rand() % 401;
    if (rand() % 2 == 0) m_vx *= -1;
    m_vy = (100 + rand() % 401);

    // Set the initial color of the particle
    m_color1 = Color::White;
    m_color2 = Color(rand() % 256, rand() % 256, rand() % 256);

    // Generate the vertices of the particle
    float theta = (rand() / (PI /2));
    float dTheta = 2.0 * PI / (numPoints -1);

    for (int j = 0; j < numPoints; j++) {
        float radius = (rand() % 60) + 20;
        float dx = radius * cos(theta);
        float dy = radius * sin(theta);
        theta += dTheta;
        m_A(0, j) = dx + m_centerCoordinate.x;
        m_A(1, j) = dy + m_centerCoordinate.y;
    }
}

void Particle::draw(RenderTarget& target, RenderStates states) const
{
    VertexArray lines(TriangleFan, m_numPoints + 1);

    Vector2f center = (Vector2f)target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane);

    lines[0].position = center;
    lines[0].color = m_color1;

    for (int j = 1; j <= m_numPoints; j++)
    {
        lines[j].position = Vector2f(target.mapCoordsToPixel(Vector2f(m_A(0, j - 1), m_A(1, j - 1)), m_cartesianPlane));
        lines[j].color = m_color2;
    }

    target.draw(lines);
}

void Particle::update(float dt) 
{
    m_ttl -= dt;
    rotate(dt * m_radiansPerSec);
    scale(SCALE);
    float dx = m_vx * dt;
    m_vy -= G * dt;
    float dy = m_vy * dt;
    translate(dx, dy);
}

void Particle::translate(double xShift, double yShift)
{
    Matrices::TranslationMatrix T(xShift, yShift, m_A.getCols());
    m_A = T + m_A;
    m_centerCoordinate.x += xShift;
    m_centerCoordinate.y += yShift;
}

void Particle::rotate(double theta)
{
    // temporarily shift to the origin
    Vector2f temp = m_centerCoordinate;
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y);

    // rotate about the origin
    Matrices::RotationMatrix R(theta);
    m_A = R * m_A;

    // shift back to original center
    translate(temp.x, temp.y);
}

void Particle::scale(double c)
{
    // temporarily shift to the origin
    Vector2f temp = m_centerCoordinate;
    translate(-m_centerCoordinate.x, -m_centerCoordinate.y);

    // scale relative to origin
    Matrices::ScalingMatrix S(c);
    m_A = S * m_A;

    // shift back to original center
    translate(temp.x, temp.y);
}


bool Particle::almostEqual(double a, double b, double eps)
{
    return fabs(a - b) < eps;
}

void Particle::unitTests()
{
    int score = 0;
    cout << "Testing RotationMatrix constructor...";
    double theta = PI / 4.0;
    RotationMatrix r(PI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta))
        && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta)))
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }
    cout << "Testing ScalingMatrix constructor...";
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2
        && almostEqual(s(0, 0), 1.5)
        && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0)
        && almostEqual(s(1, 1), 1.5))
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }
    cout << "Testing TranslationMatrix constructor...";
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3
        && almostEqual(t(0, 0), 5)
        && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5)
        && almostEqual(t(1, 1), -5)
        && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5))
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }
    cout << "Testing Particles..." << endl;
    cout << "Testing Particle mapping to Cartesian origin..." << endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0)
    {
        cout << "Failed. Expected (0,0). Received: (" << m_centerCoordinate.x <<
            "," << m_centerCoordinate.y << ")" << endl;
    }
    else
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    cout << "Applying one rotation of 90 degrees about the origin..." << endl;
    Matrix initialCoords = m_A;
    rotate(PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1,
            j), initialCoords(0, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") "
                << "==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            rotationPassed = false;
        }
    }
    if (rotationPassed)
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }
    cout << "Applying a scale of 0.5..." << endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0, j)) || !
            almostEqual(m_A(1, j), 0.5 * initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ")"
                " ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
                scalePassed = false;
        }
    }
    if (scalePassed)
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }
    cout << "Applying a translation of (10, 5)..." << endl;
    initialCoords = m_A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !
            almostEqual(m_A(1, j), 5 + initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ")"
               " ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
                translatePassed = false;
        }
    }
    if (translatePassed)
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }
    cout << "Score: " << score << " / 7" << endl;
}
