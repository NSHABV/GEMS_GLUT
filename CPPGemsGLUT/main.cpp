// CPPGems.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <GL/glew.h>
#include "windowClass.h"

Sn_GemContainer* container;

void resizeHandle(int width, int height)
{
    glutReshapeWindow(450, 450);
}

void mouseHandle(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        container->handleMouseClick(x, y);
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        container->handleRightMouseClick(x, y);
}

void idleHandle(void)
{
    container->idleFunc();
    glutPostRedisplay();
}


void displayMe(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    container->drawGems();
    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    int xSize = 450;
    int ySize = 450;
    int gemSize = 50;
    container = new Sn_GemContainer(xSize, ySize, gemSize);

    container->isNeighbour(40, 44);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(xSize, ySize);           
    glutInitWindowPosition(10, 10);   
    glutCreateWindow("Gems Game");

    glutDisplayFunc(displayMe);
    glutReshapeFunc(resizeHandle);
    glutMouseFunc(mouseHandle);
    glutIdleFunc(idleHandle);

    glutMainLoop();

    delete container;
    return 0;
}
