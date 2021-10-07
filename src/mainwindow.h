/*=========================================================================

  Copyright (C) 2021 Helder Batalha

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QProcess>
#include <QSettings>

#include "rangeslider.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    void openFile();
    void cutFile();
    void getFileDuration();
    QString formattedTime(int secs);
    void setDefaultLabels();
    void connectCutButton();

    RangeSlider* slider;
    QLabel audioNameLabel;
    QLabel audioDurationLabel;
    QLabel fromDurationLabel;
    QLabel toDurationLabel;
    QLabel notificationLabel;
    QPushButton* cutPushButton;
    QPushButton* openButton;

    QFileInfo fileInfo;
    QSettings settings;

    bool cancelled;
    bool cutting;
};
#endif // MAINWINDOW_H
