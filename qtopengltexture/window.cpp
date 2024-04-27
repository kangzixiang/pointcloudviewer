
#include <QtCore/QTime>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <time.h>

#include "window.h"

Window::Window(QWindow *parent) : QOpenGLWindow(NoPartialUpdate, parent) {
  angleInDegrees = 0;
  angleInDegrees += 0.3;

  resize(1280, 760);
  startTimer(QSurfaceFormat::defaultFormat().swapInterval());
}
Window::~Window() {}
void Window::initializeGL() {
  initializeOpenGLFunctions();
#ifdef GL_DEBUG
  mDebugger.initialize();
  connect(&mDebugger, &QOpenGLDebugLogger::messageLogged, this,
          &GLWindow::onDebuggerMessage);
  mDebugger.startLogging();
#endif
  mElapsed.start();
  mFrame = 0;

  resize(width(), height());
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  mModelMatrix.setToIdentity();
  initTextures();
  initShaders();
  initData();
}
void Window::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {

  case Qt::Key_A: {
    mModelMatrix.translate(-0.1f, 0, 0);
    update();
    break;
  }
  case Qt::Key_D: {
    mModelMatrix.translate(0.1f, 0, 0);
    update();
    break;
  }
  default: {
    break;
  }
  }
}
void Window::paintGL() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  program.bind();

  mModelMatrix.rotate(angleInDegrees, 0.0f, 0.0f, 1.0f);
  draw();

  program.release();

  calcFPS();
  paintFPS();
}

void Window::draw() {
  // 顶点
  program.enableAttributeArray(mVerticesHandle);
  program.setAttributeArray(mVerticesHandle, vertices.constData());
  // 颜色
  program.enableAttributeArray(mColorsHandle);
  program.setAttributeArray(mColorsHandle, colors.constData());
  // 纹理坐标
  program.enableAttributeArray(mTexCoordHandle);
  program.setAttributeArray(mTexCoordHandle, texcoords.constData());
  // MVP矩阵
  mMVPMatrix = mProjectionMatrix * mViewMatrix * mModelMatrix;
  program.setUniformValue(mMVPMatrixHandle, mMVPMatrix);

  // 纹理
  texture->bind();
  program.setUniformValue("qt_Texture0", 0);

  glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
  texture->release();
  program.disableAttributeArray(mVerticesHandle);
  program.disableAttributeArray(mColorsHandle);
}
void Window::initData() {
  vertices << QVector3D(-1, -1, 0.0f) << QVector3D(1, -1, 0.0f)
           << QVector3D(1, 1, 0.0f) << QVector3D(-1, 1, 0.0f);
  texcoords << QVector2D(0, 0) << QVector2D(1, 0) << QVector2D(1, 1)
            << QVector2D(0, 1);
  colors << QVector4D(1.0f, 0.0f, 0.0f, 1.0f)
         << QVector4D(0.0f, 0.0f, 1.0f, 1.0f)
         << QVector4D(0.0f, 1.0f, 0.0f, 1.0f);
  mViewMatrix.setToIdentity();
  mViewMatrix.lookAt(QVector3D(0.0f, 0.0f, 1.001f),
                     QVector3D(0.0f, 0.0f, -5.0f), QVector3D(0.0f, 1.0f, 0.0f));
}
void Window::resizeGL(int w, int h) {
  resize(w, h);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  glViewport(0, 0, w, h);

  float ratio = (float)w / h;
  float left = -ratio;
  float right = ratio;
  float bottom = -1.0f;
  float top = 1.0f;
  float n = 1.0f;
  float f = 10.0f;
  mProjectionMatrix.setToIdentity();
  mProjectionMatrix.frustum(left, right, bottom, top, n, f);
}

void Window::paintFPS() {
  QPainter painter(this);
  painter.setPen(Qt::green);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.drawText(10, 20,
                   QString("FPS:%1").arg(QString::number(mFps, 'f', 3)));
}

void Window::initShaders() {
  if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                       "res/shader/vertex.vsh")) {
    qDebug() << __FILE__ << __FUNCTION__ << " add vertex shader file failed.";
    return;
  }
  if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                       "res/shader/fragment.fsh")) {
    qDebug() << __FILE__ << __FUNCTION__ << " add fragment shader file failed.";
    return;
  }
  program.bindAttributeLocation("qt_Vertex", 0);
  program.bindAttributeLocation("a_Color", 1);
  program.bindAttributeLocation("qt_MultiTexCoord0", 2);
  program.link();
  program.bind();

  mMVPMatrixHandle = program.uniformLocation("qt_ModelViewProjectionMatrix");
  mVerticesHandle = program.attributeLocation("qt_Vertex");
  mColorsHandle = program.attributeLocation("a_Color");
  mTexCoordHandle = program.attributeLocation("qt_MultiTexCoord0");
}
void Window::initTextures() {
  texture = new QOpenGLTexture(QImage("res/image/wood.jpg").mirrored());
  texture->setMinificationFilter(QOpenGLTexture::Linear);
  texture->setMagnificationFilter(QOpenGLTexture::Linear);
  texture->setWrapMode(QOpenGLTexture::Repeat);
}

void Window::calcFPS() {
  mFrame++;
  if (mFrame > 100) {
    auto cost = mElapsed.elapsed();
    updateFPS(1.0 * mFrame / cost * 1000);
    mFrame = 0;
    mElapsed.restart();
  }
}
void Window::updateFPS(qreal v) { mFps = v; }

void Window::timerEvent(QTimerEvent *) { update(); }