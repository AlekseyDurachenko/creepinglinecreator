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
#include "cmainwindow.h"
#include "ui_cmainwindow.h"
#include "global.h"
#include "imagerender.h"
#include "caboutdialog.h"
#include <QtCore/qmath.h>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QCloseEvent>
#include <QProcess>
#include <QDebug>

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::CMainWindow)
{
    ui->setupUi(this);
    setWindowTitle(G_SHORT_PROGRAM_NAME());

    ui->toolButton_textNew->setDefaultAction(ui->action_textNew);
    ui->toolButton_textOpen->setDefaultAction(ui->action_textOpen);
    ui->toolButton_textSave->setDefaultAction(ui->action_textSave);
    ui->toolButton_textSaveAs->setDefaultAction(ui->action_textSaveAs);

    m_imageLabel = new QLabel;
    m_imageLabel->setBackgroundRole(QPalette::Dark);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(true);
    ui->scrollArea_image->setBackgroundRole(QPalette::Dark);
    ui->scrollArea_image->setWidget(m_imageLabel);

    connect(ui->spinBox_screenWidth, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreen()));
    connect(ui->spinBox_screenHeight, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreen()));
    connect(ui->spinBox_spaceLeft, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreen()));
    connect(ui->spinBox_spaceRight, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreen()));
    connect(ui->spinBox_spaceBottom, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreen()));
    connect(ui->checkBox_showGuideLine, SIGNAL(stateChanged(int)),
            this, SLOT(updateTabScreen()));
    connect(ui->horizontalSlider_currentFrame, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreen()));
    connect(ui->fontComboBox_screenTextFont, SIGNAL(currentFontChanged(QFont)),
            this, SLOT(updateTabScreenWithFrameCount()));
    connect(ui->spinBox_screenTextPixelSize, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreenWithFrameCount()));
    connect(ui->checkBox_screenTextBold, SIGNAL(stateChanged(int)),
            this, SLOT(updateTabScreenWithFrameCount()));
    connect(ui->checkBox_screenTextItalic, SIGNAL(stateChanged(int)),
            this, SLOT(updateTabScreenWithFrameCount()));
    connect(ui->comboBox_framesPerSecond, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateTabScreenWithFrameCount()));
    connect(ui->spinBox_pixelsPerSecond, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreenWithFrameCount()));
    connect(ui->spinBox_repeatCount, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreenWithFrameCount()));
    connect(ui->spinBox_pixelsBetweenLines, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreenWithFrameCount()));
    connect(ui->spinBox_pixelsBetweenRepeats, SIGNAL(valueChanged(int)),
            this, SLOT(updateTabScreenWithFrameCount()));

    loadSettings();
}

CMainWindow::~CMainWindow()
{
    saveSettings();

    delete ui;
}

void CMainWindow::loadSettings()
{
    G_SETTINGS_INIT();
    restoreGeometry(settings.value("CMainWindow/geometry", geometry()).toByteArray());
    restoreState(settings.value("CMainWindow/state", saveState()).toByteArray());
    m_screenBackgroundColor = settings.value("ScreenBackgroundColor", QColor(Qt::black).name()).toString();
    m_screenTextColor = settings.value("ScreenTextColor", QColor(Qt::green).name()).toString();

    QPalette pal(palette());
    pal.setColor(QPalette::Button, m_screenBackgroundColor);
    ui->pushButton_screenBackgroundColor->setPalette(pal);
    pal.setColor(QPalette::Button, m_screenTextColor);
    ui->pushButton_screenTextColor->setPalette(pal);

    updateTextActions();
}

void CMainWindow::saveSettings()
{
    G_SETTINGS_INIT();
    settings.setValue("CMainWindow/geometry", saveGeometry());
    settings.setValue("CMainWindow/state", saveState());
    settings.setValue("ScreenBackgroundColor", m_screenBackgroundColor.name());
    settings.setValue("ScreenTextColor", m_screenTextColor.name());
}

bool CMainWindow::textChanged()
{
    return (ui->plainTextEdit_text->toPlainText() != m_lastText);
}

int CMainWindow::calcFrameCount()
{
    // 1. calculate the line count
    QStringList lines = toLineList();
    // 2. calculate total pixel count
    int textWidth = 0;
    QFont font = ui->fontComboBox_screenTextFont->currentFont();
    font.setPixelSize(ui->spinBox_screenTextPixelSize->value());
    font.setBold(ui->checkBox_screenTextBold->isChecked());
    font.setItalic(ui->checkBox_screenTextItalic->isChecked());
    for (int i = 0; i < lines.count(); ++i)
    {
        textWidth += QFontMetrics(font).width(lines.at(i));
        if (i < lines.count()-1)
        {
            textWidth += ui->spinBox_pixelsBetweenLines->value();
        }
    }
    textWidth *= ui->spinBox_repeatCount->value();
    if (textWidth > 0)
    {
        textWidth += ui->spinBox_pixelsBetweenRepeats->value() * (ui->spinBox_repeatCount->value() - 1);
    }
    // 3. calculate the frame count
    double pps = ui->spinBox_pixelsPerSecond->value();
    double fps = ui->comboBox_framesPerSecond->currentText().toDouble();
    return qCeil((textWidth / pps) * fps);
}

QStringList CMainWindow::toLineList()
{
    QString rawText = ui->plainTextEdit_text->toPlainText();
    QStringList lines = rawText.split("\n");
    for (int i = lines.count()-1; i >= 0; --i)
    {
        lines[i] = lines.at(i).trimmed();
        if (lines.at(i).isEmpty())
        {
            lines.removeAt(i);
        }
    }

    return lines;
}

CRenderRequest CMainWindow::toRenderRequest()
{
    CRenderRequest request;
    request.setLines(toLineList());
    request.setScreenWidth(ui->spinBox_screenWidth->value());
    request.setScreenHeight(ui->spinBox_screenHeight->value());
    request.setScreenBackgroudColor(m_screenBackgroundColor);
    request.setSpaceLeft(ui->spinBox_spaceLeft->value());
    request.setSpaceRight(ui->spinBox_spaceRight->value());
    request.setSpaceBottom(ui->spinBox_spaceBottom->value());
    QFont font = ui->fontComboBox_screenTextFont->currentFont();
    font.setPixelSize(ui->spinBox_screenTextPixelSize->value());
    font.setBold(ui->checkBox_screenTextBold->isChecked());
    font.setItalic(ui->checkBox_screenTextItalic->isChecked());
    request.setScreenTextFont(font);
    request.setScreenTextColor(m_screenTextColor);
    request.setFramesPerSecond(ui->comboBox_framesPerSecond->currentText().toDouble());
    request.setPixelsPerSecond(ui->spinBox_pixelsPerSecond->value());
    request.setRepeatCount(ui->spinBox_repeatCount->value());
    request.setPixelsBetweenLines(ui->spinBox_pixelsBetweenLines->value());
    request.setPixelsBetweenRepeats(ui->spinBox_pixelsBetweenRepeats->value());
    request.setShowGuideLines(ui->checkBox_showGuideLine->isChecked());
    request.setCurrentFrame(ui->horizontalSlider_currentFrame->value());

    return request;
}

void CMainWindow::updateTabScreen()
{
    m_imageLabel->setPixmap
            (QPixmap::fromImage(imageFromRequest(toRenderRequest())));
    ui->scrollArea_image->setWidgetResizable(false);
    m_imageLabel->adjustSize();
}

void CMainWindow::updateTabScreenWithFrameCount()
{
    // recalculate the frame count and current frame position
    int frameCount = calcFrameCount();
    frameCount = (frameCount == 0) ? (0) : (frameCount-1);
    ui->horizontalSlider_currentFrame->blockSignals(true); // hack
    ui->spinBox_currentFrame->setMaximum(frameCount);
    ui->horizontalSlider_currentFrame->setMaximum(frameCount);
    ui->spinBox_frameCount->setMaximum(frameCount);
    ui->spinBox_frameCount->setValue(frameCount);
    ui->horizontalSlider_currentFrame->blockSignals(false); // hack
    // hack - for exclude the double render query

    updateTabScreen();
}

void CMainWindow::updateTextActions()
{
    ui->action_textSave->setEnabled
            (m_lastTextFileName.isEmpty() || textChanged());
}

void CMainWindow::openTextFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Warning"), file.errorString());
        return;
    }

    ui->plainTextEdit_text->setPlainText(QString::fromUtf8(file.readAll()));

    m_lastTextFileName = fileName;
    m_lastText = ui->plainTextEdit_text->toPlainText();
    updateTextActions();
}

void CMainWindow::on_action_About_triggered()
{
    CAboutDialog dlg(this);
    dlg.exec();
}

void CMainWindow::on_action_AboutQt_triggered()
{
    qApp->aboutQt();
}

void CMainWindow::on_action_Quit_triggered()
{
    close();
}

void CMainWindow::closeEvent(QCloseEvent *closeEvent)
{
#ifndef APP_DEPLOY
    Q_UNUSED(closeEvent);
#else
    if (QMessageBox::question(this, tr("Quit"), tr("Are you sure?"),
            QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        closeEvent->accept();
    }
    else
    {
        closeEvent->ignore();
    }
#endif
}

void CMainWindow::on_action_textNew_triggered()
{
    if (QMessageBox::question(this, tr("New text"),
            tr("Create new text. Are you sure?"),
            QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        m_lastTextFileName.clear();
        m_lastText.clear();
        ui->plainTextEdit_text->clear();
        updateTextActions();
    }
}

void CMainWindow::on_action_textOpen_triggered()
{
    G_SETTINGS_INIT();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open text file"),
        settings.value("LastDirectory").toString(), tr("All (*.txt)"));

    if (fileName.isEmpty())
    {
        return;
    }
    settings.setValue("LastDirectory", QFileInfo(fileName).absolutePath());

    openTextFile(fileName);
}

void CMainWindow::on_action_textSave_triggered()
{
    G_SETTINGS_INIT();

    QString fileName = m_lastTextFileName;
    if (fileName.isEmpty())
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Open text"),
            settings.value("LastDirectory").toString(), tr("Texts (*.txt)"));

        if (fileName.isEmpty())
        {
            return;
        }
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr("Critical"), file.errorString());
        return;
    }

    QByteArray ba = ui->plainTextEdit_text->toPlainText().toUtf8();
    if (file.write(ba) != ba.size())
    {
        QMessageBox::critical(this, tr("Critical"), file.errorString());
    }

    settings.setValue("LastDirectory", QFileInfo(fileName).absolutePath());
    m_lastTextFileName = fileName;
    m_lastText = ui->plainTextEdit_text->toPlainText();
    updateTextActions();
}

void CMainWindow::on_action_textSaveAs_triggered()
{
    G_SETTINGS_INIT();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Open text"),
        settings.value("LastDirectory").toString(), tr("Texts (*.txt)"));

    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr("Critical"), file.errorString());
        return;
    }

    QByteArray ba = ui->plainTextEdit_text->toPlainText().toUtf8();
    if (file.write(ba) != ba.size())
    {
        QMessageBox::critical(this, tr("Critical"), file.errorString());
        return;
    }

    settings.setValue("LastDirectory", QFileInfo(fileName).absolutePath());
    m_lastTextFileName = fileName;
    m_lastText = ui->plainTextEdit_text->toPlainText();
    updateTextActions();
}

void CMainWindow::on_plainTextEdit_text_textChanged()
{
    updateTextActions();
}

void CMainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 1) // user switched to the screen tab
    {
        updateTabScreenWithFrameCount();
    }
}

void CMainWindow::on_pushButton_screenBackgroundColor_clicked()
{
    QColor color = QColorDialog::getColor(m_screenBackgroundColor, this);
    if (color.isValid() && color != m_screenBackgroundColor)
    {
        m_screenBackgroundColor = color;

        QPalette pal(palette());
        pal.setColor(QPalette::Button, m_screenBackgroundColor);
        ui->pushButton_screenBackgroundColor->setPalette(pal);

        updateTabScreen();
    }
}

void CMainWindow::on_pushButton_screenTextColor_clicked()
{
    QColor color = QColorDialog::getColor(m_screenTextColor, this);
    if (color.isValid() && color != m_screenTextColor)
    {
        m_screenTextColor = color;

        QPalette pal(palette());
        pal.setColor(QPalette::Button, m_screenTextColor);
        ui->pushButton_screenTextColor->setPalette(pal);

        updateTabScreen();
    }
}

void CMainWindow::on_toolButton_setRenderOutput_clicked()
{
    G_SETTINGS_INIT();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Output video"),
        settings.value("LastDirectory").toString(), tr("Video (* *.*)"));

    if (fileName.isEmpty())
    {
        return;
    }

    ui->lineEdit_renderOutput->setText(fileName);
}

void CMainWindow::on_pushButton_render_clicked()
{
    CRenderRequest request = toRenderRequest();
    for (int i = 0; i < calcFrameCount(); ++i)
    {
        request.setCurrentFrame(i);
        QImage image = imageFromRequest(request);
        QString fileName = ui->lineEdit_renderOutput->text() + QString("_%1").arg(i, 10, 10, QChar('0')) + ".png";
        qDebug() << fileName;
        qDebug() << image.save(fileName, "PNG");

    }
    QStringList args;
    args << "-framerate" << ui->comboBox_framesPerSecond->currentText() << "-i" << ui->lineEdit_renderOutput->text() + "_%10d.png"
         << "-vcodec" << "libx264" << ui->lineEdit_renderOutput->text() + ".mp4";

    QProcess process(this);
    qDebug() << process.execute("avconv", args);

    for (int i = 0; i < calcFrameCount(); ++i)
    {
        QString fileName = ui->lineEdit_renderOutput->text() + QString("_%1").arg(i, 10, 10, QChar('0')) + ".png";
        QFile::remove(fileName);
    }
}
