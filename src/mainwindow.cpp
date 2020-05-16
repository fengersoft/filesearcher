#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList filesLabels;
    m_filesModel = new QStandardItemModel(this);
    filesLabels << "文件夹";
    ui->lvFile->setModel(m_filesModel);
    m_filesModel->setHorizontalHeaderLabels(filesLabels);
    loadFiles();
    m_filesInfoModel = new QStandardItemModel(this);
    ui->lvFileInfo->setModel(m_filesInfoModel);
    QStringList filesInfoLabels;
    filesInfoLabels << "文件名"
                    << "路径"
                    << "后缀"
                    << "类型";
    m_filesInfoModel->setHorizontalHeaderLabels(filesInfoLabels);

    m_exts << "png";
    m_exts << "jpg";
    m_exts << "doc";
    m_exts << "docx";
    m_exts << "xls";
    m_exts << "xlsx";
    m_exts << "ppt";
    m_exts << "pptx";
    m_exts << "pdf";
    m_exts << "exe";
    m_exts << "pas";
    m_exts << "h";
    m_exts << "c";
    m_exts << "cpp";
    getExts();
    QStringList lvFileInfoMenus;
    lvFileInfoMenus << "打开文件"
                    << "打开所在文件夹";
    for (int i = 0; i < lvFileInfoMenus.count(); i++) {
        QAction* act = new QAction(this);
        act->setText(lvFileInfoMenus[i]);
        ui->lvFileInfo->addAction(act);
        connect(act, &QAction::triggered, this, &MainWindow::onLvFileInfoTriggered);
    }

    showMaximized();
    ui->edtkeyWord->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFiles()
{
    m_filesModel->removeRows(0, m_filesModel->rowCount());
    QSqlQuery qry;
    QString sql = "select * from files order by id";
    sqliteDao()->sqliteWrapper()->select(sql, qry);
    QStandardItem* item;
    while (qry.next()) {
        QList<QStandardItem*> items;
        item = new QStandardItem();
        item->setText(qry.value("name").toString());
        item->setData(qry.value("id").toInt());
        items << item;
        m_filesModel->appendRow(item);
    }
    ui->lvFile->setColumnWidth(0, 500);
}

void MainWindow::loadFilesInfo()
{
}

void MainWindow::on_btnAdd_clicked()
{
    QString name = QFileDialog::getExistingDirectory();
    if (name == "") {
        return;
    }
    QString sql = QString("insert into files(name) values ('%1')").arg(name);
    sqliteDao()->sqliteWrapper()->execute(sql);
}

void MainWindow::on_btnSetIndex_clicked()
{
    m_fileNum = 0;
    ui->btnSetIndex->setEnabled(false);
    QApplication::processEvents();
    QStringList tables;
    tables << "fileinfos";
    sqliteDao()->sqliteWrapper()->truncateTables(tables);
    for (int i = 0; i < m_filesModel->rowCount(); i++) {
        FileSeacher* fileSearcher = new FileSeacher();
        QStandardItem* item = m_filesModel->item(i);
        int pid = item->data().toInt();
        fileSearcher->setPid(pid);
        sqliteDao()->transaction();
        connect(fileSearcher, &FileSeacher::findFileInfo, this, &MainWindow::onFindFileInfo);
        fileSearcher->startSearch(item->text());
        delete fileSearcher;
        sqliteDao()->commit();
    }
    getExts();
    QMessageBox::information(this, "提示", "索引建立完成。");
    ui->btnSetIndex->setEnabled(true);
    QApplication::processEvents();
    ui->lblInfo->setText("");
}

void MainWindow::onFindFileInfo(QString rootPath, QFileInfo& fileInfo)
{
    FileSeacher* fileSeacher = static_cast<FileSeacher*>(sender());
    int pid = fileSeacher->pid();
    QString ext = fileInfo.suffix().toLower();
    QString sql = QString("insert into fileinfos(pid,name,path,pathname,ext,type) "
                          "values (%1,'%2','%3','%4','%5','%6')")
                      .arg(pid)
                      .arg(fileInfo.fileName())
                      .arg(fileInfo.absolutePath())
                      .arg(fileInfo.absoluteFilePath())
                      .arg(fileInfo.isDir() == true ? "" : ext)
                      .arg(fileInfo.isDir() == true ? "文件夹" : "文件");
    sqliteDao()->sqliteWrapper()->execute(sql);
    m_fileNum++;
    if (m_fileNum % 5000 == 0) {
        ui->lblInfo->setText(QString("找到%1个文件，当前扫描到%2").arg(m_fileNum).arg(fileInfo.absoluteFilePath()));
        QApplication::processEvents();
    }
}

void MainWindow::on_edtkeyWord_returnPressed()
{
    QString sql;
    QString keyword = ui->edtkeyWord->text().simplified();
    if (keyword == "") {
        return;
    }
    keyword = keyword.replace(" ", "%");

    if (ui->rdFolder->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and type='文件夹'";
    } else if (ui->rdFile->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and type='文件'";
    } else if (ui->rdPng->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext='png'";
    } else if (ui->rdDoc->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%doc%'";
    } else if (ui->rdXls->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%xls%'";
    } else if (ui->rdPpt->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%ppt%'";
    } else if (ui->rdTxt->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%txt%'";
    } else if (ui->rdPas->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%pas%'";
    } else if (ui->rdH->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%h%'";
    } else if (ui->rdPdf->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%pdf%'";
    } else if (ui->rdCpp->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%cpp%'";
    } else if (ui->rdJpg->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%jpg%'";
    } else if (ui->rdAllExts->isChecked()) {
        sql = "select *  from fileinfos where name like '%" + keyword + "%' and ext like '%" + ui->cbbExts->currentText() + "%'";
    }

    else {
        sql = "select *  from fileinfos where name like '%" + keyword + "%'";
    }
    qDebug() << sql;
    QSqlQuery qry;
    m_filesInfoModel->removeRows(0, m_filesInfoModel->rowCount());
    sqliteDao()->sqliteWrapper()->select(sql, qry);
    QStandardItem* item;
    while (qry.next()) {
        QList<QStandardItem*> items;
        item = new QStandardItem();
        item->setText(qry.value("name").toString());
        items << item;

        item = new QStandardItem();
        item->setText(qry.value("path").toString());
        items << item;
        item = new QStandardItem();
        item->setText(qry.value("ext").toString());
        items << item;
        item = new QStandardItem();
        item->setText(qry.value("type").toString());
        items << item;
        item = new QStandardItem();
        item->setText(qry.value("pathname").toString());
        items << item;

        m_filesInfoModel->appendRow(items);
    }
    ui->lvFileInfo->setColumnHidden(4, true);
    ui->lvFileInfo->setColumnWidth(0, 300);
    ui->lvFileInfo->setColumnWidth(1, 700);
}

void MainWindow::onLvFileInfoTriggered(bool checked)
{
    QAction* act = static_cast<QAction*>(sender());

    if (act->text() == "打开文件") {
        openFile();
    } else if (act->text() == "打开所在文件夹") {
        openFile(1);
    }
}

void MainWindow::getExts()
{
    QString sql = "select distinct ext from fileinfos where ext!='' order by ext";
    QSqlQuery qry;
    sqliteDao()->sqliteWrapper()->select(sql, qry);
    ui->cbbExts->clear();
    while (qry.next()) {
        ui->cbbExts->addItem(qry.value(0).toString());
    }
}

void MainWindow::openFile(int itemIndex)
{
    QModelIndex index = ui->lvFileInfo->currentIndex();
    if (!index.isValid()) {
        return;
    }
    int row = index.row();
    QStandardItem* item = nullptr;
    item = m_filesInfoModel->item(row, itemIndex);
    QDesktopServices::openUrl(QUrl::fromLocalFile(item->text()));
}

void MainWindow::on_lvFileInfo_doubleClicked(const QModelIndex& index)
{
    openFile();
}

void MainWindow::on_btnClearIndex_clicked()
{
    QStringList tables;
    tables << "fileinfos";
    sqliteDao()->sqliteWrapper()->truncateTables(tables);
}
