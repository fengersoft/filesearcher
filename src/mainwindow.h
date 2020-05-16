#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "api/file/fileseacher.h"
#include "api/sql/sqlitedao.h"
#include <QAction>
#include <QDebug>
#include <QDesktopServices>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void loadFiles();
    void loadFilesInfo();
    void getExts();
    void openFile(int itemIndex = 4);
private slots:
    void on_btnAdd_clicked();

    void on_btnSetIndex_clicked();
    void onFindFileInfo(QString rootPath, QFileInfo& fileInfo);

    void on_edtkeyWord_returnPressed();

    void onLvFileInfoTriggered(bool checked = false);

    void on_lvFileInfo_doubleClicked(const QModelIndex& index);

    void on_btnClearIndex_clicked();

private:
    Ui::MainWindow* ui;
    QStandardItemModel* m_filesModel;
    QStandardItemModel* m_filesInfoModel;
    QStringList m_exts;
    int m_fileNum;
};
#endif // MAINWINDOW_H
