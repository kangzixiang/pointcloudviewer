
#include <QtOpenGL/QGLWidget>
#include <GLES/gl.h>

#include "openGLWidget.h"

static GLfloat vertices[] = {0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, //
                                -1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
                                1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, //
                                0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

openGLWidget::openGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    mWordMat.setToIdentity();
    mViewMat.setToIdentity();
    mProjMat.setToIdentity();
    mMVP = mProjMat * mViewMat * mWordMat;
}

openGLWidget::~openGLWidget() {
    m_vao.destroy();
    m_vbo.destroy();
}

void openGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram();
    m_program->bind();
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "./vshader.glsl"))
    {
        qDebug() << (m_program->log());
    }
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "./fshader.glsl"))
    {
        qDebug() << (m_program->log());
    }

    if (!m_program->link())
    {
        qDebug() << (m_program->log());
    }
    
    m_vao.create();
    m_vao.bind();
    m_vbo.create();
    m_vbo.bind();

    m_program->release();
    m_vbo.release();
    m_vao.release();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    mWordMat.setToIdentity();
    mViewMat.setToIdentity();
    mProjMat.setToIdentity();

    m_eye.setX(0.0);
    m_eye.setY(0.0);
    m_eye.setZ(2.0);

    m_eyeUp.setX(0.0);
    m_eyeUp.setY(1.0);
    m_eyeUp.setZ(0.0);
    mViewMat.lookAt(m_eye, m_eyeCenter, m_eyeUp);

    mMVP = mProjMat * mViewMat * mWordMat;

}

void openGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    mProjMat.ortho(-2, +2, -2, +2, 1.0, 15.0);
    mMVP = mProjMat * mViewMat * mWordMat;

}

void openGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
    
    draw();
    
}

void openGLWidget::mousePressEvent(QMouseEvent *event)
{
    mMousePress = true;
    lastPos = event->pos();
}

void openGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    mMousePress = false;
}

void openGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    if (event->buttons() & Qt::LeftButton) {
        mWordMat.rotate(1 + 8 * dy, 1 + 8 * dx, 1);
    } else if (event->buttons() & Qt::RightButton) {
        mWordMat.rotate(1 + 8 * dy, 1, 1  + 8 * dx);
    }
    mMVP = mProjMat * mViewMat * mWordMat;
    lastPos = event->pos();
    update();
}

void openGLWidget::wheelEvent(QWheelEvent *event)
{
    QOpenGLWidget::wheelEvent(event);
    float numStep = (event->angleDelta().y() / 8) / 15;
    static float fScale = 1.0;
    if (numStep > 0)
    {
        fScale /= 1.1;
    }
    else
    {
        fScale *= 1.1;
    }
    mWordMat.scale(fScale, fScale, fScale);
    mMVP = mProjMat * mViewMat * mWordMat;
    update();
}

void openGLWidget::draw()
{
    drawTest();
}

void openGLWidget::drawTest() {
    m_program->bind();
    m_vao.bind();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices));

    m_attr = m_program->attributeLocation("aPos");
    m_program->setAttributeBuffer(m_attr, GL_FLOAT, 0, 3, 6 * sizeof(GL_FLOAT));
    m_program->enableAttributeArray(m_attr);

    m_color = m_program->attributeLocation("aColor");
    m_program->setAttributeBuffer(m_color, GL_FLOAT, 3 * sizeof(GL_FLOAT), 3, 6 * sizeof(GL_FLOAT));
    m_program->enableAttributeArray(m_color);

    m_program->setUniformValue("MVP", mMVP);

    glDrawArrays(GL_POINTS, 0, 6);
    m_vbo.release();
    m_vao.release();
    m_program->release();
}
