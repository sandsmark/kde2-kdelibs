/* vi: ts=8 sts=4 sw=4
 *
 * $Id$
 *
 * This file is part of the KDE project, module kdecore.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 * with minor additions and based on ideas from
 * Torsten Rahn <torsten@kde.org>                                                                 
 *
 * This is free software; it comes under the GNU Library General 
 * Public License, version 2. See the file "COPYING.LIB" for the 
 * exact licensing terms.
 */

#ifndef __KIconEffect_h_Included__
#define __KIconEffect_h_Included__

#include <qimage.h>
#include <qpixmap.h>
#include <qcolor.h>

class KIconEffectPrivate;

/**
 * Apply effects to icons.
 */
class KIconEffect
{
public:
    KIconEffect();
    ~KIconEffect();

    /**
     * Reread configuration.
     */
    void init();

    enum Effects { NoEffect, ToGray, Colorize, DeSaturate, LastEffect };

    /**
     * Apply an effect to an image. The effect to apply depends on the
     * @em group and @em state parameters, and is configured by the user.
     * @param src The image.
     * @param group The group for the icon.
     * @param state The icon's state.
     * @return An image with the effect applied.
     */
    QImage apply(QImage src, int group, int state);

    /**
     * Apply an effect to an image.
     * @param src The image.
     * @param effect The effect to apply.
     * @param value Strength of the effect. 0 <= @em value <= 1.
     * @param trans Add Transparency if trans = true.                             
     * @return An image with the effect applied.
     */
    QImage apply(QImage src, int effect, float value, const QColor rgb, bool trans);

    /**
     * Apply an effect to a pixmap.
     */
    QPixmap apply(QPixmap src, int group, int state);

    /**
     * Apply an effect to a pixmap.
     */
    QPixmap apply(QPixmap src, int effect, float value, const QColor rgb, bool trans);

    /**
     * Returns an image twice as large, consisting of 2x2 pixels.
     */
    QImage doublePixels(QImage src);

private:
    void toGray(QImage &image, float value);
    void colorize(QImage &image, const QColor &col, float value);
    void deSaturate(QImage &image, float value);
    void semiTransparent(QImage &image);
    void semiTransparent(QPixmap &pixmap);

    int mEffect[6][3];
    float mValue[6][3];
    QColor mColor[6][3];
    bool mTrans[6][3];
    KIconEffectPrivate *d;
};

#endif
