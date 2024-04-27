#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

#include "LidarGrabber.hpp"
#include "main_window.h"
#include "ui_main_window.h"

void lidarGrabber(std::string pcapFile, Ui::MainWindow *ui) {
  CLidarGrabber lg(pcapFile);

  std::vector<PointXYZI> vec;
  std::function<void(const std::vector<PointXYZI> &)> cloud_cb =
      [&vec](const std::vector<PointXYZI> &cloud) { vec = cloud; };

  lg.registerCallback(cloud_cb);

  if (!lg.isRunning()) {
    lg.start();
    while (lg.isRunning()) {
      if (!vec.empty()) {
        vector<vector<double>> vecDta;
        for (std::size_t i = 0; i < vec.size(); i++) {
          vector<double> t{vec[i].x, vec[i].y, vec[i].z, vec[i].i};
          vecDta.push_back(t);
        }
        ui->_openGLWidget->updateLidarPointCloudData(vecDta);
        vec.clear();
      }
    }
    lg.stop();
  }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  connect(ui_->pointCloudBtn, SIGNAL(triggered()), this,
          SLOT(ActionOpenPointCloud()));
  connect(ui_->testBtn, SIGNAL(triggered()), this, SLOT(ActionTest()));
  connect(ui_->actionloadLidarPoint, SIGNAL(triggered()), this,
          SLOT(ActionLoadLidarPointCloud()));
  connect(ui_->actionLightOpen, SIGNAL(triggered()), this,
          SLOT(ActionLightOpen()));
  connect(ui_->actionLight_Off, SIGNAL(triggered()), this,
          SLOT(ActionLightOff()));
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::ActionTest(void) { ui_->_openGLWidget->m_nDrawType = 0; }

void MainWindow::ActionLoadLidarPointCloud(void) {
  ui_->_openGLWidget->m_nDrawType = 2;
  std::ifstream ifs("filePath.txt");
  std::string filePath = "/tmp/sensor/lidar/test.pcap";
  while (true) {
    getline(ifs, filePath);
    if (filePath.find(".pcap") != string::npos) {
      break;
    }
  }
  std::thread *ThreadHandle = new std::thread(lidarGrabber, filePath, ui_);
}

void MainWindow::ActionLightOpen(void) { ui_->_openGLWidget->LightOn(); }

void MainWindow::ActionLightOff(void) { ui_->_openGLWidget->LightOff(); }

vector<vector<double>> MainWindow::readPointCloud(string filePath) {
  vector<vector<double>> retVec;
  string comment = "";
  string VERSION = "0.7";
  string FIELDS = "";
  vector<string> vecFields;
  string SIZE = "";
  vector<int> vecSieze;
  string TYPE = "";
  string COUNT = "";
  string WIDTH = "";
  string HEIGHT = "";
  string VIEWPOINT = "";
  string POINTS = "";
  string DATA;
  std::ifstream ifs(filePath);
  std::string line;
  while (getline(ifs, line)) {
    if (line.find("PCD") != string::npos) {
      comment = line;
      continue;
    } else if (line.find("VERSION") != string::npos) {
      VERSION = line;
      continue;
    } else if (line.find("FIELDS") != string::npos) {
      FIELDS = line;
      std::stringstream ss(line);
      std::string str;
      while (getline(ss, str, ' ')) {
        if (str == "FIELDS")
          continue;
        vecFields.push_back(str);
      }
      continue;
    } else if (line.find("SIZE") != string::npos) {
      SIZE = line;
      std::stringstream ss(line);
      std::string str;
      while (getline(ss, str, ' ')) {
        int nSize = atoi(str.c_str());
        if (0 == nSize)
          continue;
        vecSieze.push_back(nSize);
      }
      continue;
    } else if (line.find("TYPE") != string::npos) {
      TYPE = line;
      continue;
    } else if (line.find("COUNT") != string::npos) {
      COUNT = line;
      continue;
    } else if (line.find("WIDTH") != string::npos) {
      WIDTH = line;
      continue;
    } else if (line.find("HEIGHT") != string::npos) {
      HEIGHT = line;
      continue;
    } else if (line.find("VIEWPOINT") != string::npos) {
      VIEWPOINT = line;
      continue;
    } else if (line.find("POINTS") != string::npos) {
      POINTS = line;
      continue;
    } else if (line.find("DATA") != string::npos) {
      DATA = line;
      break;
    }
    break;
  }
  char c[4];
  float t = 0.0;
  for (int i = 0;; i++) {
    int nIndex = i % vecFields.size();
    if (ifs.read(c, vecSieze[nIndex])) {
      if (vecFields[nIndex] == "x") {
        vector<double> v;
        memcpy(&t, c, vecSieze[nIndex]);
        v.push_back(t);

        i++;
        nIndex = i % vecFields.size();
        if (ifs.read(c, vecSieze[nIndex])) {
          if (vecFields[nIndex] == "y") {
            memcpy(&t, c, vecSieze[nIndex]);
            v.push_back(t);

            i++;
            nIndex = i % vecFields.size();
            if (ifs.read(c, vecSieze[nIndex])) {
              if (vecFields[nIndex] == "z") {
                memcpy(&t, c, vecSieze[nIndex]);
                v.push_back(t);
                retVec.push_back(v);
              }
            } else {
              break;
            }
          }
        } else {
          break;
        }
      }
    } else {
      break;
    }
  }
  ifs.close();
  return retVec;
}

void MainWindow::ActionOpenPointCloud(void) {
  std::ifstream ifs("filePath.txt");
  std::string filePath;
  getline(ifs, filePath);
  vector<vector<double>> vec = readPointCloud(filePath);
  ui_->_openGLWidget->m_nDrawType = 1;
  ui_->_openGLWidget->updatePointCloudData(vec);
}