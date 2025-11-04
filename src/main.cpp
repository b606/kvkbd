/*
 * This file is part of the Kvkbd project.
 * Copyright (C) 2007-2014 Todor Gyumyushev <yodor1@gmail.com>
 * Copyright (C) 2008 Guillaume Martres <smarter@ubuntu.com>
 * Copyright (C) 2020–2023 Anthony Fieroni, Fredrick R. Brennan and Kvkbd Developers
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <KAboutData>
#include <KLocalizedString>

#include <QCommandLineOption>
#include <QCommandLineParser>

static QString version = QLatin1String("0.8.1");

#include <X11/Xlib.h>
#include <fixx11h.h>

#include "kvkbdapp.h"

void findLoginWindow()
{
    unsigned int numkids, i, scrn;
    Window r, p;
    Window *kids = nullptr;
    Window root;
    Display *dipsy = nullptr;
    char *win_name = nullptr;

    dipsy = XOpenDisplay(nullptr);
    if (!dipsy) return;

    scrn = DefaultScreen(dipsy);
    root = RootWindow(dipsy, scrn);

    XQueryTree(dipsy, root, &r, &p, &kids, &numkids);

    for (i = 0; i < numkids;  ++i) {
        XFetchName(dipsy, kids[i], &win_name);
        QString c(QString::fromLatin1(win_name));

        if (c == QLatin1String("kvkbd.login")) {
            long wid = kids[i];
            XDestroyWindow(dipsy, wid);
            XFlush(dipsy);
            i = numkids;
        }
        XFree(win_name);
    }
    XCloseDisplay(dipsy);
}

int main(int argc, char **argv)
{
    KvkbdApp app(argc, argv);

    KLocalizedString::setApplicationDomain("kvkbd");
    KAboutData about(QLatin1String("kvkbd"), i18n("Kvkbd"), version, QLatin1String("A virtual keyboard for KDE"),
                     KAboutLicense::LGPL_V3, i18n("(C) 2007-2023 The Kvkbd Developers"));
    about.addAuthor(i18n("Todor Gyumyushev"), i18n("Original Author"), QLatin1String("yodor1@gmail.com"));
    about.addAuthor(i18n("Guillaume Martres"), i18n("KDE4 port"), QLatin1String("smarter@ubuntu.com"));
    about.addAuthor(i18n("Anthony Fieroni"), i18n("Qt5 port"), QLatin1String("bvbfan@abv.bg"));
    about.addAuthor(i18n("Fredrick R. Brennan"), i18n("General maintenance"), QLatin1String("copypaste@kittens.ph"));
    QApplication::setWindowIcon(QIcon::fromTheme(QLatin1String("preferences-desktop-keyboard")));

    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    QCommandLineOption loginhelper(QLatin1String("loginhelper"), i18n("Stand alone version for use with KDM or XDM.\n"
                                   "See Kvkbd Handbook for information on how to use this option."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(loginhelper);
    parser.process(app);

    bool is_login = parser.isSet(loginhelper);
    if (!is_login) {
        findLoginWindow();
    }

    app.initGui(is_login);

    return app.exec();
}
