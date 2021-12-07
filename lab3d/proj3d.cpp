#include "pch.h"
#include <cmath>
#include <vector>

typedef sf::Event sfe;
typedef sf::Keyboard sfk;

float R = 3.0;
float theta = 0.76;
float phi = 1.57;

float RLight = 3.0;
float thetaLight = 0.99;
float phiLight = 0.60;

sf::Vector3f rot(0.0f, 0.0f, 0.0f);
float* rot_offsets[3] = { &rot.x, &rot.y, &rot.z };

bool start = true;
bool game = true;

sf::Vector3f ball_pos(0.5 + 0.125, 0.125, 0.5 + 0.125);
double ball_r = 0.08;

std::vector<std::vector<char>> board = {
    { 'x', 'x', 'x', ' ', 'x', 'x', 'x', 'x'},
    { 'x', ' ', ' ', ' ', 'x', 'x', ' ', 'x'},
    { 'x', ' ', 'x', 'x', ' ', ' ', ' ', 'x'},
    { 'x', ' ', ' ', ' ', 'x', ' ', 'x', 'x'},
    { 'x', ' ', 'x', ' ', ' ', ' ', 'x', 'x'},
    { 'x', ' ', ' ', 'x', ' ', 'x', 'x', 'x'},
    { 'x', 'x', ' ', 'x', ' ', ' ', 'o', 'x'},
    { 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}
};

std::vector<std::vector<char>> board2 = {
    { 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
    { 'x', 'x', ' ', ' ', ' ', 'x', ' ', 'x'},
    { 'x', ' ', ' ', 'x', ' ', ' ', ' ', 'x'},
    { 'x', ' ', 'x', 'x', ' ', 'x', ' ', 'x'},
    { 'x', ' ', ' ', ' ', 'x', ' ', ' ', 'x'},
    { 'x', 'x', 'x', ' ', 'x', 'x', ' ', 'x'},
    { 'x', ' ', ' ', ' ', 'x', 'x', 'o', 'x'},
    { 'x', ' ', 'x', 'x', 'x', 'x', 'x', 'x'}
};

const GLfloat EmeraldAmbient[4] = { 0.021500, 0.174500, 0.021500, 0.9 };
const GLfloat EmeraldDiffuse[4] = { 0.075680, 0.614240, 0.075680, 0.9 };
const GLfloat EmeraldSpecular[4] = { 0.633000, 0.727811, 0.633000, 0.9 };
const GLfloat EmeraldShininess = 76.8;

void initOpenGL(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
}

void reshapeScreen(sf::Vector2u size)
{
    glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble)size.x / (GLdouble)size.y, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawScene()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float x1 = R * std::sin(theta) * std::cos(phi);
    float z1 = R * std::sin(theta) * std::sin(phi);
    float y1 = R * std::cos(theta);

    float x2 = RLight * std::sin(thetaLight) * std::cos(phiLight);
    float z2 = RLight * std::sin(thetaLight) * std::sin(phiLight);
    float y2 = RLight * std::cos(thetaLight);

    gluLookAt(x1, y1, z1, 0, 0, 0, 0, 1, 0);
    GLfloat lightpos[] = { x2, y2, z2, 0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
}

void drawLabyrinth(double x, double z)
{
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);

    glVertex3f(x, 0.25, z);
    glVertex3f(x + 0.25, 0.25, z);
    glVertex3f(x + 0.25, 0.25, z + 0.25);
    glVertex3f(x, 0.25, z + 0.25);

    glColor3f(0.0, 1.0, 0.0);

    glVertex3f(x, 0.01, z);
    glVertex3f(x + 0.25, 0.01, z);
    glVertex3f(x + 0.25, 0.25, z);
    glVertex3f(x, 0.25, z);

    glVertex3f(x + 0.25, 0.01, z);
    glVertex3f(x + 0.25, 0.01, z + 0.25);
    glVertex3f(x + 0.25, 0.25, z + 0.25);
    glVertex3f(x + 0.25, 0.25, z);

    glVertex3f(x + 0.25, 0.01, z + 0.25);
    glVertex3f(x, 0.01, z + 0.25);
    glVertex3f(x, 0.25, z + 0.25);
    glVertex3f(x + 0.25, 0.25, z + 0.25);

    glVertex3f(x, 0.01, z + 0.25);
    glVertex3f(x, 0.01, z);
    glVertex3f(x, 0.25, z);
    glVertex3f(x, 0.25, z + 0.25);

    glEnd();
}

int getBoardIndex(double pos, int sign = 1, double delta = 0)
{
    return (pos + delta + sign * ball_r + 1) * 4;
}

void moveBall()
{
    double delta_x = 0.03 * std::sin(std::abs(rot.z * 3.14 / 180));
    double delta_z = 0.03 * std::sin(std::abs(rot.x * 3.14 / 180));

    if (rot.z > 0)
    {
        if (board[getBoardIndex(ball_pos.z, -1)][getBoardIndex(ball_pos.x, -1, -delta_x)] != 'x'
            && board[getBoardIndex(ball_pos.z, 1)][getBoardIndex(ball_pos.x, -1, -delta_x)] != 'x')
            ball_pos.x -= delta_x;
    }
    else if (rot.z < 0)
    {
        if (board[getBoardIndex(ball_pos.z, -1)][getBoardIndex(ball_pos.x, 1, delta_x)] != 'x'
            && board[getBoardIndex(ball_pos.z, 1)][getBoardIndex(ball_pos.x, 1, delta_x)] != 'x')
            ball_pos.x += delta_x;
    }
    if (rot.x > 0)
    {
        if (board[getBoardIndex(ball_pos.z, 1, delta_z)][getBoardIndex(ball_pos.x, -1)] != 'x'
            && board[getBoardIndex(ball_pos.z, 1, delta_z)][getBoardIndex(ball_pos.x, 1)] != 'x')
            ball_pos.z += delta_z;
    }
    else if (rot.x < 0)
    {
        if (board[getBoardIndex(ball_pos.z, -1, -delta_z)][getBoardIndex(ball_pos.x, -1)] != 'x'
            && board[getBoardIndex(ball_pos.z, -1, -delta_z)][getBoardIndex(ball_pos.x, 1)] != 'x')
            ball_pos.z -= delta_z;
    }

    if (ball_pos.x < -0.9 || ball_pos.x > 0.9 || ball_pos.z < -0.9 || ball_pos.z > 0.9)
    {
        game = false;
    }
}

void drawBall()
{
    GLUquadricObj* qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);

    glDisable(GL_COLOR_MATERIAL);
    glPushMatrix();
    glTranslatef(ball_pos.x, ball_pos.y, ball_pos.z);
    glMaterialfv(GL_FRONT, GL_AMBIENT, EmeraldAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, EmeraldDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, EmeraldSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &EmeraldShininess);
    glColor3f(1.0f, 0.0f, 0.0f);
    gluSphere(qobj, ball_r, 15, 10);
    glPopMatrix();
    glEnable(GL_COLOR_MATERIAL);
}

int main()
{
    bool running = true;
    sf::ContextSettings context(24, 0, 0, 4, 5);
    sf::RenderWindow window(sf::VideoMode(800, 600), "Grafika 3D - Projekt - Olga Kubiszyn", 7u, context);
    sf::Clock deltaClock;

    ImGui::SFML::Init(window);

    window.setVerticalSyncEnabled(true);

    reshapeScreen(window.getSize());
    initOpenGL();

    while (running)
    {
        sfe event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sfe::Closed || (event.type == sfe::KeyPressed && event.key.code == sfk::Escape)) running = false;
            if (event.type == sfe::Resized) reshapeScreen(window.getSize());
        }

        drawScene();

        ImGui::SFML::Update(window, deltaClock.restart());

        if (start)
        {
            ImGui::Begin("Hello!");
            ImGui::Text("Welcome to Ball in Labyrinth!");
            ImGui::Text("Project by Olga Kubiszyn");

            if (ImGui::Button("Board1"))
            {
                start = false;
            }
            else if (ImGui::Button("Board2"))
            {
                board = board2;
                start = false;
            }

            ImGui::End();
        }
        else if (game)
        {
            glRotatef(rot.x, 1, 0, 0);
            glRotatef(rot.y, 0, 1, 0);
            glRotatef(rot.z, 0, 0, 1);

            glColor3f(1.0, 0.0, 1.0);
            glBegin(GL_QUADS);
            glVertex3f(1, 0, 1);
            glVertex3f(1, 0, -1);
            glVertex3f(-1, 0, -1);
            glVertex3f(-1, 0, 1);
            glEnd();
        
            double x = -1; double z = -1;
        
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (board[i][j] == 'x')
                    {
                        drawLabyrinth(x, z);
                    }
                    else if (board[i][j] == 'o')
                    {
                        moveBall();
                        drawBall();
                    }

                    x += 0.25;
                    if (x == 1)
                    {
                        x = -1;
                        z += 0.25;
                    }
                }
            }

            ImGui::Begin(" ");
            ImGui::Text("Tilt the board using ASDW keys.");
            ImGui::End();

            if (sfk::isKeyPressed(sfk::A))
            {
                rot.z += 0.5;
            }
            else if (sfk::isKeyPressed(sfk::D))
            {
                rot.z -= 0.5;
            }
            else if (sfk::isKeyPressed(sfk::W))
            {
                rot.x -= 0.5;
            }
            else if (sfk::isKeyPressed(sfk::S))
            {
                rot.x += 0.5;
            }
        }
        else
        {
        ImGui::Begin("Wow!");
        ImGui::Text("You won!");
        ImGui::Text("Project by Olga Kubiszyn");
        if (ImGui::Button("Play again!"))
        {
            start = true;
            game = true;
            ball_pos = { 0.5 + 0.125, 0.125, 0.5 + 0.125 };
            rot = { 0.0f, 0.0f, 0.0f };
        }
        ImGui::End();
        }

        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}