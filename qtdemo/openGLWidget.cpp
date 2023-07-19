#include "openGLWidget.h"
#include <QtOpenGL/QGLWidget>

openGLWidget::openGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    fScale = 1.0;
    m_nDraw = 0;
    m_bLight = true;
}

void openGLWidget::updatePointCloudData(const vector<vector<double>> &data)
{
    pointCloudData = data;
}

void openGLWidget::LightOn()
{
    m_bLight = true;
}

void openGLWidget::LightOff()
{
    m_bLight = false;
}

void openGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;        
    }
}

void openGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
    }
}

void openGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
    }
}

void openGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void openGLWidget::resizeGL(int w, int h)
{
    int side = qMin(width(), height());
    glViewport((width() - side) / 2, (height() - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void openGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
    glScalef(fScale, fScale, fScale);
    
    if (m_bLight)
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
    else
    {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
    }
    draw();
    
}

void openGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void openGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }

    lastPos = event->pos();
    update();
}

void openGLWidget::wheelEvent(QWheelEvent *event)
{
    QOpenGLWidget::wheelEvent(event);
    float numStep = (event->angleDelta().y() / 8) / 15;
    if (numStep > 0)
    {
        fScale /= 1.1;
    }
    else
    {
        fScale *= 1.1;
    }
    
    update();
}

void openGLWidget::draw()
{
    if (m_nDraw)
    {
        drawPointCloud();
    }
    else
    {
        drawTest();
    }
}

void openGLWidget::drawTest()
{
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

void openGLWidget::drawTriangle()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕和深度缓存
    glLoadIdentity();                                   //重置当前的模型观察矩阵
 
    glTranslatef(-1.5f, 0.0f, -6.0f);                   //左移1.5单位，并移入屏幕6.0单位
    glBegin(GL_TRIANGLES);                              //开始绘制三角形
        glColor3f(1.0f, 0.0f, 0.0f);                    //设置当前色为红色
        glVertex3f(0.0f, 1.0f, 0.0f);                   //上顶点
        glColor3f(0.0f, 1.0f, 0.0f);                    //设置当前色为绿色
        glVertex3f(-1.0f, -1.0f, 0.0f);                 //左下
        glColor3f(0.0f, 0.0f, 1.0f);                    //设置当前色为蓝色
        glVertex3f(1.0f, -1.0f, 0.0f);                  //右下
    glEnd();                                            //三角形绘制结束
 
    glTranslatef(3.0f, 0.0f, 0.0f);                     //右移3.0单位
    glColor3f(0.5f, 0.5f, 1.0f);                        //一次性将当前色设置为蓝色
    glBegin(GL_QUADS);                                  //开始绘制四边形
        glVertex3f(-1.0f, 1.0f, 0.0f);                  //左上
        glVertex3f(1.0f, 1.0f, 0.0f);                   //右上
        glVertex3f(1.0f, -1.0f, 0.0f);                  //左下
        glVertex3f(-1.0f, -1.0f, 0.0f);                 //右下
    glEnd();
}

void openGLWidget::drawPointCloud()
{
    glPointSize(1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    for (int i = 0; i < pointCloudData.size(); i++)
    {
        glBegin(GL_POINTS);
        std::vector<double> v = pointCloudData[i];
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(v[0], v[1], v[2]);
        glEnd();
    }
    glFlush();//保证前面的命令立即执行
    glPopMatrix();
}

void openGLWidget::qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}
