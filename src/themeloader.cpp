/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  Todor <email>
 * Copyright (C) 2020–2023 Anthony Fieroni, Fredrick R. Brennan and Kvkbd Developers
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

#include "themeloader.h"

#include <QActionGroup>
#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QFileInfo>
#include <QDir>
#include <QMenu>
#include <QStandardPaths>

int defaultWidth = 25;
int defaultHeight = 25;

#define DEFAULT_CSS QLatin1String(":/colors/standard.css")

ThemeLoader::ThemeLoader(QWidget *parent) : QObject(parent)
{
}

ThemeLoader::~ThemeLoader()
{
}

void ThemeLoader::loadTheme(QString &themeName)
{
    bool loading = true;
    while (loading) {
        int ret = this->loadLayout(themeName, QLatin1String(":/themes/"));
        if (ret == 0) {
            break;
        }
        themeName = QLatin1String("standard");
    }
}
void ThemeLoader::loadColorFile(const QString &fileName)
{
    QFile themeFile;

    themeFile.setFileName(fileName);

    if (!themeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QMessageBox::information(nullptr, QLatin1String("Error"), QLatin1String("Unable to open css file: %1").arg(themeFile.fileName()));
        return;
    }

    ((QWidget *)parent())->setStyleSheet(QString::fromLatin1(themeFile.readAll()));
    ((QWidget *)parent())->setProperty("colors", fileName);
    themeFile.close();

    ((QWidget *)parent())->repaint();

    Q_EMIT colorStyleChanged();
}
void ThemeLoader::loadColorStyle()
{
    QAction *action = (QAction *)QObject::sender();

    QFileInfo info(action->data().toString());

    this->loadColorFile(info.absoluteFilePath());
}
void ThemeLoader::findColorStyles(QMenu *colors, const QString &configSelectedStyle)
{
    QActionGroup *color_group = new QActionGroup(colors);
    color_group->setExclusive(true);
    colors->setTitle(QLatin1String("Color Style"));
    colors->setIcon(QIcon::fromTheme(QLatin1String("preferences-desktop-color")));

    QDir colors_dir(QLatin1String(":/colors"), QLatin1String("*.css"), QDir::Name, QDir::Files | QDir::Readable);
    QFileInfoList list = colors_dir.entryInfoList();
    QListIterator<QFileInfo> itr(list);
    while (itr.hasNext()) {
        QFileInfo fileInfo = itr.next();
        QAction *item = new QAction(colors);
        item->setCheckable(true);
        item->setText(fileInfo.baseName());
        item->setData(fileInfo.absoluteFilePath());
        colors->addAction(item);
        color_group->addAction(item);
    }

    QString selectedStyle = configSelectedStyle;
    if (selectedStyle.length() < 1) {
        selectedStyle = DEFAULT_CSS;
    }
    QAction *selectedAction = nullptr;

    QListIterator<QAction *> itrActions(color_group->actions());
    while (itrActions.hasNext()) {
        QAction *item = itrActions.next();

        if (item->data().toString() == selectedStyle) {
            item->setChecked(true);
            selectedAction = item;
        }
        connect(item, SIGNAL(triggered(bool)), this, SLOT(loadColorStyle()));
    }

    if (selectedAction) {
        selectedAction->trigger();
    }
}

int ThemeLoader::loadLayout(const QString &themeName, const QString &path)
{
    QFile themeFile;
    QDomDocument doc;

    themeFile.setFileName(QString(path + QLatin1String("%1.xml")).arg(themeName));

    if (!themeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(nullptr, QLatin1String("Error"), QLatin1String("Unable to open theme xml file: %1").arg(themeFile.fileName()));
        return -1;
    }
    if (!doc.setContent(&themeFile)) {
        QMessageBox::information(nullptr, QLatin1String("Error"), QLatin1String("Unable to parse theme xml file: %1").arg(themeFile.fileName()));
        return -2;
    }
    themeFile.close();

    QDomElement docElem = doc.documentElement();

    QDomNodeList wList = docElem.elementsByTagName(QLatin1String("buttonWidth"));
    QDomNode wNode = wList.at(0);

    //read default button width
    defaultWidth = wNode.attributes().namedItem(QLatin1String("width")).toAttr().value().toInt();

    QDomNodeList nList = (wNode.toElement()).elementsByTagName(QLatin1String("item"));
    for (int a = 0; a < nList.count(); a++) {
        QDomNode node = nList.at(a);
        int width = node.attributes().namedItem(QLatin1String("width")).toAttr().value().toInt();
        QString hintName = node.attributes().namedItem(QLatin1String("name")).toAttr().value();
        widthMap.insert(hintName, width);
    }

    wList = docElem.elementsByTagName(QLatin1String("buttonHeight"));
    wNode = wList.at(0);
    nList = (wNode.toElement()).elementsByTagName(QLatin1String("item"));
    for (int a = 0; a < nList.count(); a++) {
        QDomNode node = nList.at(a);
        int height = node.attributes().namedItem(QLatin1String("height")).toAttr().value().toInt();
        QString hintName = node.attributes().namedItem(QLatin1String("name")).toAttr().value();
        heightMap.insert(hintName, height);
    }

    wList = docElem.elementsByTagName(QLatin1String("spacingHints"));
    wNode = wList.at(0);
    nList = (wNode.toElement()).elementsByTagName(QLatin1String("item"));
    for (int a = 0; a < nList.count(); a++) {
        QDomNode node = nList.at(a);
        int width = node.attributes().namedItem(QLatin1String("width")).toAttr().value().toInt();
        QString hintName = node.attributes().namedItem(QLatin1String("name")).toAttr().value();
        spacingMap.insert(hintName, width);
    }

    wList = docElem.elementsByTagName(QLatin1String("part"));
    wNode = wList.at(0);

    //insert main part to widget
    QString partName = wNode.attributes().namedItem(QLatin1String("name")).toAttr().value();

    MainWidget *part = new MainWidget((QWidget *)parent());
    part->setProperty("part", QLatin1String("main"));

    loadKeys(part, wNode);

    wList = wNode.childNodes();

    for (int a = 0; a < wList.size(); a++) {

        QDomNode wNode = wList.at(a);
        if (wNode.toElement().tagName() == QLatin1String("extension")) {
            MainWidget *widget1 = new MainWidget((QWidget *)parent());
            widget1->setProperty("part", QLatin1String("extension"));
            loadKeys(widget1, wNode);
            break;
        }
    }
    return 0;
}
bool ThemeLoader::applyProperty(VButton *btn, const QString &attributeName, QDomNamedNodeMap *attributes, QVariant defaultValue)
{
    bool ret = false;

    QString attributeValue = attributes->namedItem(attributeName).toAttr().value();
    if (attributeValue.length() > 0) {
        btn->setProperty(qPrintable(attributeName), attributeValue);
        ret = true;
    } else {
        if (defaultValue.toString().length() > 0) {
            btn->setProperty(qPrintable(attributeName), defaultValue);
            ret = true;
        }
    }
    return ret;
}
void ThemeLoader::loadKeys(MainWidget *vPart, const QDomNode &wNode)
{
    int max_sx = 0;
    int max_sy = 0;

    int sx = 0;
    int sy = 0;
    int rowMarginLeft = 0;
    int rowSpacingY = 0;
    int rowSpacingX = 0;

    int total_cols = 0;
    int total_rows = 0;

    QDomNodeList nList = wNode.childNodes();

    for (int a = 0; a < nList.size(); a++) {

        QDomNode wNode = nList.at(a);
        if (wNode.toElement().tagName() != QLatin1String("row")) continue;

        total_rows++;

        int rowHeight = defaultHeight;

        int row_buttons = 0;

        QDomNodeList key_list = wNode.childNodes();

        QString rowHeightHint = wNode.attributes().namedItem(QLatin1String("height")).toAttr().value();
        if (heightMap.contains(rowHeightHint)) {
            rowHeight = heightMap.value(rowHeightHint);
        }

        for (int b = 0; b < key_list.count(); b++) {
            QDomNode node = key_list.at(b);
            QDomNamedNodeMap attributes = node.attributes();

            if (node.toElement().tagName() == QLatin1String("key")) {

                VButton *btn = new VButton(vPart);
                row_buttons++;

                //width
                int buttonWidth = defaultWidth;
                int buttonHeight = defaultHeight;

                QString widthHint = attributes.namedItem(QLatin1String("width")).toAttr().value();
                if (widthMap.contains(widthHint)) {
                    buttonWidth = widthMap.value(widthHint);
                }

                QString heightHint = attributes.namedItem(QLatin1String("height")).toAttr().value();
                if (heightMap.contains(heightHint)) {
                    buttonHeight = heightMap.value(heightHint);
                }

                //name
                QString button_name = attributes.namedItem(QLatin1String("name")).toAttr().value();
                if (button_name.length() > 0) {
                    btn->setObjectName(button_name);
                }

                if (applyProperty(btn, QLatin1String("label"), &attributes)) {
                    btn->setText(btn->property("label").toString());
                }

                applyProperty(btn, QLatin1String("group_label"), &attributes);
                applyProperty(btn, QLatin1String("group_toggle"), &attributes);
                applyProperty(btn, QLatin1String("group_name"), &attributes);
                applyProperty(btn, QLatin1String("colorGroup"), &attributes, QLatin1String("normal"));
                // TODO: tooltip does not show
                applyProperty(btn, QLatin1String("tooltip"), &attributes);

                QString modifier = attributes.namedItem(QLatin1String("modifier")).toAttr().value();
                if (modifier.toInt() > 0) {
                    btn->setProperty("modifier", true);
                    btn->setCheckable(true);
                }

                unsigned int key_code = attributes.namedItem(QLatin1String("code")).toAttr().value().toInt();
                if (key_code > 0) {
                    btn->setKeyCode(key_code);
                }

                if (applyProperty(btn, QLatin1String("action"), &attributes)) {
                    btn->setProperty("action", btn->property("action").toString());
                }

                int is_checkable = attributes.namedItem(QLatin1String("checkable")).toAttr().value().toInt();
                if (is_checkable > 0) {
                    btn->setCheckable(true);
                    btn->setChecked(false);
                }

                btn->move(sx, sy);
                btn->resize(buttonWidth, buttonHeight);
                btn->storeSize();

                sx += buttonWidth + rowSpacingX;

                Q_EMIT buttonLoaded(btn);
            } else if (node.toElement().tagName() == QLatin1String("spacing")) {

                QString widthHint = attributes.namedItem(QLatin1String("width")).toAttr().value();
                QString heightHint = attributes.namedItem(QLatin1String("height")).toAttr().value();

                if (spacingMap.contains(widthHint)) {
                    int spacingWidth = spacingMap.value(widthHint);
                    sx += spacingWidth;
                }
                if (heightMap.contains(heightHint)) {
                    int spacingHeight = heightMap.value(heightHint);
                    if (spacingHeight > rowHeight) rowHeight = spacingHeight;
                }
            }
        }//row

        if (sx > max_sx) max_sx = sx;

        sy += (rowHeight + rowSpacingY);
        sx = 0 + rowMarginLeft;

        if (row_buttons > total_cols) total_cols = row_buttons;

    }
    if (sy > max_sy) max_sy = sy;

    vPart->setBaseSize(max_sx, max_sy);

    Q_EMIT partLoaded(vPart, total_rows, total_cols);

}
