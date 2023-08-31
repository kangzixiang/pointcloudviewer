
#include <QtOpenGL/QGLWidget>
#include <GLES/gl.h>

#include "openGLWidget.h"

static GLfloat vertices[] = {-4.0f, -4.0f, 0.0f, 1.0f, 0.0f, 0.0f, //
                                -4.0f, 4.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                4.0f, 4.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                                4.0f, -4.0f, 0.0f, 1.0f, 0.0f, 0.0f, //
                                0.0f, 0.0f, 4.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, -4.0f, 0.0f, 0.0f, 1.0f
    };

static const char *vertexShaderSource = 
            "#version 300 es\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec3 aColor;\n"
            "out vec3 inColor;\n"
            "uniform mat4 MVP;\n"
            "void main()\n"
            "{\n"
            "   inColor = aColor;\n"
            "   gl_PointSize = 2.0;\n"
            "   gl_Position = MVP * vec4(aPos, 1.0);\n"
            "}\n";

static const char *fragmentShaderSource =
            "#version 300 es\n"
            "#undef lowp\n"
            "#undef mediump\n"
            "#undef highp\n"
            "precision mediump float;\n"
            "in vec3 inColor;\n"
            "out vec4 outColor;\n"
            "void main()\n"
            "{\n"
                "outColor = vec4(inColor, 1.0);\n"
            "}\n";

openGLWidget::openGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    mModelMat.setToIdentity();
    mViewMat.setToIdentity();
    mProjMat.setToIdentity();
    mMVP = mProjMat * mViewMat * mModelMat;
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
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource))
    {
        qDebug() << (m_program->log());
    }
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource))
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

    mModelMat.setToIdentity();
    mViewMat.setToIdentity();
    mProjMat.setToIdentity();

    m_eye.setX(0.0);
    m_eye.setY(0.0);
    m_eye.setZ(50.0);

    m_eyeUp.setX(0.0);
    m_eyeUp.setY(1.0);
    m_eyeUp.setZ(0.0);
    mViewMat.lookAt(m_eye, m_eyeCenter, m_eyeUp);

    mMVP = mProjMat * mViewMat * mModelMat;

}

void openGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    mProjMat.perspective(45.0, (float)w / h, 0.1, 100);
    mMVP = mProjMat * mViewMat * mModelMat;

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
        mModelMat.rotate(1 + 8 * dy, 1 + 8 * dx, 1);
    } else if (event->buttons() & Qt::RightButton) {
        mModelMat.rotate(1 + 8 * dy, 1, 1  + 8 * dx);
    }
    mMVP = mProjMat * mViewMat * mModelMat;
    lastPos = event->pos();
    update();
}

void openGLWidget::wheelEvent(QWheelEvent *event)
{
    QOpenGLWidget::wheelEvent(event);
    float numStep = (event->angleDelta().y() / 8) / 15;
    static float fScale = 1.0;
    static float z = 4.0;
    if (numStep > 0)
    {
        z++;
        m_eye.setX(0.0);
        m_eye.setY(0.0);
        m_eye.setZ(z);

        m_eyeUp.setX(0.0);
        m_eyeUp.setY(1.0);
        m_eyeUp.setZ(0.0);
        mViewMat.lookAt(m_eye, m_eyeCenter, m_eyeUp);
    }
    else
    {
        z--;
        m_eye.setX(0.0);
        m_eye.setY(0.0);
        m_eye.setZ(z);

        m_eyeUp.setX(0.0);
        m_eyeUp.setY(1.0);
        m_eyeUp.setZ(0.0);
        mViewMat.lookAt(m_eye, m_eyeCenter, m_eyeUp);
    }
    // mModelMat.scale(fScale, fScale, fScale);
    mMVP = mProjMat * mViewMat * mModelMat;
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