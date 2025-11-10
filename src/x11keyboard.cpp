/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014 Todor Gyumyushev <yodor1@gmail.com>
 * Copyright (C) 2020–2025 Anthony Fieroni, Fredrick R. Brennan, b606 and Kvkbd Developers
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

#include <QDebug>
#include <QDataStream>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

// TODO: QNativeInterface::QX11Application (Qt 6.2+)
// #include <QGuiApplication>

#include <X11/extensions/XTest.h>
#include <X11/Xlocale.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/XKBlib.h>
#include <fixx11h.h>

#include "keysymconvert.h"
#include "vbutton.h"
#include "kbdlayout.h"
#include "x11keyboard.h"

extern QList<VButton *> modKeys;

X11Keyboard::X11Keyboard(QObject *parent): VKeyboard(parent)
{
    // Opening a display every 250ms in queryModKeyState drains X11 resources.
    m_display = XOpenDisplay(nullptr);
    assert(m_display != nullptr);

    KbdLayout::registerMetaType();
    QString service = QLatin1String("");
    QString path = QLatin1String("/Layouts");
    QString interface = QLatin1String("org.kde.KeyboardLayouts");

    QDBusConnection session = QDBusConnection::sessionBus();

    session.connect(service, path, interface, QLatin1String("layoutChanged"), this, SLOT(layoutChanged()));
    session.connect(service, path, interface, QLatin1String("layoutListChanged"), this, SLOT(constructLayouts()));

    groupTimer = new QTimer(parent);
    groupTimer->setInterval(250);

    groupState.insert(QLatin1String("capslock"), this->queryModKeyState(XK_Caps_Lock));
    groupState.insert(QLatin1String("numlock"), this->queryModKeyState(XK_Num_Lock));
    groupState.insert(QLatin1String("shitlevel3"), this->queryModKeyState(XK_ISO_Level3_Shift));

    connect(groupTimer, SIGNAL(timeout()), this, SLOT(queryModState()));
}

X11Keyboard::~X11Keyboard()
{
    XCloseDisplay(m_display);
}

void X11Keyboard::start()
{
    constructLayouts();
    layoutChanged();
    Q_EMIT groupStateChanged(groupState);
    groupTimer->start();
}

void X11Keyboard::constructLayouts()
{
    QDBusInterface iface(QLatin1String("org.kde.keyboard"), QLatin1String("/Layouts"), QLatin1String("org.kde.KeyboardLayouts"), QDBusConnection::sessionBus());

    QDBusReply<QList<KbdLayout>> reply = iface.call(QLatin1String("getLayoutsList"));
    if (reply.isValid()) {

        QList<KbdLayout> lst = reply.value();
        layouts.clear();

        QListIterator<KbdLayout> itr(lst);

        while (itr.hasNext()) {
            QString layout_name = itr.next().shortName();
            layouts << layout_name;
        }
    }
}

void X11Keyboard::processKeyPress(unsigned int keyCode)
{
    groupTimer->stop();
    sendKey(keyCode);
    Q_EMIT keyProcessComplete(keyCode);
    groupTimer->start();
}

void X11Keyboard::sendKey(unsigned int keycode)
{
    Window currentFocus;
    int revertTo;

    XGetInputFocus(m_display, &currentFocus, &revertTo);

    QListIterator<VButton *> itr(modKeys);
    while (itr.hasNext()) {
        VButton *mod = itr.next();
        if (mod->isChecked()) {
            XTestFakeKeyEvent(m_display, mod->getKeyCode(), true, 2);
        }
    }

    XTestFakeKeyEvent(m_display, keycode, true, 2);
    XTestFakeKeyEvent(m_display, keycode, false, 2);

    itr.toFront();
    while (itr.hasNext()) {
        VButton *mod = itr.next();
        if (mod->isChecked()) {
            XTestFakeKeyEvent(m_display, mod->getKeyCode(), false, 2);
        }
    }
    XFlush(m_display);
}

bool X11Keyboard::queryModKeyState(KeySym iKey)
{
    int          iKeyMask = 0;
    Window       wDummy1, wDummy2;
    int          iDummy3, iDummy4, iDummy5, iDummy6;
    unsigned int iMask;

    XModifierKeymap *map = XGetModifierMapping(m_display);
    KeyCode keyCode = XKeysymToKeycode(m_display, iKey);
    if (keyCode == NoSymbol) return false;
    for (int i = 0; i < 8; ++i) {
        if (map->modifiermap[map->max_keypermod * i] == keyCode) {
            iKeyMask = 1 << i;
        }
    }
    XQueryPointer(m_display, DefaultRootWindow(m_display), &wDummy1, &wDummy2, &iDummy3, &iDummy4, &iDummy5, &iDummy6, &iMask);
    XFreeModifiermap(map);
    return ((iMask & iKeyMask) != 0);
}

void X11Keyboard::queryModState()
{
    bool curr_caps_state = this->queryModKeyState(XK_Caps_Lock);
    bool curr_num_state = this->queryModKeyState(XK_Num_Lock);

    // TODO: find a way to query these modifiers.
    // XK_ISO_Level3_Shift, XK_Mode_switch, XK_Alt_R
    bool curr_shift_level3_state = this->queryModKeyState(XK_Alt_R);

    bool caps_state = groupState.value(QLatin1String("capslock"));
    bool num_state = groupState.value(QLatin1String("numlock"));
    bool shift_level3_state = groupState.value(QLatin1String("shitlevel3"));

    groupState.insert(QLatin1String("capslock"), curr_caps_state);
    groupState.insert(QLatin1String("numlock"), curr_num_state);
    groupState.insert(QLatin1String("shitlevel3"), curr_shift_level3_state);

    if ((curr_caps_state != caps_state)
            || (curr_num_state != num_state)
            || (curr_shift_level3_state != shift_level3_state)) {
        Q_EMIT groupStateChanged(groupState);
    }
}

void X11Keyboard::layoutChanged()
{
    QDBusInterface iface(QLatin1String("org.kde.keyboard"), QLatin1String("/Layouts"), QLatin1String("org.kde.KeyboardLayouts"), QDBusConnection::sessionBus());

    QDBusReply<uint> reply = iface.call(QLatin1String("getLayout"));

    if (reply.isValid()) {
        layout_index = (int) reply.value();
        Q_EMIT layoutUpdated(layout_index, layouts.at(layout_index));
    } else {
        layout_index = 0;
        Q_EMIT layoutUpdated(0, QLatin1String("us"));
    }
}

void X11Keyboard::textForKeyCode(unsigned int keyCode,  ButtonText &text)
{
    if (keyCode == 0) {
        text.clear();
        return;
    }

    KeyCode button_code = keyCode;

    // layout_index cycles around the first four layouts on X11 (Plasma keyboard kcm can define more layouts)
    KeySym normal = XkbKeycodeToKeysym(m_display, button_code, layout_index, 0);

    KeySym shift  = XkbKeycodeToKeysym(m_display, button_code, layout_index, 1);
    if (shift == NO_KEYSYM_UNICODE_CONVERSION) {
        shift = normal;
    }
    KeySym normal_L3 = XkbKeycodeToKeysym(m_display, button_code, layout_index, 2);
    if (normal_L3 == NO_KEYSYM_UNICODE_CONVERSION) {
        normal_L3 = normal;
    }

    KeySym shift_L3  = XkbKeycodeToKeysym(m_display, button_code, layout_index, 3);
    if (shift_L3 == NO_KEYSYM_UNICODE_CONVERSION) {
        shift_L3 = (normal_L3 == normal) ? shift : normal_L3;
    }

    // int XkbTranslateKeySym (Display *dpy, KeySym *sym_inout, unsigned int mods, char *buf, int nbytes, int *extra_rtrn);
    char buffer[5]; // max size so far: n = 3
    unsigned int mods = 0;
    int extra_rtrn;

    KeySym xkb = normal;
    XkbTranslateKeySym(m_display, &xkb, mods, buffer, 5, &extra_rtrn);
    QString normalText = QString(buffer);

    xkb = shift;
    XkbTranslateKeySym(m_display, &xkb, mods, buffer, 5, &extra_rtrn);
    QString shiftText = QString(buffer);

    xkb = normal_L3;
    XkbTranslateKeySym(m_display, &xkb, mods, buffer, 5, &extra_rtrn);
    QString normalText_L3 = QString(buffer);

    xkb = shift_L3;
    XkbTranslateKeySym(m_display, &xkb, mods, buffer, 5, &extra_rtrn);
    QString shiftText_L3 = QString(buffer);

    text.clear();
    text.append(normalText);
    text.append(shiftText);
    text.append(normalText_L3);
    text.append(shiftText_L3);
}
