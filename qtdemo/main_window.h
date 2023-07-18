#include <QtCore/QMutex>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>

#include <map>
#include <memory>
#include <string>

#include "openGLWidget.h"


using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

public slots:  // NOLINT
    void ActionOpenPointCloud(void);


private:
    Ui::MainWindow* ui_;

    vector<vector<double>> readPointCloud(string filePath);
};