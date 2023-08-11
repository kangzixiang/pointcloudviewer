#pragma once
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QMouseEvent>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>

#include <vector>
#include <mutex>

using namespace std;

class openGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    openGLWidget(QWidget* parent = nullptr);
    virtual ~openGLWidget();

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
    void drawTriangleTest();

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;

    QOpenGLShaderProgram *m_program;
    int m_attr;
    int m_color;
};