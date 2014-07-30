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
#ifndef CRENDERREQUEST_H
#define CRENDERREQUEST_H

#include <QColor>
#include <QFont>

class CRenderRequest
{
public:
    CRenderRequest();

    bool isValid() const;

    QStringList lines() const;
    void setLines(const QStringList &lines);

    int screenWidth() const;
    void setScreenWidth(int screenWidth);

    int screenHeight() const;
    void setScreenHeight(int screenHeight);

    QColor screenBackgroudColor() const;
    void setScreenBackgroudColor(const QColor &screenBackgroudColor);

    int spaceLeft() const;
    void setSpaceLeft(int spaceLeft);

    int spaceRight() const;
    void setSpaceRight(int spaceRight);

    int spaceBottom() const;
    void setSpaceBottom(int spaceBottom);

    QFont screenTextFont() const;
    void setScreenTextFont(const QFont &screenTextFont);

    QColor screenTextColor() const;
    void setScreenTextColor(const QColor &screenTextColor);

    double framesPerSecond() const;
    void setFramesPerSecond(double framesPerSecond);

    int pixelsPerSecond() const;
    void setPixelsPerSecond(int pixelsPerSecond);

    int repeatCount() const;
    void setRepeatCount(int repeatCount);

    int pixelsBetweenLines() const;
    void setPixelsBetweenLines(int pixelsBetweenLines);

    int pixelsBetweenRepeats() const;
    void setPixelsBetweenRepeats(int pixelsBetweenRepeats);

    bool showGuideLines() const;
    void setShowGuideLines(bool showGuideLines);

    int currentFrame() const;
    void setCurrentFrame(int currentFrame);
private:
    QStringList m_lines;
    int m_screenWidth;
    int m_screenHeight;
    QColor m_screenBackgroudColor;
    int m_spaceLeft;
    int m_spaceRight;
    int m_spaceBottom;
    QFont m_screenTextFont;
    QColor m_screenTextColor;
    double m_framesPerSecond;
    int m_pixelsPerSecond;
    int m_repeatCount;
    int m_pixelsBetweenLines;
    int m_pixelsBetweenRepeats;
    bool m_showGuideLines;
    int m_currentFrame;
};

#endif // CRENDERREQUEST_H
