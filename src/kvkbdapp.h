/*
 * This file is part of the Kvkbd project.
 * Copyright (C) 2007-2014 Todor Gyumyushev <yodor1@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef KVKBDAPP_H
#define KVKBDAPP_H

#include <QAction>
#include <QApplication>
#include <QSignalMapper>
#include <QGridLayout>
#include <QDomNode>
#include <QDomNamedNodeMap>

#include "resizabledragwidget.h"
#include "mainwidget.h"
#include "kbdtray.h"
#include "vbutton.h"
#include "themeloader.h"
#include "kbddock.h"
#include "vkeyboard.h"

class KvkbdApp : public QApplication
{
    Q_OBJECT

public:
    using QApplication::QApplication;
    ~KvkbdApp();

    void initGui(bool loginhelper = false);

public Q_SLOTS:
    void keyProcessComplete(unsigned int);

    void buttonAction(const QString& action);
    void storeConfig();
    void toggleExtension();

    void chooseFont();
    void autoResizeFont(bool mode);
    void setStickyModKeys(bool mode);

    void partLoaded(MainWidget *vPart, int total_rows, int total_cols);
    void buttonLoaded(VButton *btn);

protected:
    QMap<QString, QString> colorMap;
    QMap<QString, MainWidget*> parts;
    QMap<QString, QRect> layoutPosition;
    QSignalMapper *signalMapper = nullptr;
    QMultiMap<QString, VButton*> actionButtons;
    KbdTray *tray = nullptr;
    KbdDock *dock = nullptr;
    VKeyboard *xkbd = nullptr;
    QGridLayout *layout = nullptr;
    ThemeLoader *themeLoader = nullptr;
    ResizableDragWidget *widget = nullptr;
    bool is_login = false;

Q_SIGNALS:
    void textSwitch(bool);
    void textSwitchLevel3(bool);
    void fontUpdated(const QFont& font);
    void startupCompleted();
};

#endif // KVKBDAPP_H
