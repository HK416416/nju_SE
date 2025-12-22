#include <QApplication>
#include <QMainWindow>
#include "gui/MainWindow.h"

int main(int argc, char *argv[])
{
    // 设置高 DPI 支持
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    QApplication::setApplicationName("家庭记账本");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("FamilyBudget");
    
    // 设置中文字体
    QFont font("Microsoft YaHei", 9);
    QApplication::setFont(font);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}