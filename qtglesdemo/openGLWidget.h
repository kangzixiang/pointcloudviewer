#pragma once
#include <QtGui/QMatrix4x4>
#include <QtGui/QMouseEvent>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtWidgets/QOpenGLWidget>

#include <mutex>
#include <vector>

using namespace std;

class openGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
public:
  openGLWidget(QWidget *parent = nullptr);
  virtual ~openGLWidget();
  void updateLidarPointCloudData(const vector<vector<double>> &data);

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
  void drawLidarPointCloud();

  vector<vector<double>> m_vecLidarPointCloudData;
  std::mutex m_vecLidarPointCloudDataMutex;

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