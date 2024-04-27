#pragma once
#include <QtCore/qtimer.h>
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

public slots:

  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);

signals:
  // signaling rotation from mouse movement
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

private:
  void draw();
  void drawTest();
  void drawLidarPointCloud();
  void qNormalizeAngle(int &angle);

  int xRot;
  int yRot;
  int zRot;
  QPoint lastPos;

  float fScale;

  QTimer *mrefreshTimer;

  vector<vector<double>> m_vecLidarPointCloudData;
  std::mutex m_vecLidarPointCloudDataMutex;
};