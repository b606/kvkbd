#include <QDebug>
#include "vbutton.h"

#define TIMER_INTERVAL_SHORT 40
#define TIMER_INTERVAL_LONG  200

int VButton::RepeatShortDelay = TIMER_INTERVAL_SHORT;
int VButton::RepeatLongDelay = TIMER_INTERVAL_LONG;


VButton::VButton(QWidget *parent) :
    QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_AlwaysShowToolTips);

    keyCode = 0;
    rightClicked = false;
    mTextIndex = 0;
    isCaps = false;
    isShift = false;
    isShiftLevel3 = false;

    keyTimer = new QTimer(this);

    connect(keyTimer, SIGNAL(timeout()), this, SLOT(repeatKey()));
}

void VButton::storeSize()
{
    vpos = geometry();
}

QRect VButton::VRect()
{
    return vpos;
}

void VButton::setKeyCode(unsigned int keyCode)
{
    this->keyCode = keyCode;
}

unsigned int VButton::getKeyCode()
{
    return this->keyCode;
}

void VButton::setButtonText(const ButtonText &text)
{
    this->mButtonText = text;
}

ButtonText VButton::buttonText() const
{
    return this->mButtonText;
}

void VButton::setTextIndex(int index)
{
    this->mTextIndex = index;
}

int VButton::textIndex()
{
    return this->mTextIndex;
}

void VButton::nextText()
{
    if (mButtonText.count() < 1) return;

    mTextIndex++;
    int textCount = mButtonText.count() - 1;
    if (mTextIndex > textCount) mTextIndex = 0;

    updateText();
}

void VButton::setCaps(bool mode)
{
    if (mButtonText.count() < 1) return;

    isCaps = mode;
}

void VButton::setShift(bool mode)
{
    int textCount = mButtonText.count();
    if (mButtonText.count() < 1) return;
    if (mode) {
        this->mTextIndex += 1;
    } else {
        this->mTextIndex -= 1;
    }
    // unsigned modulo: mTextIndex % textCount
    if (mTextIndex < 0) mTextIndex += textCount;
    if (mTextIndex >= textCount) mTextIndex -= textCount;
    isShift = mode;
}

// mButtonText.at(index): index = 0:normal, 1:shift, 2:altgr, 3:altgr+shift
void VButton::setShiftLevel3(bool mode)
{
    int textCount = mButtonText.count();
    if (mButtonText.count() < 3) return;
    if (mode) {
        this->mTextIndex += 2;
    } else {
        this->mTextIndex -= 2;
    }
    if (mTextIndex < 0) mTextIndex += textCount;
    if (mTextIndex >= textCount) mTextIndex -= textCount;
    assert((mTextIndex >= 0) && (mTextIndex < textCount));
    isShiftLevel3 = mode;
}

void VButton::updateText()
{
    if (mButtonText.count() < 1)return;

    QString text = mButtonText.at(this->mTextIndex);
    if (text == QLatin1Char('&')) {
        text += QLatin1Char('&');
    }

    bool doCaps = isCaps ;
    if (isShift) doCaps = !doCaps;

    // TODO: bug for keycode 51 (french).
    // The button displays 'M' instead of 'µ' (greek micron) when capslock
    // but X11Keyboard sends the correct char 'µ'.
    if (doCaps) {
        text = text.toUpper();
    } else {
        text = text.toLower();
    }
    this->setText(text);
}

QString VButton::toString()
{
    QString mstr = QLatin1String("{keyCode:");
    QString code = QLatin1String("0x%1(%2), ");
    mstr.append(code.arg(keyCode, 2, 16, QChar(u'0')).arg(keyCode));
    QString mods = QLatin1String("mods(caps,shift,alt-gr): %1 %2 %3, ");
    mstr.append(mods.arg(isCaps, 0, 2).arg(isShift, 0, 2).arg(isShiftLevel3, 0, 2));
    mstr.append(QLatin1String("chars: ( "));
    for (int i = 0; i < mButtonText.count(); i++) mstr.append(QLatin1String("%1 ").arg(mButtonText.at(i)));
    mstr.append(QLatin1String(")}"));
    return mstr;
}

void VButton::sendKey()
{
    // qDebug() << this->toString();

    Q_EMIT keyClick(this->keyCode);
}

void VButton::mousePressEvent(QMouseEvent *e)
{
    QPushButton::mousePressEvent(e);
    rightClicked = false;
    if (e->button() == Qt::RightButton) {
        rightClicked = true;
    }

    if (this->keyCode > 0) {
        sendKey();

        if (!isCheckable()) {
            if (!keyTimer->isActive()) {
                //200 ms is a bit more that the time needed for a single click
                keyTimer->start(VButton::RepeatLongDelay);
            }
        }
    }
}

void VButton::mouseReleaseEvent(QMouseEvent *e)
{
    if (keyTimer->isActive())keyTimer->stop();
    QPushButton::mouseReleaseEvent(e);
}

void VButton::repeatKey()
{
    //if the user is still pressing the button after 200 ms, we assume
    //he wants the key to be quickly repeated and we decrease the interval
    if (keyTimer->interval() == VButton::RepeatLongDelay) {
        //TODO: make this configurable?
        keyTimer->setInterval(VButton::RepeatShortDelay);
    }

    sendKey();
}
