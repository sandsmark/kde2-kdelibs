#ifndef INCLUDE_MENUITEM_DEF
#define INCLUDE_MENUITEM_DEF
#endif

#include <qmenudata.h>
#include "b2style.h"
#include <kapp.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kdrawutil.h>
#include <qpalette.h>
#include <qbitmap.h>
#include <qtabbar.h>

#include "bitmaps.h"

B2Style::B2Style()
    :KStyle()
{
    KConfig *config = KGlobal::config();
    QString oldGrp = config->group();
    QPalette p = kapp->palette();
}

B2Style::~B2Style()
{
    ;
}

void B2Style::polish(QPalette &)
{
    KConfig *config = KGlobal::config();
    QString oldGrp = config->group();
    config->setGroup("B2");

    // if the config has specific colors for items set use those and don't
    // worry about high color usage, otherwise use KDE standard colors for
    // everything except one color - the slider groove fill. That doesn't
    // really look good with any of the standard colors and one additional
    // color alloc shouldn't kill psudeocolor display users :P
    QColor tmpColor(192, 0, 0);
    if(config->hasKey("SliderColor")){
        tmpColor = config->readColorEntry("SliderColor", &tmpColor);
        sliderGrp.setColor(QColorGroup::Mid, tmpColor);
        sliderGrp.setColor(QColorGroup::Light, tmpColor.light(130));
        sliderGrp.setColor(QColorGroup::Dark, tmpColor.dark(130));
    }
    else{
        sliderGrp.setColor(QColorGroup::Mid, tmpColor);
        sliderGrp.setColor(QColorGroup::Light, QColor(255, 0, 0));
        sliderGrp.setColor(QColorGroup::Dark, QColor(128, 0, 0));
    }
    tmpColor.setRgb(0, 0, 192);
    if(config->hasKey("RadioOnColor")){
        tmpColor = config->readColorEntry("RadioOnColor", &tmpColor);
        radioOnGrp.setColor(QColorGroup::Mid, tmpColor);
        radioOnGrp.setColor(QColorGroup::Light, tmpColor.light(130));
        radioOnGrp.setColor(QColorGroup::Dark, tmpColor.dark(130));
    }
    else{
        radioOnGrp.setColor(QColorGroup::Mid, tmpColor);
        radioOnGrp.setColor(QColorGroup::Light, QColor(0, 0, 255));
        radioOnGrp.setColor(QColorGroup::Dark, QColor(0, 0, 128));
    }
    tmpColor.setRgb(152, 204, 152);
    if(config->hasKey("SliderGrooveColor")){
        tmpColor = config->readColorEntry("SliderGrooveColor", &tmpColor);
        sliderGrooveGrp.setColor(QColorGroup::Button, tmpColor);
        sliderGrooveGrp.setColor(QColorGroup::Light, tmpColor.light(130));
        sliderGrooveGrp.setColor(QColorGroup::Dark, tmpColor.dark(130));
    }
    else{
        sliderGrooveGrp.setColor(QColorGroup::Button, tmpColor);
        sliderGrooveGrp.setColor(QColorGroup::Light, QColor(192, 255, 192));
        sliderGrooveGrp.setColor(QColorGroup::Dark, QColor(0, 128, 0));
    }
    config->setGroup(oldGrp);
}

void B2Style::polish(QWidget *w)
{
    if ( !w->isTopLevel() ) {
        if (w->inherits("QPushButton")
            || w->inherits("QComboBox")
            || w->inherits("QSlider"))
            w->setAutoMask(true);
    }
}
 
void B2Style::unPolish(QWidget *w)
{
    if ( !w->isTopLevel() ) {
        if (w->inherits("QPushButton")
            || w->inherits("QComboBox")
            || w->inherits("QSlider"))
            w->setAutoMask(false);
    }
}                              


void B2Style::drawButton(QPainter *p, int x, int y, int w, int h,
                            const QColorGroup &g, bool sunken,
                            const QBrush *fill)
{
    kDrawBeButton(p, x, y, w, h, g, sunken, fill);
}

void B2Style::drawPushButton(QPushButton *btn, QPainter *p)
{
    QRect r = btn->rect();
    bool sunken = btn->isOn() || btn->isDown();
    QColorGroup g = btn->colorGroup();
    drawButton(p, r.x(), r.y(), r.width(), r.height(), g,
               sunken, sunken ? &g.brush(QColorGroup::Mid) :
               btn->isDefault() ? &g.brush(QColorGroup::Midlight) :
               &g.brush(QColorGroup::Button));
    if(btn->isDefault()){
        p->setPen(Qt::black);
        p->drawRect(r);
    }

}

void B2Style::drawPushButtonLabel(QPushButton *btn, QPainter *p)
{
    int x1, y1, x2, y2;
    btn->rect().coords(&x1, &y1, &x2, &y2);

    bool act = btn->isOn() || btn->isDown();
    if(act){
        ++x1, ++y1;
    }

    if(act || btn->isDefault()){
        QFont font = btn->font();
        font.setBold(true);
        p->setFont(font);
        drawItem(p, x1+1, y1+1, btn->width(), btn->height(),
                 AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
                 btn->pixmap(), btn->text(), -1, act ?
                 &btn->colorGroup().dark() : &btn->colorGroup().mid());

        drawItem(p, x1, y1, btn->width(), btn->height(),
                 AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
                 btn->pixmap(), btn->text(), -1, act ?
                 &btn->colorGroup().light() : &btn->colorGroup().text());
    }
    else{
        drawItem(p, x1+act?1:0, y1+act?1:0, btn->width(), btn->height(),
                 AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
                 btn->pixmap(), btn->text(), -1,
                 act ? &btn->colorGroup().light() : &btn->colorGroup().buttonText());
    }
}

void B2Style::drawButtonMask(QPainter *p, int x, int y, int w, int h)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    p->fillRect(x, y, w, h, QBrush(color1, SolidPattern));
    p->setPen(color0);
    p->drawPoint(x, y);
    p->drawPoint(x2, y);
    p->drawPoint(x, y2);
    p->drawPoint(x2, y2);
}

void B2Style::drawBevelButton(QPainter *p, int x, int y, int w, int h,
                              const QColorGroup &g, bool sunken,
                              const QBrush *fill)
{
    drawButton(p, x, y, w, h, g, sunken, fill);
}

QRect B2Style::buttonRect(int x, int y, int w, int h)
{
    return(QRect(x+4, y+4, w-6, h-6));
}

void B2Style::drawComboButton(QPainter *p, int x, int y, int w, int h,
                                 const QColorGroup &g, bool sunken,
                                 bool, bool, const QBrush *)
{
    int x2 = x+w-1;
    int y2 = y+h-1;

    p->setPen(sunken ? g.dark() : g.light());
    p->drawLine(x, y, x2-2, y);
    p->drawLine(x, y, x, y2-2);
    
    p->setPen(sunken ? g.light() : g.dark());
    p->drawLine(x2-1, y, x2-1, y2-1);
    p->drawLine(x, y2-1, x2-1, y2-1);

    p->setPen(g.mid());
    p->drawLine(x+2, y2, x2, y2);
    p->drawLine(x2, y+2, x2, y2);

    p->fillRect(x+1, y+1, w-3, h-3, g.brush(QColorGroup::Button));

    QPlatinumStyle::drawArrow(p, DownArrow, false, x2-12, y2-12, 6, 6,
                              g, true);
}

void B2Style::drawComboButtonMask(QPainter *p, int x, int y, int w, int h)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    p->fillRect(x, y, w, h, QBrush(color1, SolidPattern));
    p->setPen(color0);
    p->drawPoint(x, y2);
    p->drawPoint(x+1, y2);
    p->drawPoint(x2, y);
    p->drawPoint(x2, y+1);
}

QRect B2Style::comboButtonRect(int x, int y, int w, int h)
{
    return(QRect(x+3, y+3, w-20, h-7));
}

QRect B2Style::comboButtonFocusRect(int x, int y, int w, int h)
{
    return(QRect(x+5, y+5, w-10-16, h-10));
}

void B2Style::drawScrollBarControls(QPainter *p, const QScrollBar *sb,
                                       int sliderStart, uint controls,
                                       uint activeControl)
{
    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );
 
    if (sliderStart > sliderMax) 
        sliderStart = sliderMax;

    bool horiz = sb->orientation() == QScrollBar::Horizontal;
    QColorGroup g = sb->colorGroup();
    QRect addB, subB;
    QRect addPageR, subPageR, sliderR;
    int addX, addY, subX, subY;
    int len = horiz ? sb->width() : sb->height();
    int extent = horiz ? sb->height() : sb->width();
 
    if (horiz) {
        subY = addY = ( extent - buttonDim ) / 2;
        subX = 0;
        addX = len - buttonDim;
    }
    else {
        subX = addX = ( extent - buttonDim ) / 2;
        subY = 0;
        addY = len - buttonDim;
    }
    subB.setRect( subX,subY,buttonDim,buttonDim );
    addB.setRect( addX,addY,buttonDim,buttonDim );
 
    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent;
    if (horiz) {
        subPageR.setRect( subB.right() + 1, 0,
                          sliderStart - subB.right() - 1 , sliderW );
        addPageR.setRect( sliderEnd, 0, addX - sliderEnd, sliderW );
        sliderR .setRect( sliderStart, 0, sliderLength, sliderW );
    }
    else {
        subPageR.setRect( 0, subB.bottom() + 1, sliderW,
                          sliderStart - subB.bottom() - 1 );
        addPageR.setRect( 0, sliderEnd, sliderW, addY - sliderEnd );
        sliderR .setRect( 0, sliderStart, sliderW, sliderLength );
    }
    bool maxed = sb->maxValue() == sb->minValue();
    if ( controls & AddLine ) {
        drawSBButton(p, addB, g);
        drawArrow( p, horiz ? RightArrow : DownArrow,
                   activeControl == AddLine, addB.x()+4, addB.y()+4,
                   addB.width()-8, addB.height()-8, g, !maxed);
    }
    if ( controls & SubLine ) {
        drawSBButton(p, subB, g);
        drawArrow( p, horiz ? LeftArrow : UpArrow,
                   activeControl == SubLine, subB.x()+4, subB.y()+4,
                   subB.width()-8, subB.height()-8, g, !maxed);
    }
    if(controls & AddPage){
        if(addPageR.width()){
            p->fillRect(addPageR, activeControl == AddPage ?
                        g.brush(QColorGroup::Mid) :
                        g.brush(QColorGroup::Button));
            p->setPen(g.mid());
            if(horiz){
                p->drawLine(addPageR.x(), addPageR.y(), addPageR.right(),
                            addPageR.y());
                p->drawLine(addPageR.x(), addPageR.bottom(), addPageR.right(),
                            addPageR.bottom());
                p->setPen(activeControl==AddPage ? g.button() : g.midlight());
                p->drawLine(addPageR.x(), addPageR.y()+1, addPageR.right(),
                            addPageR.y()+1);
            }
            else{
                p->drawLine(addPageR.x(), addPageR.y(), addPageR.x(),
                            addPageR.bottom());
                p->drawLine(addPageR.right(), addPageR.y(), addPageR.right(),
                            addPageR.bottom());
                p->setPen(activeControl==AddPage ? g.button() : g.midlight());
                p->drawLine(addPageR.x()+1, addPageR.y(), addPageR.x()+1,
                            addPageR.bottom());
            }
        }
    }
    if(controls & SubPage){
        if(subPageR.height()){
            p->fillRect(subPageR, activeControl == SubPage ?
                        g.brush(QColorGroup::Mid) :
                        g.brush(QColorGroup::Button));
            p->setPen(g.mid());
            if(horiz){
                p->drawLine(subPageR.x(), subPageR.y(), subPageR.right(),
                            subPageR.y());
                p->drawLine(subPageR.x(), subPageR.bottom(), subPageR.right(),
                            subPageR.bottom());
                p->setPen(activeControl==SubPage ? g.button() : g.midlight());
                p->drawLine(subPageR.x(), subPageR.y()+1, subPageR.right(),
                            subPageR.y()+1);
                p->drawLine(subPageR.x(), subPageR.y()+1, subPageR.x(),
                            subPageR.bottom()-1);
            }
            else{
                p->drawLine(subPageR.x(), subPageR.y(), subPageR.x(),
                            subPageR.bottom());
                p->drawLine(subPageR.right(), subPageR.y(), subPageR.right(),
                            subPageR.bottom());
                p->setPen(activeControl==SubPage ? g.button() : g.midlight());
                p->drawLine(subPageR.x()+1, subPageR.y(), subPageR.x()+1,
                            subPageR.bottom());
                p->drawLine(subPageR.x()+1, subPageR.y(), subPageR.right()-1,
                            subPageR.y());
            }
        }
    }
    if ( controls & Slider ) {
        drawSBButton(p, sliderR, g, activeControl == Slider);
        g.setColor(QColorGroup::Dark, g.mid());
        drawSBDeco(p, sliderR, g, horiz);
    }
}

void B2Style::drawSBButton(QPainter *p, const QRect &r, const QColorGroup &g,
                           bool down)
{
    p->setPen(g.mid());
    p->drawRect(r);
    p->fillRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2,
                    g.brush(QColorGroup::Midlight));

    p->setPen(g.light());
    if(down){
        p->drawLine(r.x()+1, r.bottom()-1, r.right()-1, r.bottom()-1);
        p->drawLine(r.right()-1, r.top()+1, r.right()-1, r.bottom()-1);
    }
    else{
        p->drawLine(r.x()+1, r.y()+1, r.right()-1, r.y()+1);
        p->drawLine(r.x()+1, r.y()+1, r.x()+1, r.bottom()-1);
    }
    
}

void B2Style::drawSBDeco(QPainter *p, const QRect &r, const QColorGroup &g,
                         bool horiz)
{
    if(horiz){
        int y = r.y() + (r.height()-6)/2;
        if(r.width() >= 32){
            int x = r.x() + (r.width()-22)/2;
            qDrawShadePanel(p, x, y, 6, 6, g, false, 1);
            qDrawShadePanel(p, x+8, y, 6, 6, g, false, 1);
            qDrawShadePanel(p, x+16, y, 6, 6, g, false, 1);
        }
        else if(r.width() >= 24 ){
            int x = r.x() + (r.width()-14)/2;
            qDrawShadePanel(p, x, y, 6, 6, g, false, 1);
            qDrawShadePanel(p, x+8, y, 6, 6, g, false, 1);
        }
        else if(r.width() >= 16)
            qDrawShadePanel(p, r.x()+(r.width()-6)/2, y, 6, 6, g, false, 1);
    }
    else{
        int x = r.x() + (r.width()-6)/2;
        if(r.height() >= 32){
            int y = r.y() + (r.height()-22)/2;
            qDrawShadePanel(p, x, y, 6, 6, g, false, 1);
            qDrawShadePanel(p, x, y+8, 6, 6, g, false, 1);
            qDrawShadePanel(p, x, y+16, 6, 6, g, false, 1);
        }
        else if(r.height() >= 24 ){
            int y = r.y() + (r.height()-14)/2;
            qDrawShadePanel(p, x, y, 6, 6, g, false, 1);
            qDrawShadePanel(p, x, y+8, 6, 6, g, false, 1);
        }
        else if(r.height() >= 16)
            qDrawShadePanel(p, x, r.y()+(r.height()-6)/2, 6, 6, g, false, 1);
    }
}


void B2Style::scrollBarMetrics(const QScrollBar *sb, int &sliderMin,
                                  int &sliderMax, int &sliderLength,
                                  int &buttonDim)
{
    QWindowsStyle::scrollBarMetrics(sb, sliderMin, sliderMax, sliderLength,
                                    buttonDim);
}

QStyle::ScrollControl B2Style::scrollBarPointOver(const QScrollBar *sb,
                                                     int sliderStart,
                                                     const QPoint &p)
{
    return(QWindowsStyle::scrollBarPointOver(sb, sliderStart, p));
}

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)


QSize B2Style::exclusiveIndicatorSize() const
{
    return(QSize(13,13));
}

void B2Style::drawExclusiveIndicator(QPainter *p, int x, int y, int w,
                                     int h, const QColorGroup &g, bool on,
                                     bool down, bool)
{
    static QBitmap lightBmp(13, 13, radiooff_light_bits, true);
    static QBitmap grayBmp(13, 13, radiooff_gray_bits, true);
    static QBitmap dgrayBmp(13, 13, radiooff_dgray_bits, true);
    static QBitmap blueBmp(13, 13, radioon_blue_bits, true);
    static QBitmap lblueBmp(13, 13, radioon_lblue_bits, true);
    static QBitmap dblueBmp(13, 13, radioon_dblue_bits, true);

    p->fillRect(x, y, w, h, g.brush(QColorGroup::Background));
    kColorBitmaps(p, g, x, y, &lightBmp, &grayBmp,
                  NULL, &dgrayBmp);

    if(on || down){
        kColorBitmaps(p, radioOnGrp, x, y, &lblueBmp, &blueBmp,
                      NULL, &dblueBmp);
    }
}

QSize B2Style::indicatorSize() const
{
    return(QSize(13, 13));
}

void B2Style::drawIndicator(QPainter *p, int x, int y, int w, int h,
                            const QColorGroup &g, int state, bool down, bool)
{
    int x2 = x+w-1;
    int y2 = y+h-1;

    static unsigned char x_bits[] = {0x63, 0x77, 0x3e, 0x1c, 0x3e, 0x77, 0x63};
    static QBitmap xBmp(7, 7, x_bits, true);
    if(!xBmp.mask())
        xBmp.setMask(xBmp);

    
    p->setPen(g.mid());
    p->drawLine(x, y, x2, y);
    p->drawLine(x, y, x, y2);

    p->setPen(g.light());
    p->drawLine(x2, y+1, x2, y2);
    p->drawLine(x+1, y2, x2, y2);

    p->setPen(g.dark());
    p->drawLine(x+1, y+1, x2-1, y+1);
    p->drawLine(x+1, y+1, x+1, y2-1);

    p->setPen(g.midlight());
    p->drawLine(x2-1, y+2, x2-1, y2-1);
    p->drawLine(x+2, y2-1, x2-1, y2-1);

    p->fillRect(x+2, y+2, w-4, h-4, down ? g.brush(QColorGroup::Button) :
                g.brush(QColorGroup::Light));

    if(state != QButton::Off){
        p->setPen(g.highlight());
        p->drawPixmap(3, 3, xBmp);
    }
    
}

void B2Style::drawSliderGrooveMask(QPainter *p, int x, int y, int w, int h,
                                   QCOORD, Orientation)
{
    static QBrush fillBrush(color1, SolidPattern);
    int x2 = x+w-1;
    int y2 = y+h-1;
    p->fillRect(x, y, w, h, fillBrush);
    p->setPen(color0);
    p->drawPoint(x, y);
    p->drawPoint(x2, y);
    p->drawPoint(x, y2);
    p->drawPoint(x2, y2);
    
}

void B2Style::drawSliderGroove(QPainter *p, int x, int y, int w, int h,
                               const QColorGroup &, QCOORD, Orientation)
{
    p->setPen(Qt::black);
    p->drawRect(x, y, w, h);
    qDrawShadeRect(p, x+1, y+1, w-2, h-2, sliderGrooveGrp, true, 1, 0,
                   &sliderGrooveGrp.brush(QColorGroup::Button));
    int pos;
    for(pos = x+8; pos < x+w-9; pos +=8){
        p->setPen(sliderGrooveGrp.light());
        p->drawLine(pos, y+4, pos, y+h-5);
        p->setPen(sliderGrooveGrp.dark());
        p->drawLine(pos+1, y+4, pos+1, y+h-5);
    }
}

int B2Style::sliderLength() const
{
    return(11);
}

void B2Style::drawSlider(QPainter *p, int x, int y, int, int h,
                         const QColorGroup &, Orientation,
                         bool, bool)
{
    static QBitmap blackBmp(9, 15, slider_black_bits, true);
    static QBitmap darkBmp(9, 15, slider_dark_bits, true);
    static QBitmap fillBmp(9, 15, slider_fill_bits, true);
    static QBitmap lightBmp(9, 15, slider_light_bits, true);

    int dy = (h-15)/2;
    kColorBitmaps(p, sliderGrp, x+1, y+dy, &lightBmp, &fillBmp,
                  NULL, &darkBmp, &blackBmp);
}

void B2Style::drawSliderMask(QPainter *p, int x, int y, int, int h,
                             Orientation, bool, bool)
{
    static QBitmap sliderMaskBmp(9, 15, slider_mask_bits, true);
    if(!sliderMaskBmp.mask())
        sliderMaskBmp.setMask(sliderMaskBmp);
    int dy = (h-15)/2;
    p->setPen(Qt::color1);
    p->drawPixmap(x+1, y+dy, sliderMaskBmp);
}

void B2Style::drawArrow(QPainter *p, Qt::ArrowType type, bool down, int x,
                            int y, int w, int h, const QColorGroup &g,
                            bool enabled, const QBrush *)
{
    qDrawArrow(p, type, Qt::MotifStyle, down, x, y, w, h, g, enabled);
}

void B2Style::drawKBarHandle(QPainter *p, int x, int y, int w, int h,
                                const QColorGroup &g, bool, QBrush *)
{
    qDrawShadePanel(p, x, y, w, h, g, false, 1, &g.brush(QColorGroup::Button));
    if(h > w)
        qDrawArrow(p, RightArrow, WindowsStyle, false, x+1, y+4, w-4, w-4, g,
                   true);
    else
        qDrawArrow(p, DownArrow, WindowsStyle, false, x+4, y+1, h-4, h-4, g,
                   true);
        
}

void B2Style::drawKMenuBar(QPainter *p, int x, int y, int w, int h,
                           const QColorGroup &g, QBrush *)
{
    qDrawShadePanel(p, x, y, w, h, g, false, 1, &g.brush(QColorGroup::Button));
}

void B2Style::drawKToolBar(QPainter *p, int x, int y, int w, int h,
                           const QColorGroup &g, bool)
{
    drawKMenuBar(p, x, y, w, h, g, NULL);
}

void B2Style::drawKToolBarButton(QPainter *p, int x, int y, int w, int h,
                                    const QColorGroup &g, bool sunken,
                                    bool raised, bool enabled, bool popup,
                                    KToolButtonType icontext,
                                    const QString& btext, const QPixmap *pixmap,
                                    QFont *font)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    int dx, dy;

    p->fillRect(x, y, w, h, g.brush(QColorGroup::Button));
    p->setPen(g.dark());
    p->drawLine(x+1, y+1, x2-2, y+1);
    p->drawLine(x, y+2, x, y2-3);
    p->drawLine(x2-1, y+2, x2-1, y2-3);
    p->drawLine(x+1, y2-2, x2-2, y2-2);

    p->setPen(sunken ? g.mid() : g.light());
    p->drawLine(x+1, y+2, x2-2, y+2);
    p->drawLine(x+1, y+2, x+1, y2-3);
    p->setPen(g.light());
    p->drawLine(x+1, y2-1, x2-2, y2-1);
    p->drawLine(x2, y+2, x2, y2-3);

    p->drawPoint(x2-1, y2-2);
    if(sunken)
        p->fillRect(x+2, y+3, w-4, h-6, g.brush(QColorGroup::Midlight));
    
    if (icontext == Icon){ // icon only
        if (pixmap){
            dx = ( w - pixmap->width() ) / 2;
            dy = ( h - pixmap->height() ) / 2;
            if ( sunken )
            {
                ++dx;
                ++dy;
            }
            p->drawPixmap( x+dx, y+dy, *pixmap );
        }
    }
    else if (icontext == IconTextRight){ // icon and text (if any)
        if (pixmap){
            dx = 1;
            dy = ( h - pixmap->height() ) / 2;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            p->drawPixmap( x+dx, y+dy, *pixmap );
        }
        if (!btext.isNull()){
            int tf = AlignVCenter|AlignLeft;
            if (!enabled)
                p->setPen(g.dark());
            if (pixmap)
                dx= pixmap->width();
            else
                dx= 1;
            dy = 0;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            if (font)
                p->setFont(*font);
            if(raised)
                p->setPen(Qt::blue);
            p->drawText(x+dx, y+dy, w-dx, h, tf, btext);
        }
    }
    else if (icontext == Text){ // only text, even if there is a icon
        if (!btext.isNull()){
            int tf = AlignVCenter|AlignLeft;
            if (!enabled)
                p->setPen(g.dark());
            dx= 1;
            dy= 0;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            if (font)
                p->setFont(*font);
            if(raised)
                p->setPen(Qt::blue);
            p->drawText(x+dx, y+dy, w-dx, h, tf, btext);
        }
    }
    else if (icontext == IconTextBottom){
        if (pixmap){
            dx = (w - pixmap->width()) / 2;
            dy = 1;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            p->drawPixmap( x+dx, y+dy, *pixmap );
        }
        if (!btext.isNull()){
            int tf = AlignBottom|AlignHCenter;
            if (!enabled)
                p->setPen(g.dark());
            dy= pixmap->height();
            dx = 2;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            if (font)
                p->setFont(*font);
            if(raised)
                p->setPen(Qt::blue);
            p->drawText(x, y, w, h-4, tf, btext);
        }
    }
    if (popup){
        if (enabled)
            qDrawArrow (p, Qt::DownArrow, Qt::WindowsStyle, false, w-5, h-5, 0, 0,
                        g, true);
        else
            qDrawArrow (p, Qt::DownArrow, Qt::WindowsStyle, false, w-5, h-5,
                        0, 0, g, false);
    }
}

void B2Style::drawKMenuItem(QPainter *p, int x, int y, int w, int h,
                            const QColorGroup &g, bool active, QMenuItem *mi,
                            QBrush *)
{
    QColor btext = active ? g.highlightedText() : g.text();
    if(active){
        QFont oldFont = p->font();
        QFont newFont = oldFont;
        newFont.setBold(true);
        p->setFont(newFont);
        p->fillRect(x, y, w, h, g.brush(QColorGroup::Highlight));
        QApplication::style().drawItem(p, x, y, w, h,
                                       AlignCenter|ShowPrefix|DontClip|SingleLine,
                                       g, mi->isEnabled(), mi->pixmap(), mi->text(),
                                       -1, &btext );
        p->setFont(oldFont);
    }
    else
        QApplication::style().drawItem(p, x, y, w, h,
                                       AlignCenter|ShowPrefix|DontClip|SingleLine,
                                       g, mi->isEnabled(), mi->pixmap(), mi->text(),
                                       -1, &btext );
}

void B2Style::drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw,
                                     int tab, QMenuItem* mi,
                                     const QPalette& pal, bool act,
                                     bool enabled, int x, int y, int w, int h)
{
static const int motifItemFrame         = 2;
static const int motifItemHMargin       = 3;
static const int motifItemVMargin       = 2;
static const int motifArrowHMargin      = 6;
static const int windowsRightBorder     = 12;
    if(act){
        bool dis = !enabled;
        QColorGroup itemg = dis ? pal.disabled() : pal.active();
        itemg.setColor(QColorGroup::Text, itemg.highlightedText());
        
        if (checkable)
            maxpmw = QMAX( maxpmw, 12 );
        int checkcol = maxpmw;

        QFont newFont = p->font();
        QFont oldFont(newFont);
        newFont.setBold(true);
        p->setFont(newFont);
        
        p->fillRect(x, y, w, h, itemg.highlight());

        if ( mi->iconSet() ) {
            QIconSet::Mode mode = dis? QIconSet::Disabled : QIconSet::Normal;
            if (!dis)
                mode = QIconSet::Active;
            QPixmap pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);
            int pixw = pixmap.width();
            int pixh = pixmap.height();
            QRect cr(x, y, checkcol, h);
            QRect pmr(0, 0, pixw, pixh);
            pmr.moveCenter( cr.center() );
            p->setPen(itemg.highlightedText());
            p->drawPixmap(pmr.topLeft(), pixmap );

        }
        else if(checkable) {
            int mw = checkcol + motifItemFrame;
            int mh = h - 2*motifItemFrame;
            if (mi->isChecked()){
                drawCheckMark( p, x + motifItemFrame,
                               y+motifItemFrame, mw, mh, itemg, act, dis );
            }
        }
        p->setPen(itemg.highlightedText());
        QColor discol;
        if (dis) {
            discol = itemg.text();
            p->setPen(discol);
        }
        int xm = motifItemFrame + checkcol + motifItemHMargin;
        QString s = mi->text();
        if (!s.isNull()) {
            int t = s.find( '\t' );
            int m = motifItemVMargin;
            const int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
            if (t >= 0) {
                p->drawText(x+w-tab-windowsRightBorder-motifItemHMargin-motifItemFrame,
                            y+m, tab, h-2*m, text_flags, s.mid( t+1 ));
            }
            p->drawText(x+xm, y+m, w-xm-tab+1, h-2*m, text_flags, s, t);
        } else if (mi->pixmap()) {
            QPixmap *pixmap = mi->pixmap();
            if (pixmap->depth() == 1)
                p->setBackgroundMode(OpaqueMode);
            p->drawPixmap( x+xm, y+motifItemFrame, *pixmap);
            if (pixmap->depth() == 1)
                p->setBackgroundMode(TransparentMode);
        }
        if (mi->popup()) {
            int dim = (h-2*motifItemFrame) / 2;
            if (!dis)
                discol = itemg.text();
            QColorGroup g2(discol, itemg.highlight(),
                           white, white,
                           dis ? discol : white,
                           discol, white);
            drawArrow(p, RightArrow, true,
                      x+w - motifArrowHMargin - motifItemFrame - dim,  y+h/2-dim/2,
                      dim, dim, itemg, TRUE);
        }
        p->setFont(oldFont);
    }
    else
        KStyle::drawPopupMenuItem(p, checkable, maxpmw, tab, mi, pal, act,
                                  enabled, x, y, w, h);
}

void B2Style::drawKProgressBlock(QPainter *p, int x, int y, int w, int h,
                                    const QColorGroup &g, QBrush *)
{
    p->fillRect(x, y, w, h, g.brush(QColorGroup::Text));
}

void B2Style::drawFocusRect(QPainter *p, const QRect &r,
                               const QColorGroup &g, const QColor *,
                               bool atBorder)
{
    p->setPen(g.foreground());
    p->setBrush(NoBrush);
    if ( atBorder )
        p->drawWinFocusRect( QRect( r.x()+1, r.y()+1, r.width()-2, r.height()-2 ) );
    else
        p->drawWinFocusRect( r );
}





