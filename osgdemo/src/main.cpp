#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgUtil/Optimizer>

// 读取点云文件，临时用的
#include <fstream>
#include <sstream>
#include <vector>

int main() {

    // 新建一个 group
    osg::ref_ptr<osg::Group> root = new osg::Group();
    // 创建一个 viewer
    osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
    // 添加状态事件
    viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));

    // 新建一个 osg::GraphicsContext::Traits，描述窗口的属性
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 40;
    traits->y = 40;
    traits->width = 600;
    traits->height = 480;
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->windowName = "Point Cloud Viewer";

    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setGraphicsContext(gc.get());
    camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
    GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
    camera->setDrawBuffer(buffer);
    camera->setReadBuffer(buffer);

    // add this slave camera to the viewer, with a shift left of the projection matrix
    viewer->addSlave(camera.get());

    // 创建顶点数组
    osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array();
    // 创建颜色
    osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();

    //读取点云文件//
    int cloudNum = 0;
    {
        // 这里演示读取一个 .txt 点云文件
        std::ifstream ifs("../p213.pcd");
        std::string line;
        while (getline(ifs, line)) {
            std::stringstream ss(line);
            std::vector<double> v;
            std::string str;
            while (getline(ss, str, ' '))
                v.push_back(std::stod(str));
            coords->push_back(osg::Vec3(v[0], v[1], v[2]));
            color->push_back(osg::Vec4(v[3] / 255.0, v[4] / 255.0, v[5] / 255.0, 1.0f));

            cloudNum++;
        }
        ifs.close();
    }
    //读取点云文件//

    //创建几何体
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    // 设置顶点数组
    geometry->setVertexArray(coords.get());
    geometry->setColorArray(color.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    osg::Vec3Array *normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
    // geometry->setNormalArray(normals);
    // geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    // 设置关联方式
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, cloudNum));

    // 添加到叶节点
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable(geometry.get());
    root->addChild(geode.get());

    //root->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF | StateAttribute::OVERRIDE);

    // 优化场景数据
    osgUtil::Optimizer optimizer;
    optimizer.optimize(root.get());
    viewer->setSceneData(root.get());
    // 窗口大小变化事件
    viewer->addEventHandler(new osgViewer::WindowSizeHandler);

    viewer->realize();
    viewer->run();
    return 0;
}
