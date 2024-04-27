
#include <QtOpenGL/QGLWidget>

#include "openGLWidget.h"

openGLWidget::openGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
  fScale = 1.0;
}

openGLWidget::~openGLWidget() {
  if (nullptr != mrefreshTimer) {
    mrefreshTimer->stop();
    delete mrefreshTimer;
    mrefreshTimer = nullptr;
  }
}

void openGLWidget::updateLidarPointCloudData(
    const vector<vector<double>> &data) {
  m_vecLidarPointCloudDataMutex.lock();
  m_vecLidarPointCloudData = data;
  m_vecLidarPointCloudDataMutex.unlock();
  // update();
}

void openGLWidget::setXRotation(int angle) {
  qNormalizeAngle(angle);
  if (angle != xRot) {
    xRot = angle;
  }
}

void openGLWidget::setYRotation(int angle) {
  qNormalizeAngle(angle);
  if (angle != yRot) {
    yRot = angle;
  }
}

void openGLWidget::setZRotation(int angle) {
  qNormalizeAngle(angle);
  if (angle != zRot) {
    zRot = angle;
  }
}

void openGLWidget::initializeGL() {
  initializeOpenGLFunctions();

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  mrefreshTimer = new QTimer(this);
  mrefreshTimer->setObjectName(tr("_refreshTimer"));
  mrefreshTimer->setInterval(40);
  mrefreshTimer->start();
  connect(mrefreshTimer, SIGNAL(timeout()), this, SLOT(repaint()));
}

void openGLWidget::resizeGL(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-2, +2, -2, +2, 1.0, 15.0);

  glMatrixMode(GL_MODELVIEW);
}

void openGLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -10.0);
  glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
  glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
  glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
  glScalef(fScale, fScale, fScale);

  draw();
}

void openGLWidget::mousePressEvent(QMouseEvent *event) {
  lastPos = event->pos();
}

void openGLWidget::mouseMoveEvent(QMouseEvent *event) {
  int dx = event->x() - lastPos.x();
  int dy = event->y() - lastPos.y();

  if (event->buttons() & Qt::LeftButton) {
    setXRotation(xRot + 8 * dy);
    setYRotation(yRot + 8 * dx);
  } else if (event->buttons() & Qt::RightButton) {
    setXRotation(xRot + 8 * dy);
    setZRotation(zRot + 8 * dx);
  }

  lastPos = event->pos();
  update();
}

void openGLWidget::wheelEvent(QWheelEvent *event) {
  QOpenGLWidget::wheelEvent(event);
  float numStep = (event->angleDelta().y() / 8) / 15;
  if (numStep > 0) {
    fScale /= 1.1;
  } else {
    fScale *= 1.1;
  }

  update();
}

void openGLWidget::draw() { drawLidarPointCloud(); }

void openGLWidget::drawTest() {
  // glColor3f(1.0f, 0.0f, 0.0f);
  // glBegin(GL_QUADS);
  //     glNormal3f(0,0,-1);
  //     glVertex3f(-1,-1,0);
  //     glVertex3f(-1,1,0);
  //     glVertex3f(1,1,0);
  //     glVertex3f(1,-1,0);
  // glEnd();
  // glColor3f(0.0f, 1.0f, 0.0f);
  // glBegin(GL_TRIANGLES);
  //     glNormal3f(0,-1,0.707);
  //     glVertex3f(-1,-1,0);
  //     glVertex3f(1,-1,0);
  //     glVertex3f(0,0,1.2);
  // glEnd();
  // glColor3f(0.0f, 0.0f, 1.0f);
  // glBegin(GL_TRIANGLES);
  //     glNormal3f(1,0, 0.707);
  //     glVertex3f(1,-1,0);
  //     glVertex3f(1,1,0);
  //     glVertex3f(0,0,1.2);
  // glEnd();
  // glColor3f(0.0f, 1.0f, 1.0f);
  // glBegin(GL_TRIANGLES);
  //     glNormal3f(0,1,0.707);
  //     glVertex3f(1,1,0);
  //     glVertex3f(-1,1,0);
  //     glVertex3f(0,0,1.2);
  // glEnd();
  // glColor3f(1.0f, 0.0f, 1.0f);
  // glBegin(GL_TRIANGLES);
  //     glNormal3f(-1,0,0.707);
  //     glVertex3f(-1,1,0);
  //     glVertex3f(-1,-1,0);
  //     glVertex3f(0,0,1.2);
  // glEnd();
}

void openGLWidget::drawLidarPointCloud() {
  glPointSize(1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  m_vecLidarPointCloudDataMutex.lock();
  for (std::size_t i = 0; i < m_vecLidarPointCloudData.size(); i++) {
    glBegin(GL_POINTS);
    std::vector<double> v = m_vecLidarPointCloudData[i];
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(v[0], v[1], v[2]);
    glEnd();
  }
  m_vecLidarPointCloudDataMutex.unlock();
}

void openGLWidget::qNormalizeAngle(int &angle) {
  while (angle < 0)
    angle += 360 * 16;
  while (angle > 360 * 16)
    angle -= 360 * 16;
}
