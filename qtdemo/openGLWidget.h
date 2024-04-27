#pragma once
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>

#include <vector>

using namespace std;

class openGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
public:
  openGLWidget(QWidget *parent = nullptr);
  void updatePointCloudData(const vector<vector<double>> &data);
  void updateLidarPointCloudData(const vector<vector<double>> &data);

  int m_nDrawType;

  void LightOn();
  void LightOff();
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
  void drawTriangle();
  void drawPointCloud();
  void drawLidarPointCloud();
  void qNormalizeAngle(int &angle);

  int xRot;
  int yRot;
  int zRot;
  QPoint lastPos;

  float fScale;
  bool m_bLight;

  vector<vector<double>> pointCloudData;
  vector<vector<double>> m_vecLidarPointCloudData;
};