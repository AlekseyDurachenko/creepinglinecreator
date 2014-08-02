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
#include <QScrollBar>
#include <QProcess>
#include <QTimer>
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
    ui->toolButton_logClear->setDefaultAction(ui->action_logClear);
    ui->toolButton_logSaveAs->setDefaultAction(ui->action_logSaveAs);

    m_imageLabel = new QLabel;
    m_imageLabel->setBackgroundRole(QPalette::Dark);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(true);
    ui->scrollArea_image->setBackgroundRole(QPalette::Dark);
    ui->scrollArea_image->setWidget(m_imageLabel);

    m_testTimer = new QTimer(this);
    connect(m_testTimer, SIGNAL(timeout()), this, SLOT(testTimerTimeout()));

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

    ui->spinBox_screenWidth->setValue(settings.value("CMainWindow/Screen/width", 800).toInt());
    ui->spinBox_screenHeight->setValue(settings.value("CMainWindow/Screen/height", 600).toInt());
    m_screenBackgroundColor = settings.value("CMainWindow/Screen/backgroundColor", QColor(Qt::black).name()).toString();

    ui->spinBox_spaceLeft->setValue(settings.value("CMainWindow/Screen/spacingLeft", 10).toInt());
    ui->spinBox_spaceRight->setValue(settings.value("CMainWindow/Screen/spacingRight", 10).toInt());
    ui->spinBox_spaceBottom->setValue(settings.value("CMainWindow/Screen/spacingBottom", 10).toInt());

    ui->fontComboBox_screenTextFont->setCurrentFont(qvariant_cast<QFont>(settings.value("CMainWindow/Screen/textFont", QFont())));
    ui->spinBox_screenTextPixelSize->setValue(settings.value("CMainWindow/Screen/textPixelSize", 20).toInt());
    ui->checkBox_screenTextBold->setChecked(settings.value("CMainWindow/Screen/textBold", false).toBool());
    ui->checkBox_screenTextItalic->setChecked(settings.value("CMainWindow/Screen/textItalic", false).toBool());
    m_screenTextColor = settings.value("CMainWindow/Screen/textColor", QColor(Qt::green).name()).toString();

    ui->comboBox_framesPerSecond->setCurrentIndex
            (ui->comboBox_framesPerSecond->findText
                (settings.value("CMainWindow/Screen/framePerSecond", "60").toString()));
    ui->spinBox_pixelsPerSecond->setValue(settings.value("CMainWindow/Screen/pixelPerSecond", 150).toInt());
    ui->spinBox_repeatCount->setValue(settings.value("CMainWindow/Screen/repeatCount", 1).toInt());
    ui->spinBox_pixelsBetweenLines->setValue(settings.value("CMainWindow/Screen/pixelsBetweenLines", 300).toInt());
    ui->spinBox_pixelsBetweenRepeats->setValue(settings.value("CMainWindow/Screen/pixelsBetweenRepeats", 800).toInt());
    ui->checkBox_showGuideLine->setChecked(settings.value("CMainWindow/Screen/showGuideLines", false).toBool());

    ui->lineEdit_renderOutput->setText(settings.value("CMainWindow/Render/outputDirectory", "").toString());
    ui->lineEdit_renderFileName->setText(settings.value("CMainWindow/Render/fileName", "").toString());
    ui->lineEdit_renderAvconvArgsUser->setText(settings.value("CMainWindow/Render/avconvArgsUser", "").toString());

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

    settings.setValue("CMainWindow/Screen/width", ui->spinBox_screenWidth->value());
    settings.setValue("CMainWindow/Screen/height", ui->spinBox_screenHeight->value());
    settings.setValue("CMainWindow/Screen/backgroundColor", m_screenBackgroundColor.name());

    settings.setValue("CMainWindow/Screen/spacingLeft", ui->spinBox_spaceLeft->value());
    settings.setValue("CMainWindow/Screen/spacingRight", ui->spinBox_spaceRight->value());
    settings.setValue("CMainWindow/Screen/spacingBottom", ui->spinBox_spaceBottom->value());

    settings.setValue("CMainWindow/Screen/textFont", ui->fontComboBox_screenTextFont->currentFont());
    settings.setValue("CMainWindow/Screen/textPixelSize", ui->spinBox_screenTextPixelSize->value());
    settings.setValue("CMainWindow/Screen/textBold", ui->checkBox_screenTextBold->isChecked());
    settings.setValue("CMainWindow/Screen/textItalic", ui->checkBox_screenTextItalic->isChecked());
    settings.setValue("CMainWindow/Screen/textColor", m_screenTextColor.name());

    settings.setValue("CMainWindow/Screen/framePerSecond", ui->comboBox_framesPerSecond->currentText());
    settings.setValue("CMainWindow/Screen/pixelPerSecond", ui->spinBox_pixelsPerSecond->value());
    settings.setValue("CMainWindow/Screen/repeatCount", ui->spinBox_repeatCount->value());
    settings.setValue("CMainWindow/Screen/pixelsBetweenLines", ui->spinBox_pixelsBetweenLines->value());
    settings.setValue("CMainWindow/Screen/pixelsBetweenRepeats", ui->spinBox_pixelsBetweenRepeats->value());
    settings.setValue("CMainWindow/Screen/showGuideLines", ui->checkBox_showGuideLine->isChecked());

    settings.setValue("CMainWindow/Render/outputDirectory", ui->lineEdit_renderOutput->text());
    settings.setValue("CMainWindow/Render/fileName", ui->lineEdit_renderFileName->text());
    settings.setValue("CMainWindow/Render/avconvArgsUser", ui->lineEdit_renderAvconvArgsUser->text());
}

bool CMainWindow::isTextChanged()
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
    m_testTimer->setInterval
            (1000.0 / ui->comboBox_framesPerSecond->currentText().toDouble());

    updateTabScreen();
}

void CMainWindow::updateTextActions()
{
    ui->action_textSave->setEnabled
            (m_lastTextFileName.isEmpty() || isTextChanged());
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
    else if (index == 2) // render tab
    {
        QString text;
        text += QString(" -framerate %1").arg(ui->comboBox_framesPerSecond->currentText());
        text += " -start_number 0";
        text += QString(" -vframes %1").arg(ui->spinBox_frameCount->value()+1);
        ui->lineEdit_renderAvconvArgs->setText(text);
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

    QString dirName = QFileDialog::getExistingDirectory(this, tr("Select dir"),
            QFileInfo(settings.value("LastDirectory").toString()).absolutePath(),
            QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);

    if (dirName.isEmpty())
    {
        return;
    }

    ui->lineEdit_renderOutput->setText(dirName);
    settings.setValue("LastDirectory", dirName);
}

void CMainWindow::on_pushButton_render_clicked()
{
    int frameCount = calcFrameCount();
    CRenderRequest request = toRenderRequest();
    request.setShowGuideLines(false);

    ui->progressBar_render->setMaximum(frameCount + frameCount + 1);
    ui->progressBar_render->setValue(0);
    ui->plainTextEdit_log->insertPlainText(tr("Creating frames"));

    for (int i = 0; i < calcFrameCount(); ++i)
    {
        QString fileName = ui->lineEdit_renderOutput->text() + QDir::separator()
                + QString("video_%1").arg(i, 10, 10, QChar('0')) + ".png";
        //ui->plainTextEdit_log->insertPlainText(tr("Creating frame: ") + fileName + "\n");
        //ui->plainTextEdit_log->verticalScrollBar()->setValue(ui->plainTextEdit_log->verticalScrollBar()->maximum());
        ui->progressBar_render->setValue(i);
        qApp->processEvents();
        request.setCurrentFrame(i);
        QImage image = imageFromRequest(request);        
        image.save(fileName, "PNG");

    }
    ui->plainTextEdit_log->insertPlainText(tr("Converting...") + "\n");
    ui->plainTextEdit_log->verticalScrollBar()->setValue(ui->plainTextEdit_log->verticalScrollBar()->maximum());
    qApp->processEvents();

    QStringList args;
    args << "-framerate" << ui->comboBox_framesPerSecond->currentText();
    args << "-i" << ui->lineEdit_renderOutput->text() + QDir::separator() + "video_%10d.png";
    //text += " -start_number 0";
    //text += QString(" -vframes %1").arg(ui->spinBox_frameCount->value()+1);
    args += ui->lineEdit_renderAvconvArgsUser->text().split(" ");
    args << "-y";
    args.push_back(ui->lineEdit_renderOutput->text() + QDir::separator() + ui->lineEdit_renderFileName->text());

    QProcess process(this);
    qDebug() << "-------";
    qDebug() << "parguments: " << args.join(" ");
    qDebug() << "process state: " << process.execute("avconv", args);

    ui->progressBar_render->setValue(frameCount + frameCount);
    qApp->processEvents();

    for (int i = 0; i < calcFrameCount(); ++i)
    {
        QString fileName = ui->lineEdit_renderOutput->text() + QDir::separator()
                + QString("video_%1").arg(i, 10, 10, QChar('0')) + ".png";
        //ui->plainTextEdit_log->insertPlainText(tr("Removing frame: ") + fileName + "\n");
        //ui->plainTextEdit_log->verticalScrollBar()->setValue(ui->plainTextEdit_log->verticalScrollBar()->maximum());
        qApp->processEvents();
        QFile::remove(fileName);
    }
    ui->plainTextEdit_log->insertPlainText(tr("Done!") + "\n");
    ui->plainTextEdit_log->verticalScrollBar()->setValue(ui->plainTextEdit_log->verticalScrollBar()->maximum());
    ui->progressBar_render->setValue(frameCount + frameCount + 1);
}

void CMainWindow::on_pushButton_testStartStop_clicked()
{
    if (m_testTimer->isActive())
    {
        m_testTimer->stop();
        ui->pushButton_testStartStop->setText(tr("Start test"));
    }
    else
    {
        m_testTimer->start();
        ui->pushButton_testStartStop->setText(tr("Stop test"));
    }
}

void CMainWindow::testTimerTimeout()
{
    if (ui->spinBox_currentFrame->value() == ui->spinBox_frameCount->value())
    {
        ui->spinBox_currentFrame->setValue(0);
    }
    ui->spinBox_currentFrame->setValue(ui->spinBox_currentFrame->value() + 1);
}

void CMainWindow::on_toolButton_logClear_clicked()
{
    if (QMessageBox::question(this, tr("Clear log"),
            tr("Clear the log. Are you sure?"),
            QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        ui->plainTextEdit_log->clear();
    }
}

void CMainWindow::on_toolButton_logSaveAs_clicked()
{
    G_SETTINGS_INIT();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save log"),
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
}
