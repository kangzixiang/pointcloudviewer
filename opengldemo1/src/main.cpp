#define GL_GLEXT_PROTOTYPES 1

#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>

#include "../../tools/tools.hpp"

using namespace std;

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static int lastX = 0;
static int lastY = 0;
static float fScale = 1.0 / 1.0f;

void loadPointCloud();
void qNormalizeAngle(int &angle);

void displayPointCloud(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glPushMatrix();
  glRotatef(xRot, 1.0, 0.0, 0.0);
  glRotatef(yRot, 0.0, 1.0, 0.0);
  glScalef(fScale, fScale, fScale);
  glBegin(GL_POINTS);
  std::ifstream ifs("filePath.txt");
  std::string filePath;
  getline(ifs, filePath);
  vector<vector<double>> vec = readPointCloud(filePath);
  for (std::size_t i = 0; i < vec.size(); i++) {
    std::vector<double> v = vec[i];
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(v[0], v[1], v[2]);
  }
  glEnd();
  glFlush(); // 保证前面的命令立即执行
  glPopMatrix();
  glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y) {
  float span = 5.0;
  switch (key) {
  case GLUT_KEY_F1:
    glutLeaveMainLoop();
    break;
  case GLUT_KEY_UP:
    xRot -= span;
    break;
  case GLUT_KEY_DOWN:
    xRot += span;
    break;
  case GLUT_KEY_LEFT:
    yRot -= span;
    break;
  case GLUT_KEY_RIGHT:
    yRot += span;
    break;
  case GLUT_KEY_PAGE_UP:
    fScale *= 1.1;
    break;
  case GLUT_KEY_PAGE_DOWN:
    fScale /= 1.1;
    break;
  default:
    break;
  }
  xRot = (xRot > 360.0) ? (xRot - 360.0) : xRot;
  xRot = (xRot < 0.0) ? (xRot + 360.0) : xRot;
  yRot = (yRot > 360.0) ? (yRot - 360.0) : yRot;
  yRot = (yRot < 0.0) ? (yRot + 360.0) : yRot;
  glutPostRedisplay();
}

void idle(void) { glutPostRedisplay(); }

// 用户自定义窗口调整大小事件的处理函数
// 在这个函数中要求设置视口、投影、透视变换的相关参数
void reshape(int w, int h) {
  // 设置视口参数为整个窗口范围内
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  // 设置投影参数:投影矩阵初始为单位阵
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // 设置透视参数: 眼睛或摄像机的视角参数为60
  // 度，视景体的宽度和高度比，视距（焦距） (near)和视径(far)参数 near = 1, far
  // = 100, Z 轴负向顺着视线方向指向屏幕内 X 轴正向向右，Y
  // 轴正向向上，坐标原点在屏幕中心处
  gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
  // 设置摄像机的位置及姿态参数:
  // 摄像机位置(cX, cY, cZ)
  // 视点所观察中心位置Ow(oX, oY, oZ)
  // 摄像机位姿参数——摄像机顶部矢量
  gluLookAt(0, 0, 5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  // 设置矩阵模式为模型-视图变换模式，以便于后面的自定义显示函数继续本模式
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouseWheel(int button, int dir, int x, int y) {
  switch (button) {
  case 0: // mouse left button
  {
    lastX = x;
    lastY = y;
    break;
  }
  case 3: {
    // Zoom in
    fScale *= 1.1;
    break;
  }
  case 4: {
    // Zoom out
    fScale /= 1.1;
    break;
  }
  default:
    break;
  }
  glutPostRedisplay();
  return;
}

void mouseMove(int x, int y) {
  int angle = 0;
  int dx = x - lastX;
  int dy = y - lastY;
  angle = xRot + 8 * dy;
  qNormalizeAngle(angle);
  if (angle != xRot) {
    xRot = angle;
  }
  angle = yRot + 8 * dx;
  qNormalizeAngle(angle);
  if (angle != yRot) {
    yRot = angle;
  }
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowPosition(400, 200);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Point Cloud Window"); // 直到glutMainLoop才显示窗口
  glPointSize(1.0);
  glLineWidth(1.0);
  // glClearColor(0.0,0.0,0.0,0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
  glutSwapBuffers();
  // glViewport(0, 0, 100, 100);
  glMatrixMode(GL_MODELVIEW); // 设置当前操作的矩阵为模型视图矩阵

  glutDisplayFunc(displayPointCloud);
  glutSpecialFunc(SpecialKeys);
  glutMouseFunc(mouseWheel);
  glutMotionFunc(mouseMove);
  glutIdleFunc(idle);
  // 窗口调整大小事件的处理函数
  glutReshapeFunc(reshape);
  // 设置窗口关闭后继续执行程序，而不是exit直接退出了程序
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
  //
  glutMainLoop();
  return 0;
}

void qNormalizeAngle(int &angle) {
  while (angle < 0)
    angle += 360 * 16;
  while (angle > 360 * 16)
    angle -= 360 * 16;
}
