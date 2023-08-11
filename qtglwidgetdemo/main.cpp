
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>

#include <QtWidgets/QApplication>

#include "openGLWidget.h"
#include "LidarGrabber.hpp"

void lidarGrabber(std::string pcapFile, openGLWidget* _openGLWidget) {
    CLidarGrabber lg(pcapFile);

    std::vector<PointXYZI> vec;
    std::function<void (const std::vector<PointXYZI> &)> cloud_cb =
        [&vec](const std::vector<PointXYZI> &cloud) {vec = cloud;};
    
    lg.registerCallback(cloud_cb);

    if (!lg.isRunning())
    {
        lg.start();
        while (lg.isRunning())
        {
            if (!vec.empty())
            {   vector<vector<double>> vecDta;
                for (std::size_t i = 0; i < vec.size(); i++)
                {
                    vector<double> t{vec[i].x, vec[i].y, vec[i].z, vec[i].i};
                    vecDta.push_back(t);
                }
                _openGLWidget->updateLidarPointCloudData(vecDta);
                vec.clear();
            }
        }
        lg.stop();
    }
}

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    openGLWidget w;
    w.setWindowTitle("Point Cloud Viewer");
    w.show();

    std::ifstream ifs("filePath.txt");
    std::string filePath = "/tmp/sensor/lidar/test.pcap";
    while (true)
    {
        getline(ifs, filePath);
        if (filePath.find(".pcap") != string::npos)
        {
            break;
        }
    }
    std::thread *ThreadHandle = new std::thread(lidarGrabber, filePath, &w);
    ThreadHandle->detach();

    return a.exec();
}