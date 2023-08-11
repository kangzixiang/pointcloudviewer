
#include <QtOpenGL/QGLWidget>

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

}

void openGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

}

void openGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
    // glLoadIdentity();
    // glTranslatef(0.0, 0.0, -10.0);
    
    draw();
    
}

void openGLWidget::mousePressEvent(QMouseEvent *event)
{
    
}

void openGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    update();
}

void openGLWidget::wheelEvent(QWheelEvent *event)
{
    QOpenGLWidget::wheelEvent(event);
    float numStep = (event->angleDelta().y() / 8) / 15;
    
    update();
}

void openGLWidget::draw()
{
    drawTriangleTest();
}

void openGLWidget::drawTest() {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glNormal3f(0,0,-1);
        glVertex3f(-1,-1,0);
        glVertex3f(-1,1,0);
        glVertex3f(1,1,0);
        glVertex3f(1,-1,0);
    glEnd();
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
        glNormal3f(0,-1,0.707);
        glVertex3f(-1,-1,0);
        glVertex3f(1,-1,0);
        glVertex3f(0,0,1.2);
    glEnd();
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
        glNormal3f(1,0, 0.707);
        glVertex3f(1,-1,0);
        glVertex3f(1,1,0);
        glVertex3f(0,0,1.2);
    glEnd();
    glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
        glNormal3f(0,1,0.707);
        glVertex3f(1,1,0);
        glVertex3f(-1,1,0);
        glVertex3f(0,0,1.2);
    glEnd();
    glColor3f(1.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
        glNormal3f(-1,0,0.707);
        glVertex3f(-1,1,0);
        glVertex3f(-1,-1,0);
        glVertex3f(0,0,1.2);
    glEnd();
}

void openGLWidget::drawTriangleTest() {
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

    QMatrix4x4 m;
    m.rotate(45, 0.0f, 0.0f, 1.0f);
    m_program->setUniformValue("MVP", m);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_vbo.release();
    m_vao.release();
    m_program->release();
}
