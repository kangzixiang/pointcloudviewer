#define GL_GLEXT_PROTOTYPES 1

#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "LidarGrabber.hpp"

using namespace std;

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static vector<tuple<float, float, float, float>> vecPointData;
static float fMaxValue = 1.0f;
static float fScale = 1.0 / 100.0f;

void Display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glPushMatrix();
  glRotatef(xRot, 1.0, 0.0, 0.0);
  glRotatef(yRot, 0.0, 1.0, 0.0);

  glScalef(fScale, fScale, fScale);

  glBegin(GL_POINTS);
  glColor3f(1.0, 1.0, 1.0); // 白色
  glVertex3f(0.5f, 0.5f, 0.5f);
  glColor3f(1.0, 1.0, 1.0); // 白色
  glVertex3f(0.0f, 0.5f, 0.5f);
  glColor3f(1.0, 1.0, 1.0); // 白色
  glVertex3f(0.5f, 0.0f, 0.5f);
  glColor3f(1.0, 1.0, 1.0); // 白色
  glVertex3f(0.5f, 0.5f, 0.0f);

  glColor3f(1.0, 0.0, 0.0); // RGB red
  glVertex3f(1.0f, 1.0f, 1.0f);
  glColor3f(1.0, 0.0, 0.0); // RGB red
  glVertex3f(0.0f, 1.0f, 1.0f);
  glColor3f(1.0, 0.0, 0.0); // RGB red
  glVertex3f(1.0f, 0.0f, 1.0f);
  glColor3f(1.0, 0.0, 0.0); // RGB red
  glVertex3f(1.0f, 1.0f, 0.0f);

  glColor3f(0.0, 1.0, 0.0); // RGB green
  glVertex3f(2.0f, 2.0f, 2.0f);
  glColor3f(0.0, 1.0, 0.0); // RGB green
  glVertex3f(0.0f, 2.0f, 2.0f);
  glColor3f(0.0, 1.0, 0.0); // RGB green
  glVertex3f(2.0f, 0.0f, 2.0f);
  glColor3f(0.0, 1.0, 0.0); // RGB green
  glVertex3f(2.0f, 2.0f, 0.0f);
  glEnd();

  // Draw Coordinate
  glBegin(GL_LINES);        // glBegin和glEnd保证指定的这些点有效
  glColor3f(1.0, 0.0, 0.0); // 红色 X-Axis
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(100.0, 0.0, 0.0);
  glColor3f(0.0, 1.0, 0.0); // 绿色 Y-Axis
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 100.0, 0.0);
  glColor3f(0.0, 0.0, 1.0); // 蓝色 Z-Axis
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 100.0);
  glEnd();

  glFlush(); // 保证前面的命令立即执行
  glPopMatrix();
  glutSwapBuffers();
}

void displayPointCloud(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glPushMatrix();
  glRotatef(xRot, 1.0, 0.0, 0.0);
  glRotatef(yRot, 0.0, 1.0, 0.0);
  glScalef(fScale, fScale, fScale);
  glBegin(GL_POINTS);
  for (int i = 0; i < vecPointData.size() & i != 10; i++) {
    float x = 0.0, y = 0.0, z = 0.0, in = 0.0;
    x = std::get<0>(vecPointData[i]);
    y = std::get<1>(vecPointData[i]);
    z = std::get<2>(vecPointData[i]);
    in = std::get<3>(vecPointData[i]);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x / fMaxValue, y / fMaxValue, z / fMaxValue);
  }
  vecPointData.clear();
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
  default:
    break;
  }
  xRot = (xRot > 360.0) ? (xRot - 360.0) : xRot;
  xRot = (xRot < 0.0) ? (xRot + 360.0) : xRot;
  yRot = (yRot > 360.0) ? (yRot - 360.0) : yRot;
  yRot = (yRot < 0.0) ? (yRot + 360.0) : yRot;
  glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y) {
  static float fScaleValue = fScale;
  switch (button) {
  case GLUT_LEFT_BUTTON: {
    fScaleValue *= 1;
    break;
  }
  case GLUT_MIDDLE_BUTTON: {
    fScaleValue *= 1;
    // glScalef(fScale * 2, fScale * 2, fScale * 2);
    break;
  }
  case GLUT_RIGHT_BUTTON: {
    fScaleValue *= 1;
    break;
  }
  case 3: {
    fScaleValue *= 2;
    break;
  }
  case 4: {
    fScaleValue *= 0.5;
    break;
  }
  default: {
    break;
  }
  }
  glScalef(fScaleValue, fScaleValue, fScaleValue);
  glutPostRedisplay();
}

void idle(void) { glutPostRedisplay(); }

void lidarGrabberTest(std::string pcapFile) {
  CLidarGrabber lg(pcapFile);

  std::vector<PointXYZI> vec;
  std::function<void(const std::vector<PointXYZI> &)> cloud_cb =
      [&vec](const std::vector<PointXYZI> &cloud) { vec = cloud; };

  lg.registerCallback(cloud_cb);

  if (!lg.isRunning()) {
    lg.start();
    while (lg.isRunning()) {
      if (!vec.empty()) {
        vector<tuple<float, float, float, float>> vecDta;
        for (int i = 0; i < vec.size(); i++) {
          tuple<float, float, float, float> t(vec[i].x, vec[i].y, vec[i].z,
                                              vec[i].i);
          vecDta.push_back(t);
        }
        vecPointData = vecDta;
        vec.clear();
      }
    }
    lg.stop();
  }
}

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
  //  gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
  // 设置摄像机的位置及姿态参数:
  // 摄像机位置(cX, cY, cZ)
  // 视点所观察中心位置Ow(oX, oY, oZ)
  // 摄像机位姿参数——摄像机顶部矢量
  //  gluLookAt(0, 0, 5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  // 设置矩阵模式为模型-视图变换模式，以便于后面的自定义显示函数继续本模式
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouseWheel(int button, int dir, int x, int y);

int main(int argc, char **argv) {
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowPosition(400, 200);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Point Cloud Window"); // 直到glutMainLoop才显示窗口
  glPointSize(4.0);
  glLineWidth(1.0);
  // glClearColor(0.0,0.0,0.0,0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
  glutSwapBuffers();
  // glViewport(0, 0, 100, 100);
  glMatrixMode(GL_MODELVIEW); // 设置当前操作的矩阵为模型视图矩阵

  glutDisplayFunc(Display);
  glutSpecialFunc(SpecialKeys);
  glutIdleFunc(idle);
  glutMouseFunc(mouseCB);
  glutMouseWheelFunc(mouseWheel);
  // 窗口调整大小事件的处理函数
  glutReshapeFunc(reshape);
  // 设置窗口关闭后继续执行程序，而不是exit直接退出了程序
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
  //
  // std::thread *ThreadHandle = new std::thread(lidarGrabberTest,
  // "/tmp/sensor/lidar/test.pcap");
  glutMainLoop();
  return 0;
}

void mouseWheel(int button, int dir, int x, int y) { return; }
