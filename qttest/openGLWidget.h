#pragma once
#include <QtGui/QMatrix4x4>
#include <QtGui/QMouseEvent>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtWidgets/QOpenGLWidget>

using namespace std;

class openGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {

public:
  openGLWidget(QWidget *parent = nullptr);
  virtual ~openGLWidget();

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

private:
  void draw();
  void drawTest();

  QOpenGLVertexArrayObject m_vao;
  QOpenGLBuffer m_vbo;

  QOpenGLShaderProgram *m_program;
  int m_attr;
  int m_color;

  bool mMousePress = false;

  QMatrix4x4 mModelMat;
  QMatrix4x4 mViewMat;
  QMatrix4x4 mProjMat;

  QMatrix4x4 mMVP;

  QPoint lastPos;

  QVector3D m_eye;
  QVector3D m_eyeCenter;
  QVector3D m_eyeUp;
};