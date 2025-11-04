// Class KbdLayout: reply object for the DBus interface org.kde.KeyboardLayouts.getLayoutsList
// SPDX-FileCopyrightText: Copyright (C) 2025 b606 <b606@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "kbdlayout.h"
#include <QDBusArgument>
#include <QDBusMetaType>


KbdLayout::KbdLayout(const QString &shortName, const QString &name,
                     const QString &variantName) : QObject()
{
    setShortName(shortName);
    setName(name);
    setVariantName(variantName);
}

KbdLayout::KbdLayout(const KbdLayout &kbdLayout) : QObject()
{
    setShortName(kbdLayout.shortName());
    setName(kbdLayout.name());
    setVariantName(kbdLayout.variantName());
}

KbdLayout::~KbdLayout()
{}

QString KbdLayout::name() const
{
    return m_name;
}

void KbdLayout::setName(const QString &name)
{
    if (m_name == name) {
        return;
    }

    m_name = name;
}

QString KbdLayout::shortName() const
{
    return m_shortName;
}

void KbdLayout::setShortName(const QString &shortName)
{
    if (m_shortName == shortName) {
        return;
    }

    m_shortName = shortName;
}

QString KbdLayout::variantName() const
{
    return m_variantName;
}

void KbdLayout::setVariantName(const QString &variantName)
{
    if (m_variantName == variantName) {
        return;
    }

    m_variantName = variantName;
}

KbdLayout &KbdLayout::operator=(const KbdLayout &kbdLayout)
{
    if (&kbdLayout != this) {
        setShortName(kbdLayout.shortName());
        setName(kbdLayout.name());
        setVariantName(kbdLayout.variantName());
    }
    return *this;
}

void KbdLayout::registerMetaType()
{
    qRegisterMetaType<KbdLayout>("KbdLayout");
    qRegisterMetaType<QList<KbdLayout>>("QList<KbdLayout>");
    qDBusRegisterMetaType<KbdLayout>();
    qDBusRegisterMetaType<QList<KbdLayout>>();
}


QDBusArgument &operator<<(QDBusArgument &argument, const KbdLayout &kbdLayout)
{
    argument.beginStructure();
    argument << kbdLayout.m_shortName;
    argument << kbdLayout.m_name;
    argument << kbdLayout.m_variantName;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, KbdLayout &kbdLayout)
{
    argument.beginStructure();
    argument >> kbdLayout.m_shortName;
    argument >> kbdLayout.m_name;
    argument >> kbdLayout.m_variantName;
    argument.endStructure();
    return argument;
}

