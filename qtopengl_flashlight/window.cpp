#include "window.h"

#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

Window::Window(QWidget* parent)
	: QOpenGLWidget(parent)
	, materialV(nullptr)
	, materialF(nullptr)
	, lightF(nullptr)
	, lightV(nullptr)
	, cube(0)
	, textureMaterial(0)
	, textureSpecular(0)
	, angularSpeed(0)
{
	fps = 60.0;
	connect(&timer, SIGNAL(timeout()), this, SLOT(processTimeout()));
	timer.start(1000 / 60);
}

Window::~Window()
{
	makeCurrent();
	delete materialF;
	delete materialV;
	delete lightF;
	delete lightV;
	delete textureMaterial;
	delete textureSpecular;
	delete cube;
	doneCurrent();
}

void Window::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
		case Qt::Key_A:
		{
			shininess += 8.0;
			break;
		}
		case Qt::Key_D:
		{
			shininess -= 8.0;
			if (shininess < 8.0)
			{
				shininess = 8.0;
			}
			break;
		}
		default:
			break;
	}
	event->accept();
}

void Window::mousePressEvent(QMouseEvent* e)
{
	// Save mouse press position
	mousePressPosition = QVector2D(e->localPos());
}

void Window::mouseReleaseEvent(QMouseEvent* e)
{
	// Mouse release position - mouse press position
	QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

	// Rotation axis is perpendicular to the mouse position difference
	// vector
	QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

	// Accelerate angular speed relative to the length of the mouse sweep
	qreal acc = diff.length() / 100.0;

	// Calculate new rotation axis as weighted sum
	rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

	// Increase angular speed
	angularSpeed += acc;
}

void Window::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	initShaders();
	initTextures();
	initDatas();
	cube = new Cube;
}

void Window::processTimeout()
{

	// Decrease angular speed (friction)
	angularSpeed *= 0.99;

	// Stop rotation when speed goes below threshold
	if (angularSpeed < 0.01)
	{
		angularSpeed = 0.0;
	}
	else
	{
		// Update rotation
		rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
		// Request an update
	}
	update();
}

void Window::useMaterialShader()
{
	program.removeAllShaders();

	if (!program.addShader(materialV))
	{
		qDebug() << "add vertex shader file failed.";
	}
	if (!program.addShader(materialF))
	{
		qDebug() << "add fragment shader file failed.";
	}
	if (!program.link())
	{
		qDebug() << "program link failed";
	}
	if (!program.bind())
	{
		qDebug() << "program bind failed";
	}
}

void Window::useLightShader()
{
	program.removeAllShaders();
	if (!program.addShader(lightV))
	{
		qDebug() << "add vertex shader file failed.";
	}
	if (!program.addShader(lightF))
	{
		qDebug() << "add fragment shader file failed.";
	}
	if (!program.link())
	{
		qDebug() << "program link failed";
	}
	if (!program.bind())
	{
		qDebug() << "program bind failed";
	}
}

void Window::initShaders()
{
	bool ret  = false;
	materialV = new QOpenGLShader(QOpenGLShader::Vertex);
	ret		  = materialV->compileSourceFile("res/shader/vshader.glsl");
	if (!ret)
	{
		qDebug() << "compile shader failed -1";
	}

	materialF = new QOpenGLShader(QOpenGLShader::Fragment);
	ret		  = materialF->compileSourceFile("res/shader/fshader.glsl");
	if (!ret)
	{
		qDebug() << "compile shader failed -2";
	}

	lightV = new QOpenGLShader(QOpenGLShader::Vertex);
	ret	   = lightV->compileSourceFile("res/shader/light.vsh");
	if (!ret)
	{
		qDebug() << "compile shader failed -3";
	}

	lightF = new QOpenGLShader(QOpenGLShader::Fragment);
	ret	   = lightF->compileSourceFile("res/shader/light.fsh");
	if (!ret)
	{
		qDebug() << "compile shader failed -4";
	}
}

void Window::initTextures()
{
	textureMaterial = new QOpenGLTexture(QImage("res/image/container.png").mirrored());
	textureMaterial->setMinificationFilter(QOpenGLTexture::Nearest);
	textureMaterial->setMagnificationFilter(QOpenGLTexture::Linear);
	textureMaterial->setWrapMode(QOpenGLTexture::Repeat);

	textureSpecular = new QOpenGLTexture(QImage("res/image/container2_specular.png").mirrored());
	textureSpecular->setMinificationFilter(QOpenGLTexture::Nearest);
	textureSpecular->setMagnificationFilter(QOpenGLTexture::Linear);
	textureSpecular->setWrapMode(QOpenGLTexture::Repeat);
}

void Window::initDatas()
{
	resize(width(), height());

	view.setToIdentity();
	view.lookAt(QVector3D(0.0f, 0.0f, 3.0f), QVector3D(0.0f, 0.0f, 2.0f), QVector3D(0.0f, 1.0f, 0.0f));
	model.setToIdentity();

	lightDir   = QVector3D(0.f, 0.f, -3.0f);
	lightColor = QVector3D(1.0f, 1.0f, 1.0f);

	shininess = 64.0f;

	lightAmibent  = QVector3D(0.2f, 0.2f, 0.2f);
	lightDiffuse  = QVector3D(0.5f, 0.5f, 0.5f);
	lightSpecular = QVector3D(1.0f, 1.0f, 1.0f);

	cubePositions << QVector3D(0.0f, 0.0f, 0.0f) << QVector3D(2.0f, 5.0f, -15.0f) << QVector3D(-1.5f, -2.2f, -2.5f) << QVector3D(-3.8f, -2.0f, -12.3f)
				  << QVector3D(2.4f, -0.4f, -3.5f) << QVector3D(-1.7f, 3.0f, -7.5f) << QVector3D(1.3f, -2.0f, -2.5f) << QVector3D(1.5f, 2.0f, -2.5f)
				  << QVector3D(1.5f, 0.2f, -1.5f) << QVector3D(-1.3f, 1.0f, -1.5f);
}

void Window::resizeGL(int w, int h)
{
	Q_ASSERT(h);
	glViewport(0, 0, w, h);
	float ratio	 = (float)w / h;
	float aspect = 45;
	projection.setToIdentity();
	projection.perspective(aspect, ratio, 0.1f, 100.0f);
}

void Window::paintGL()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//    static float angle = 0;
	//    angle += 10;

	//    float radian = qDegreesToRadians(angle);
	//    float ratio = sin(radian) ;
	//    lightPos = QVector3D(ratio * 2.0f, 0.1f, 1.0f);

	//    lightColor.setX(sin(qDegreesToRadians(angle)) * 2.0);
	//    lightColor.setY(sin(qDegreesToRadians(angle)) * 0.7);
	//    lightColor.setZ(sin(qDegreesToRadians(angle)) * 1.3);

	//    lightDiffuse   = lightColor * QVector3D(0.5f, 0.5f, 0.5f);
	//    lightAmibent   = lightDiffuse * QVector3D(0.2f, 0.2f, 0.2f);

	{
		//        useLightShader();
		//        program.setUniformValue("lightColor", lightColor);
		//        model.setToIdentity();

		//        model.translate(lightPos);
		//        int modelLoc    = program.uniformLocation("modelMat");
		//        int projectLoc  = program.uniformLocation("projectMat");
		//        int viewLoc     = program.uniformLocation("viewMat");
		//        program.setUniformValue(modelLoc, model);
		//        program.setUniformValue(projectLoc, projection);
		//        program.setUniformValue(viewLoc, view);

		//        cube->drawCube(&program);
	}

	{
		useMaterialShader();

		int diffuseLoc	= program.uniformLocation("material.diffuse");
		int specularLoc = program.uniformLocation("material.specular");
		int shineLoc	= program.uniformLocation("material.shininess");

		program.setUniformValue(shineLoc, shininess);

		int lightAmbientLoc	 = program.uniformLocation("light.ambient");
		int lightDiffuseLoc	 = program.uniformLocation("light.diffuse");
		int lightSpecularLoc = program.uniformLocation("light.specular");
		int lightDirLoc		 = program.uniformLocation("light.direction");

		int lightPosLoc	   = program.uniformLocation("light.position");
		int constantLoc	   = program.uniformLocation("light.constant");
		int linearLoc	   = program.uniformLocation("light.linear");
		int quadraticLoc   = program.uniformLocation("light.quadratic");
		int cutoffLoc	   = program.uniformLocation("light.cutoff");
		int outerCutOffLoc = program.uniformLocation("light.outerCutOff");

		program.setUniformValue(lightAmbientLoc, lightAmibent);
		program.setUniformValue(lightDiffuseLoc, lightDiffuse);
		program.setUniformValue(lightSpecularLoc, lightSpecular);
		program.setUniformValue(lightDirLoc, lightDir);

		program.setUniformValue(lightPosLoc, QVector3D(0.0, 0.0, 3.0));
		program.setUniformValue(constantLoc, (GLfloat)1.0f);
		program.setUniformValue(linearLoc, (GLfloat)0.09);
		program.setUniformValue(quadraticLoc, (GLfloat)0.032);
		program.setUniformValue(cutoffLoc, (GLfloat)cos(qDegreesToRadians(6.)));
		program.setUniformValue(outerCutOffLoc, (GLfloat)cos(qDegreesToRadians(9.)));

		int viewPosLoc = program.uniformLocation("viewPos");
		program.setUniformValue(viewPosLoc, QVector3D(0.0f, 0.0f, 3.0f));

		glActiveTexture(GL_TEXTURE0);
		textureMaterial->bind();
		program.setUniformValue(diffuseLoc, 0);

		glActiveTexture(GL_TEXTURE1);
		textureSpecular->bind();
		program.setUniformValue(specularLoc, 1);

		int projectLoc = program.uniformLocation("projectMat");
		int viewLoc	   = program.uniformLocation("viewMat");

		program.setUniformValue(projectLoc, projection);
		program.setUniformValue(viewLoc, view);

		for (auto i : cubePositions)
		{
			model.setToIdentity();
			model.translate(i);
			model.rotate(rotation);
			int modelLoc = program.uniformLocation("modelMat");
			program.setUniformValue(modelLoc, model);

			cube->drawCube(&program);
		}
	}
	calcFPS();
	paintFPS();
}

void Window::calcFPS()
{
	static QTime time;
	static int	 once = [=]() {
		  time.start();
		  return 0;
	}();
	Q_UNUSED(once)

	static int frame = 0;
	if (frame++ > 100)
	{
		qreal elasped = time.elapsed();
		updateFPS(frame / elasped * 1000);
		time.restart();
		frame = 0;
	}
}

void Window::updateFPS(qreal v)
{
	fps = v;
}

void Window::paintFPS()
{
	static int count = 0;
	count++;
	if (count >= 100)
	{
		count = 0;
		qDebug() << "\033[32m" << fps << "\033[0m";

		QString str = QString("FPS:%1").arg(QString::number(fps, 'f', 3));
		this->setWindowTitle(str);
	}
}
