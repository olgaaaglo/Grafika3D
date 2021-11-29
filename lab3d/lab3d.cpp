// lab3d.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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

float Position[3] = { 0, 0, 0 };
float Scale[3] = { 1, 1, 1 };
float Rotation[3] = { 0, 0, 0 };

sf::Vector3f pos(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rot(0.0f, 0.0f, 0.0f);

float* pos_offsets[3] = { &pos.x, &pos.y, &pos.z };
float* scale_offsets[3] = { &scale.x, &scale.y, &scale.z };
float* rot_offsets[3] = { &rot.x, &rot.y, &rot.z };

bool perspective_projection = true;
float fov = 45;

float timer = 0.0;

sf::Vector3f ball_pos(0.5 + 0.125, 0.125, 0.5 + 0.125);

double ball_r = 0.08;

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
    //gluPerspective(45.0, (GLdouble)size.x / (GLdouble)size.y, 0.1, 100.0);
    if (perspective_projection) gluPerspective(fov, (GLdouble)size.x / (GLdouble)size.y, 0.1, 100.0);
    else glOrtho(-1.245 * ((GLdouble)size.x / (GLdouble)size.y), 1.245 * ((GLdouble)size.x / (GLdouble)size.y), -1.245, 1.245, -3.0, 12.0);
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

    //std::cout << theta << " " << phi << std::endl;

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

void drawLine(GLfloat c1, GLfloat c2, GLfloat c3, GLfloat v11, GLfloat v12, GLfloat v13, GLfloat v21, GLfloat v22, GLfloat v23)
{
    glColor3f(c1, c2, c3);
    glVertex3f(v11, v12, v13);
    glVertex3f(v21, v22, v23);
}

void drawTriangle()
{
    glBegin(GL_POINTS);

    double r = 1.0; double g = 0.0; double b = 0.0;
    double x1 = -0.5; double y1 = 0.5; double z1 = 0.5;
    while (x1 <= 0.5)
    {
        glColor3f(r, g, b);
        glVertex3f(x1, y1, z1);
        r -= 0.001;
        g += 0.001;
        x1 += 0.001;
        y1 -= 0.001;
        double x2 = x1;
        double r2 = r;
        double g2 = g;
        while (x2 <= 0.5)
        {
            b += 0.001;
            x2 += 0.001;
            z1 -= 0.001;
            r2 -= 0.001;
            glColor3f(r2, g2, b);
            glVertex3f(x2, y1, z1);
        }
        b = 0.0;
        z1 = 0.5;
    }
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.5, 0.5, -0.5);

    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-0.5, 0.5, 0.5);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.5, -0.5, 0.5);

    glEnd();
}

sf::Vector3f cross_product(sf::Vector3f u, sf::Vector3f v)
{
    sf::Vector3f res;
    res.x = u.y * v.z - u.z * v.y;
    res.y = u.z * v.x - u.x * v.z;
    res.z = u.x * v.y - u.y * v.x;
    return res;
}

void glVertexsf(sf::Vector3f v)
{
    glVertex3f(v.x, v.y, v.z);
}

void glNormalsf(sf::Vector3f v)
{
    glNormal3f(v.x, v.y, v.z);
}

int getBoardIndex(double pos, double delta = 0, int sign = 1)
{
    return (pos + delta + sign*ball_r + 1) * 4;
}

int main()
{
    bool running = true;
    sf::ContextSettings context(24, 0, 0, 4, 5);
    sf::RenderWindow window(sf::VideoMode(800, 600), "Open GL Lab3 00", 7u, context);
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

        //ImGui::SFML::Update(window, deltaClock.restart());

        glDisable(GL_LIGHTING);

        glBegin(GL_LINES);

        drawLine(1.0, 0.0, 0.0, 0, 0, 0, 1, 0, 0);
        drawLine(0.0, 1.0, 0.0, 0, 0, 0, 0, 1, 0);
        drawLine(0.0, 0.0, 1.0, 0, 0, 0, 0, 0, 1);

        glEnd();

        glPushAttrib(GL_ENABLE_BIT);
        glLineStipple(2, 0xAAAA);
        glEnable(GL_LINE_STIPPLE);
        glBegin(GL_LINES);

        drawLine(1.0, 0.0, 0.0, 0, 0, 0, -1, 0, 0);
        drawLine(0.0, 1.0, 0.0, 0, 0, 0, 0, -1, 0);
        drawLine(0.0, 0.0, 1.0, 0, 0, 0, 0, 0, -1);
        glEnd();
        glPopAttrib();

        /*double Points[12][6] = { {-0.5, 0.5, -0.5, 0.5, 0.5, -0.5 },
        {-0.5, -0.5, -0.5, 0.5, -0.5, -0.5},
            {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5},
            {-0.5, 0.5, 0.5, 0.5, 0.5, 0.5},
            {-0.5, -0.5, -0.5, -0.5, 0.5, -0.5},
            {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5},
            {0.5, -0.5, -0.5, 0.5, 0.5, -0.5},
            { 0.5, -0.5, 0.5, 0.5, 0.5, 0.5},
            {-0.5, -0.5, -0.5, -0.5, -0.5, 0.5},
            {-0.5, 0.5, -0.5, -0.5, 0.5, 0.5},
            {0.5, -0.5, -0.5, 0.5, -0.5, 0.5},
            { 0.5, 0.5, -0.5, 0.5, 0.5, 0.5} };

        glTranslatef(pos.x, pos.y, pos.z);
        glRotatef(rot.x, 1, 0, 0);
        glRotatef(rot.y, 0, 1, 0);
        glRotatef(rot.z, 0, 0, 1);
        glScalef(scale.x, scale.y, scale.z);

        glBegin(GL_LINES);
        for (int i = 0; i < 12; i++)
        {
                drawLine(0.0, 0.0, 0.0, Points[i][0] + Position[0], Points[i][1] + Position[1], Points[i][2] + Position[2], Points[i][3] + Position[0], Points[i][4] + Position[1], Points[i][5] + Position[2]);
        }

        glEnd();*/

        //drawTriangle();

        glEnable(GL_LIGHTING);

        /*GLUquadricObj* qobj = gluNewQuadric();
        gluQuadricDrawStyle(qobj, GLU_FILL);
        gluQuadricNormals(qobj, GLU_SMOOTH);

        glPushMatrix();
        glColor3f(1.0f, 0.0f, 0.0f);
        glTranslatef(-0.75, 0.0, 0.0);
        gluSphere(qobj, 0.2, 15, 10);
        glPopMatrix();

        const GLfloat EmeraldAmbient[4] = { 0.021500, 0.174500, 0.021500, 0.550000 };
        const GLfloat EmeraldDiffuse[4] = { 0.075680, 0.614240, 0.075680, 0.550000 };
        const GLfloat EmeraldSpecular[4] = { 0.633000, 0.727811, 0.633000, 0.550000 };
        const GLfloat EmeraldShininess = 76.8;

        glDisable(GL_COLOR_MATERIAL);
        glPushMatrix();
        glTranslatef(0.75, 0.0, 0.0);
        glRotatef(300.0, 1.0, 0.0, 0.0);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, EmeraldAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, EmeraldDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, EmeraldSpecular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &EmeraldShininess);
        glColor3f(0.0f, 1.0f, 0.0f);
        gluCylinder(qobj, 0.25, 0.0, 0.5, 15, 5);
        glPopMatrix();
        glEnable(GL_COLOR_MATERIAL);*/

        //glColor3d(1, 1, 0);

        //for (int i = 0.5; i >= -0.5; i-= 0.01)
        //{
        //    for (int j = 0.5; j >= -0.5; j-= 0.01)
        //    {
        //        glBegin(GL_POLYGON);
        //        glVertex3f(i, 0, j);
        //        glVertex3f(i, 0, j - 0.01);
        //        glVertex3f(i - 0.01, 0, j);
        //        glVertex3f(i - 0.01, 0, j - 0.01);
        //        glEnd();
        //    }
        //}
        ///*glVertex3f(0.5, 0, -0.5);
        //glVertex3f(0.5, 0, 0.5);
        //glVertex3f(-0.5, 0, 0.5);
        //glVertex3f(-0.5, 0, -0.5);*/

        sf::Vector3f u, v, res[4], flag[20][20], flag_n[20][20];

        for (int x = 0; x < 20; x++)
            for (int y = 0; y < 20; y++)
            {
                flag[x][y].x = (x - 10) * 0.1f;
                flag[x][y].y = 0.2f * sin((x - 10) * 0.2f - timer) * cos((y - 10) * 0.2f + timer);
                flag[x][y].z = (y - 10) * 0.1f;
            }
        for (int x = 1; x < 19; x++)
            for (int y = 1; y < 19; y++)
            {
                u = flag[x][y + 1] - flag[x][y];
                v = flag[x + 1][y] - flag[x][y];
                res[0] = cross_product(u, v);

                u = flag[x + 1][y] - flag[x][y];
                v = flag[x][y - 1] - flag[x][y];
                res[1] = cross_product(u, v);

                u = flag[x][y - 1] - flag[x][y];
                v = flag[x - 1][y] - flag[x][y];
                res[2] = cross_product(u, v);

                u = flag[x - 1][y] - flag[x][y];
                v = flag[x][y + 1] - flag[x][y];
                res[3] = cross_product(u, v);

                flag_n[x][y] = (res[0] + res[1] + res[2] + res[3]);
            }

        //glColor3f(1.0f, 1.0f, 0.0f);

        //glDisable(GL_COLOR_MATERIAL);
        //glEnable(GL_TEXTURE_2D);

        //sf::Texture texture;
        //if (texture.loadFromFile("water.jpg"))
        //{
        //    sf::Texture::bind(&texture);


        //    glBegin(GL_QUADS);
        //    // glNormal3f(0, 1, 0);
        //    for (int x = 1; x < 18; x++)
        //        for (int y = 1; y < 18; y++)
        //        {
        //            glNormalsf(flag_n[x][y]);
        //            glVertexsf(flag[x][y]);
        //            glNormalsf(flag_n[x + 1][y]);
        //            glVertexsf(flag[x + 1][y]);
        //            glNormalsf(flag_n[x + 1][y + 1]);
        //            glVertexsf(flag[x + 1][y + 1]);
        //            glNormalsf(flag_n[x][y + 1]);
        //            glVertexsf(flag[x][y + 1]);
        //        }
        //    glEnd();


        //    sf::Texture::bind(NULL);
        //}

        //glEnable(GL_COLOR_MATERIAL);

        /* glDisable(GL_LIGHTING);
         glColor3f(0, 0, 0.8f);
         glBegin(GL_LINES);
         for (int x = 1; x < 19; x++)
             for (int y = 1; y < 19; y++)
             {
                 glVertexsf(flag[x][y]);
                 flag[x][y] += flag_n[x][y];
                 glVertexsf(flag[x][y]);
             }
         glEnd();
         glEnable(GL_LIGHTING);*/

        timer += deltaClock.getElapsedTime().asSeconds();
        ImGui::SFML::Update(window, deltaClock.restart());

        /*glColor4f(1.0, 0.0, 0.0, 0.5);

        glBegin(GL_POLYGON);
        glVertex3f(0.5, 0.5, 1);
        glVertex3f(0.5, -0.5, 1);
        glVertex3f(-0.5, -0.5, 1);
        glVertex3f(-0.5, 0.5, 1);
        glEnd();*/

        //glEnable(GL_BLEND);

        //sf::BlendMode(sf::BlendMode::One, sf::BlendMode::SrcAlpha, sf::BlendMode::Subtract);
        ////  glBlendEquation(GL_ONE_MINUS_SRC_ALPHA);// <-SFML tego nie lubi - wygl?da mi to na bug'a
        ////  glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);

        //glBegin(GL_QUADS);
        //glColor4f(1.0, 0.0, 0.0, 0.5); glVertex3f(-0.5, 0.5, 1.0);
        //glColor4f(0.0, 1.0, 0.0, 0.5); glVertex3f(0.5, 0.5, 1.0);
        //glColor4f(0.0, 0.0, 1.0, 0.5); glVertex3f(0.5, -0.5, 1.0);
        //glColor4f(0.0, 0.0, 0.0, 0.5); glVertex3f(-0.5, -0.5, 1.0);
        //glEnd();
        //glDisable(GL_BLEND);

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

        /*std::vector<std::vector<int> > board1000(1000, std::vector<int>(1000));
        for (int i = 0; i < 1000; i++)
        {
            for (int j = 0; j < 1000; j++)
            {
                if (board[i/125][j/125] == 'x')
                    board1000
            }
        }*/
        
        double x = -1; double z = -1;
        /*glBegin(GL_QUADS);
        glVertex3f(-1, 0.01, -1);
        glVertex3f(-1 + 0.25, 0.01, - 1);
        glVertex3f(-1 + 0.25, 0.01, -1 + 0.25);
        glVertex3f(-1, 0.01, -1 + 0.25);
        glEnd();*/
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (board[i][j] == 'x')
                {
                    glColor3f(1.0, 0.271, 0.0);
                    glBegin(GL_QUADS);

                    /*glVertex3f(x, 0.01, z);
                    glVertex3f(x + 0.25, 0.01, z);
                    glVertex3f(x + 0.25, 0.01, z + 0.25);
                    glVertex3f(x, 0.01, z + 0.25);*/

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
                else if (board[i][j] == 'o')
                {
                    //std::cout << rot.x << " " << rot.z << std::endl;
                    std::cout << (ball_pos.x - 0.002 - ball_r + 1) * 4 << std::endl;//8
                    if (rot.z > 0) 
                    { 
                        if (board[getBoardIndex(ball_pos.z)][getBoardIndex(ball_pos.x, -0.002, -1)] != 'x')
                            ball_pos.x -= 0.002; 
                    }
                    else if (rot.z < 0) 
                    {
                        if (board[getBoardIndex(ball_pos.z)][getBoardIndex(ball_pos.x, 0.002, 1)] != 'x')
                            ball_pos.x += 0.002;  
                    }
                    if (rot.x > 0) 
                    { 
                        if (board[getBoardIndex(ball_pos.z, 0.002, 1)][getBoardIndex(ball_pos.x)] != 'x')
                            ball_pos.z += 0.002; 
                    }
                    else if (rot.x < 0) 
                    { 
                        if (board[getBoardIndex(ball_pos.z, -0.002, -1)][getBoardIndex(ball_pos.x)] != 'x')
                            ball_pos.z -= 0.002;  
                    }

                    GLUquadricObj* qobj = gluNewQuadric();
                    gluQuadricDrawStyle(qobj, GLU_FILL);
                    gluQuadricNormals(qobj, GLU_SMOOTH);

                    glPushMatrix();
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glTranslatef(ball_pos.x, ball_pos.y, ball_pos.z);
                    gluSphere(qobj, ball_r, 15, 10);
                    glPopMatrix();

                    //std::cout << x << " " << z << std::endl;
                }

                //std::cout << x << " " << z << std::endl;

                x += 0.25;
                if (x == 1)
                {
                    x = -1;
                    z += 0.25;
                }
            }
        }

        /*if (sfk::isKeyPressed(sfk::S))
        {
            BYTE* pixels = new BYTE[3 * 800 * 600];

            glReadPixels(0, 0, 800, 600, GL_RGB, GL_UNSIGNED_BYTE, pixels);

            sf::Texture texture;
            texture.create(window.getSize().x, window.getSize().y);
            texture.update(pixels);
            texture.copyToImage().saveToFile("screen.jpg");
        }*/


        ImGui::Begin("Camera");
        ImGui::SliderFloat("R", &R, 0.5f, 10.0f);
        ImGui::SliderAngle("theta", &theta, 0, 360);
        ImGui::SliderAngle("phi", &phi, 0, 180);
        if (ImGui::Checkbox("Perspective projection", &perspective_projection)) reshapeScreen(window.getSize());
        if (ImGui::SliderFloat("FoV", &fov, 10.0f, 90.0f)) reshapeScreen(window.getSize());
        ImGui::End();

        ImGui::Begin("Transformations");
        ImGui::SliderFloat3("Position", *pos_offsets, -3.0f, 3.0f);
        ImGui::SliderFloat3("Scale", *scale_offsets, -2.0f, 2.0f);
        ImGui::SliderFloat3("Rotation", *rot_offsets, -15.0f, 15.0f);
        ImGui::End();

        ImGui::Begin("Light");
        ImGui::SliderFloat("R", &RLight, 0.5f, 10.0f);
        ImGui::SliderAngle("theta", &thetaLight, 0, 360);
        ImGui::SliderAngle("phi", &phiLight, 0, 180);
        ImGui::End();

        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file