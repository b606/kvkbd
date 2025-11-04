#ifndef VBUTTON_H
#define VBUTTON_H

#include <QDomNode>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QRect>
#include <QSize>
#include <QString>
#include <QTimer>
#include "vkeyboard.h"

class VButton : public QPushButton
{
    Q_OBJECT

public:
    explicit VButton(QWidget *parent = nullptr);

    void storeSize();

    void reposition(const QSize &baseSize, const QSize &size);
    QRect VRect();

    unsigned int getKeyCode();
    void setKeyCode(unsigned int keyCode);

    void setButtonText(const ButtonText& text);
    ButtonText buttonText() const;

    void setTextIndex(int index);
    int textIndex();
    void updateText();
    void nextText();
    void setCaps(bool mode);
    void setShift(bool mode);
    void setShiftLevel3(bool mode);

Q_SIGNALS:
    void keyClick(unsigned int);
    void buttonAction(const QString& action);

public Q_SLOTS:
    void sendKey();

protected:
    unsigned int keyCode;
    QRect vpos;

    bool rightClicked;
    QTimer *keyTimer;

    ButtonText mButtonText;
    int mTextIndex;

    bool isCaps;
    bool isShift;
    bool isShiftLevel3;

    static int RepeatShortDelay;
    static int RepeatLongDelay;

protected Q_SLOTS:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void repeatKey();
};

#endif // VBUTTON_H
