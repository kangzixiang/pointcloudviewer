#include <QtWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>

class openGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    openGLWidget(QWidget* parent = nullptr);
    void updatePointCloudData(const std::vector<float> &data);
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
    void qNormalizeAngle(int &angle);

    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;

    float fScale;

    std::vector<float> pointCloudData;
};