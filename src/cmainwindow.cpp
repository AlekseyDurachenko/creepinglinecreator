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
    updateTextActions();
}

void CMainWindow::saveSettings()
{
    G_SETTINGS_INIT();
    settings.setValue("CMainWindow/geometry", saveGeometry());
    settings.setValue("CMainWindow/state", saveState());
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
