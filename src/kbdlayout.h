// Class KbdLayout: reply object for the DBus interface org.kde.KeyboardLayouts.getLayoutsList
// SPDX-FileCopyrightText: Copyright (C) 2025 b606 <b606@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KBDLAYOUT_H
#define KBDLAYOUT_H

#include <QObject>
#include <QDBusArgument>

/**
 * Class KbdLayout:
 * Reply objects for the DBus interface org.kde.KeyboardLayouts.
 * A call to the method "getLayoutsList" returns an array of three strings
 * (signature: "a(sss)") which will be cast to a QList<KbdLayout>.
 */
class KbdLayout : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString shortName READ shortName WRITE setShortName)
    Q_PROPERTY(QString variantName READ variantName WRITE setVariantName)

public:
    /**
     * Default constructor
     */
    KbdLayout();

    explicit KbdLayout(const QString& shortName, const QString& name,
              const QString& variantName);

    /**
     * Copy constructor
     */
    KbdLayout(const KbdLayout& kbdLayout);
    
    /**
     * Destructor
     */
    ~KbdLayout();

    /**
     * @return the name
     */
    QString name() const;

    /**
     * @return the shortName
     */
    QString shortName() const;

    /**
     * @return the variantName
     */
    QString variantName() const;
    
    /**
     * Copy operator= override.
     * 
     * @return the KbdLayout& .
     */
    KbdLayout& operator=(const KbdLayout& kbdLayout);

    /**
     * Marshalling operator<< to push a KbdLayout into a QDBusArgument.
     * 
     * @return the modified QDBusArgument& .
     */
    friend QDBusArgument &operator<<(QDBusArgument &argument, const KbdLayout &kbdLayout); 

    /**
     * Marshalling operator>> to load a KbdLayout from a QDBusArgument.
     * 
     * @return the const QDBusArgument&.
     */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, KbdLayout &kbdLayout);

    static void registerMetaType();

public Q_SLOTS:
    /**
     * Sets the name.
     *
     * @param name the new name
     */
    void setName(const QString& name);

    /**
     * Sets the shortName.
     *
     * @param shortName the new shortName
     */
    void setShortName(const QString& shortName);

    /**
     * Sets the variantName.
     *
     * @param variantName the new variantName
     */
    void setVariantName(const QString& variantName);

private:
    QString m_shortName;
    QString m_name;
    QString m_variantName;
};

Q_DECLARE_METATYPE(KbdLayout);
Q_DECLARE_METATYPE(QList<KbdLayout>);

#endif // KBDLAYOUT_H
