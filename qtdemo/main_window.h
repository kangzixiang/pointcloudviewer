#include <QtCore/QMutex>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>

#include <map>
#include <memory>
#include <string>

#include "openGLWidget.h"

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
};