// Copyright 2014, Durachenko Aleksey V. <durachenko.aleksey@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include "imagerender.h"
#include "utils.h"
#include <QPainter>
#include <QDebug>

QImage imageFromRequest(const CRenderRequest &request)
{
    int fontHeight = QFontMetrics(request.screenTextFont()).height();
    int fontAscent = QFontMetrics(request.screenTextFont()).ascent();
    int textBottom = request.screenHeight()-request.spaceBottom();
    int textTop = textBottom - fontHeight;
    int textLeft = request.spaceLeft();
    int textRight = request.screenWidth()-request.spaceRight();

    // create image with text
    QImage textImage = QImage(textRight-textLeft, fontHeight, QImage::Format_ARGB32);
    textImage.fill(request.screenBackgroudColor());
    QPainter textPainter(&textImage);
    textPainter.setPen(request.screenTextColor());
    textPainter.setFont(request.screenTextFont());
    // pixel per frame
    double ppf = request.pixelsPerSecond() / request.framesPerSecond();
    // offset of the first line
    int firstPixel = ppf * request.currentFrame();
    // find && draw the visible lines
    int offset = -firstPixel;
    for (int repeat = 0; repeat < request.repeatCount() && offset < request.screenWidth(); ++repeat)
    {
        for (int line = 0; line < request.lines().count() && offset < request.screenWidth(); ++line)
        {
            QString text = request.lines().at(line);
            int len = QFontMetrics(request.screenTextFont()).width(text);
            if (isInterset(0, request.screenWidth(), offset, offset+len))
            {
                textPainter.drawText(offset, fontAscent, text);
            }
            offset += len;
            offset += request.pixelsBetweenLines();
        }
        offset += request.pixelsBetweenRepeats();
    }

    // create result image
    QImage image = QImage(request.screenWidth(), request.screenHeight(), QImage::Format_ARGB32);
    image.fill(request.screenBackgroudColor());
    QPainter painter(&image);
    painter.drawImage(textLeft, textTop, textImage);

    if (request.showGuideLines())
    {
        painter.setPen(QPen(request.screenTextColor(), 1, Qt::DashLine));
        painter.drawLine(0, textTop, request.screenWidth(), textTop);
        painter.drawLine(0, textBottom, request.screenWidth(), textBottom);
        painter.drawLine(textLeft, 0, textLeft, request.screenHeight());
        painter.drawLine(textRight, 0, textRight, request.screenHeight());
    }

    return image;
}
