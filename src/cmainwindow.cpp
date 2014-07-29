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
#include "caboutdialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QCloseEvent>
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

// this is example of the resize
//    m_imageLabel->setPixmap(QPixmap::fromImage(QImage(100, 300, QImage::Format_RGB32)));
//    ui->scrollArea_image->setWidgetResizable(false);
//    m_imageLabel->adjustSize();

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
    // TODO: update the screen
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
    }
}
