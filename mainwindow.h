#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sxcuinterface.h"
#include "persistencemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SXCUInterface *sxcu_interface;
    QString *currentFile;
    QString *currentLink;
    SXCUInterface::SXCUCollection *currentCollection;
    PersistenceManager *pman;

    void showCollectionRemover();
    void showCollectionAdder();
    void import(QString& filepath);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

public slots:

    void collectionSelectionChanged(const QString &content);
    void uploadButtonClicked();
    void importButtonClicked();
    void copyToCBoardClicked();

    void importEditChanged(const QString &content);

    void entryDoubleClicked(const QModelIndex &index);
    void removeEntry();

};

/*
class DragDropGScene : QGraphicsScene {
protected:
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
};
*/
#endif // MAINWINDOW_H
