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
#include "crenderrequest.h"

CRenderRequest::CRenderRequest()
{
    m_lines = QStringList();
    m_screenWidth = -1;
    m_screenHeight = -1;
    m_screenBackgroudColor = QColor();
    m_spaceLeft = -1;
    m_spaceRight = -1;
    m_spaceBottom = -1;
    m_screenTextFont = QFont();
    m_screenTextColor = QColor();
    m_framesPerSecond = -1;
    m_pixelsPerSecond = -1;
    m_repeatCount = -1;
    m_pixelsBetweenLines = -1;
    m_pixelsBetweenRepeats = -1;
    m_showGuideLines = false;
}

bool CRenderRequest::isValid() const
{
    return (m_screenWidth > 0 && m_screenHeight > 0
            && m_spaceLeft >= 0 && m_spaceRight >= 0 && m_spaceBottom >= 0
            && m_framesPerSecond > 0 && m_pixelsPerSecond > 0
            && m_repeatCount > 0 && m_pixelsBetweenLines > 0
            && m_pixelsBetweenRepeats > 0);
}

QStringList CRenderRequest::lines() const
{
    return m_lines;
}

void CRenderRequest::setLines(const QStringList &lines)
{
    m_lines = lines;
}

int CRenderRequest::screenWidth() const
{
    return m_screenWidth;
}

void CRenderRequest::setScreenWidth(int screenWidth)
{
    m_screenWidth = screenWidth;
}

int CRenderRequest::screenHeight() const
{
    return m_screenHeight;
}

void CRenderRequest::setScreenHeight(int screenHeight)
{
    m_screenHeight = screenHeight;
}

QColor CRenderRequest::screenBackgroudColor() const
{
    return m_screenBackgroudColor;
}

void CRenderRequest::setScreenBackgroudColor(const QColor &screenBackgroudColor)
{
    m_screenBackgroudColor = screenBackgroudColor;
}

int CRenderRequest::spaceLeft() const
{
    return m_spaceLeft;
}

void CRenderRequest::setSpaceLeft(int spaceLeft)
{
    m_spaceLeft = spaceLeft;
}

int CRenderRequest::spaceRight() const
{
    return m_spaceRight;
}

void CRenderRequest::setSpaceRight(int spaceRight)
{
    m_spaceRight = spaceRight;
}

int CRenderRequest::spaceBottom() const
{
    return m_spaceBottom;
}

void CRenderRequest::setSpaceBottom(int spaceBottom)
{
    m_spaceBottom = spaceBottom;
}

QFont CRenderRequest::screenTextFont() const
{
    return m_screenTextFont;
}

void CRenderRequest::setScreenTextFont(const QFont &screenTextFont)
{
    m_screenTextFont = screenTextFont;
}

QColor CRenderRequest::screenTextColor() const
{
    return m_screenTextColor;
}

void CRenderRequest::setScreenTextColor(const QColor &screenTextColor)
{
    m_screenTextColor = screenTextColor;
}

double CRenderRequest::framesPerSecond() const
{
    return m_framesPerSecond;
}

void CRenderRequest::setFramesPerSecond(double framesPerSecond)
{
    m_framesPerSecond = framesPerSecond;
}

int CRenderRequest::pixelsPerSecond() const
{
    return m_pixelsPerSecond;
}

void CRenderRequest::setPixelsPerSecond(int pixelsPerSecond)
{
    m_pixelsPerSecond = pixelsPerSecond;
}

int CRenderRequest::repeatCount() const
{
    return m_repeatCount;
}

void CRenderRequest::setRepeatCount(int repeatCount)
{
    m_repeatCount = repeatCount;
}

int CRenderRequest::pixelsBetweenLines() const
{
    return m_pixelsBetweenLines;
}

void CRenderRequest::setPixelsBetweenLines(int pixelsBetweenLines)
{
    m_pixelsBetweenLines = pixelsBetweenLines;
}

int CRenderRequest::pixelsBetweenRepeats() const
{
    return m_pixelsBetweenRepeats;
}

void CRenderRequest::setPixelsBetweenRepeats(int pixelsBetweenRepeats)
{
    m_pixelsBetweenRepeats = pixelsBetweenRepeats;
}

bool CRenderRequest::showGuideLines() const
{
    return m_showGuideLines;
}

void CRenderRequest::setShowGuideLines(bool showGuideLines)
{
    m_showGuideLines = showGuideLines;
}

int CRenderRequest::currentFrame() const
{
    return m_currentFrame;
}

void CRenderRequest::setCurrentFrame(int currentFrame)
{
    m_currentFrame = currentFrame;
}
