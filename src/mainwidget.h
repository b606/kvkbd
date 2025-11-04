#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QDebug>
#include <QObjectList>
#include <QList>
#include <QWidget>
#include <QString>
#include <QFont>
#include <QSize>
#include <QResizeEvent>

#include "vkeyboard.h"


class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    void setBaseSize(int w, int h);

public Q_SLOTS:
    void textSwitch(bool);
    void textSwitchLevel3(bool);
    void updateLayout(int, const QString&);
    void updateGroupState(const ModifierGroupStateMap&);
    void updateFont(const QFont&);

protected:
    void resizeEvent(QResizeEvent *ev) override;
    QSize bsize;
};

#endif // MAINWIDGET_H
