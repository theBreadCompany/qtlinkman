#ifndef DRAGDROPGRAPHICSVIEW_H
#define DRAGDROPGRAPHICSVIEW_H

#include <QGraphicsView>

class DragDropGraphicsView : public QGraphicsView
{
public:
    DragDropGraphicsView();
protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
};

#endif // DRAGDROPGRAPHICSVIEW_H
