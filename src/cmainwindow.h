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
#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

namespace Ui {
class CMainWindow;
}

class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit CMainWindow(QWidget *parent = 0);
    virtual ~CMainWindow();
private slots:
    void on_action_About_triggered();
    void on_action_AboutQt_triggered();
    void on_action_Quit_triggered();
    void on_action_textNew_triggered();
    void on_action_textOpen_triggered();
    void on_action_textSave_triggered();
    void on_action_textSaveAs_triggered();
    void on_plainTextEdit_text_textChanged();
    void on_tabWidget_currentChanged(int index);
    void on_pushButton_screenBackgroundColor_clicked();
    void on_pushButton_screenTextColor_clicked();
protected:
    void closeEvent(QCloseEvent *closeEvent);
private:
    void loadSettings();
    void saveSettings();
    bool textChanged();
    void updateTextActions();
    void openTextFile(const QString &fileName);
private:
    Ui::CMainWindow *ui;
    QLabel *m_imageLabel;
    QString m_lastTextFileName;
    QString m_lastText;
    QColor m_screenBackgroundColor;
    QColor m_screenTextColor;
};

#endif // CMAINWINDOW_H
