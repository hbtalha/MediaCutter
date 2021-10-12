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

#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTimer>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    slider = new RangeSlider(Qt::Horizontal);
    openButton = new QPushButton("Open");
    openButton->setMaximumSize(50, 28);
    cutPushButton = new QPushButton("Cut");
    cutPushButton->setMaximumSize(50, 28);

    QLabel* separatorLabel = new QLabel("-");

    setDefaultLabels();

    QHBoxLayout* audioInfoLayout = new QHBoxLayout(this);
    audioInfoLayout->addWidget(&audioNameLabel);
    audioInfoLayout->addStretch();
    audioInfoLayout->addWidget(&audioDurationLabel);

    QHBoxLayout* durationRangeLayout = new QHBoxLayout(this);
    durationRangeLayout->addStretch();
    durationRangeLayout->addWidget(&fromDurationLabel);
    durationRangeLayout->addWidget(separatorLabel);
    durationRangeLayout->addWidget(&toDurationLabel);
    durationRangeLayout->addStretch();
    durationRangeLayout->addSpacing(5);

    QHBoxLayout* bottomLayout = new QHBoxLayout(this);
    bottomLayout->addWidget(cutPushButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(&notificationLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(openButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(audioInfoLayout);
    mainLayout->addWidget(slider);
    mainLayout->addLayout(durationRangeLayout);
    mainLayout->addLayout(bottomLayout);

    QWidget* mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);

    this->setCentralWidget(mainWidget);

    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    this->setFixedHeight(107);
    this->resize(480, this->height());

    connect(openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connectCutButton();
    connect(slider, &RangeSlider::maximumValueChanged, this, [this](int duration)
    {
        toDurationLabel.setText(formattedTime(duration));
    });
    connect(slider, &RangeSlider::minimumValueChanged, this, [this](int duration)
    {
        fromDurationLabel.setText(formattedTime(duration));
    });

    slider->setEnabled(false);
    cutting = false;
    cancelled = false;
}

void MainWindow::openFile()
{
    QFileDialog dialog(this, "Select file");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Media Files(*.3g2 *.3gp *.3gp2 *.3gpp *.amv *.asf *.avi *.bik *.bin *.divx *.drc *.dv "
                            "*.f4v *.flv *.gvi *.gxf *.iso *.m1v *.m2v *.m2t *.m2ts *.m4v *.mkv *.mov *.mp2 *.mp2V "
                            "*.mp4 *.mp4v *.mpe *.mpeg *.mpeg1 *.mpeg2 *.mpeg4 *.mpg *.mpv2 *.mts *.mtv *.mxf *.mxg "
                            "*.nsv *.nuv *.ogg *.ogm *.ogv *.ogx *.ps *.rec *.rm *.rmvb *.rpl *.thp *.tod *.ts *.tts "
                            "*.txd *.vob *.vro *.webm *.wm *.wmv *.wtv *.xesc *.3ga *.669 *.152 *.aac *.ac3 *.adt "
                            "*.adts *.aif *.aiff *.amr *.aob *.ape *.awb *.caf *.dts *.flac *.it *.kar *.m4a *.m4b "
                            "*.m4p *.m5p *.mid *.mka *.mlp *.mod *.mpa *.mp1 *.mp2 *.mp3 *.mpc *.mpga *.mus *.oga "
                            "*.ogg *oma *.opus *.qcp *.ra *.rmi *.s3m *.sid *.spx *.thd *.tta *.voc *vqf *.w64 *.wav "
                            "*.wma *.wv *.xa *.xm);;Video Files(*.3g2 *.3gp *.3gp2 *.3gpp *.amv *.asf *.avi *.bik "
                            "*.bin *.divx *.drc *.dv *.f4v *.flv *.gvi *.gxf *.iso *.m1v *.m2v *.m2t *.m2ts *.m4v "
                            "*.mkv *.mov *.mp2 *.mp2V *.mp4 *.mp4v *.mpe *.mpeg *.mpeg1 *.mpeg2 *.mpeg4 *.mpg *.mpv2 "
                            "*.mts *.mtv *.mxf *.mxg *.nsv *.nuv *.ogg *.ogm *.ogv *.ogx *.ps *.rec *.rm *.rmvb *.rpl "
                            "*.thp *.tod *.ts *.tts *.txd *.vob *.vro *.webm *.wm *.wmv *.wtv *.xesc);;"
                            "Audio Files(*.3ga *.669 *.152 *.aac *.ac3 *.adt *.adts *.aif *.aiff *.amr *.aob *.ape "
                            "*.awb *.caf *.dts *.flac *.it *.kar *.m4a *.m4b *.m4p *.m5p *.mid *.mka *.mlp *.mod *.mpa "
                            "*.mp1 *.mp2 *.mp3 *.mpc *.mpga *.mus *.oga *.ogg *oma *.opus *.qcp *.ra *.rmi *.s3m *.sid "
                            "*.spx *.thd *.tta *.voc *vqf *.w64 *.wav *.wma *.wv *.xa *.xm)"
                           ));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(settings.value("last_open_folder", QStandardPaths::writableLocation(QStandardPaths::MusicLocation)).toString());

    if(dialog.exec())
    {
        settings.setValue("last_open_folder", dialog.directoryUrl().toLocalFile());
        fileInfo.setFile(dialog.selectedUrls().at(0).toLocalFile());

        audioNameLabel.setText(fileInfo.fileName());
        notificationLabel.setText("Loading...");
        getFileDuration();
    }
}

void MainWindow::cutFile()
{
    QString outFile = fileInfo.absoluteFilePath().remove( "." + fileInfo.suffix()) + "_cut." + fileInfo.suffix();
    int newDuration = slider->maximumValue() - slider->minimumValue();

    bool ok = true;
    QString errorMessage;

    if(QFile::exists(outFile))
    {
        errorMessage = "Output file alerady exists";
        ok = false;
    }

    if(newDuration <= 0)
    {
        errorMessage = "Invalid duration";
        ok = false;
    }

    if( ! ok)
    {
        notificationLabel.setText(errorMessage);
        QTimer::singleShot(3000, this, [this] { notificationLabel.clear(); });
        return;
    }

    QStringList args;
    args << "-ss" << QString::number(slider->minimumValue()) << "-t" << QString::number(newDuration)
         << "-i" << fileInfo.absoluteFilePath() << "-c:a" << "copy"
         << outFile;

    QProcess* process = new QProcess;
    process->setProcessChannelMode(QProcess::MergedChannels);

    connect(process, &QProcess::stateChanged, [process, this](QProcess::ProcessState newState)
    {
        if(newState == QProcess::NotRunning)
        {
            if(!cancelled)
            {
                if(process->exitStatus() != 0)
                    notificationLabel.setText("Error");
                else
                    notificationLabel.setText("Finihsed");
            }

            connectCutButton();

            cancelled = false;
            cutPushButton->setText("Cut");
            cutting = false;
            process->deleteLater();
            connectCutButton();
        }
    });

    connect(process, &QProcess::finished, this, [this]
    {
        cutting = false; // extra safety
        openButton->setEnabled(true);
    });
    connect(process, &QProcess::readyRead, this, [process]
    {
        qDebug() << process->readAll();
    });

    process->start("ffmpeg", args);
    cutting = true;
    notificationLabel.setText("Cutting...");
    cutPushButton->setText("Stop");
    openButton->setDisabled(true);

    disconnect(cutPushButton, &QPushButton::clicked, nullptr, nullptr);
    connect(cutPushButton, &QPushButton::clicked, this, [this, process]
    {
        if(process->state() == process->Running && cutting)
        {
            cancelled = true;
            process->kill();
            notificationLabel.setText("Cancelled");
            cutPushButton->setText("Cut");
            connectCutButton();
        }
    });
}

void MainWindow::getFileDuration()
{
    notificationLabel.clear();
    cutPushButton->setDisabled(true);

    QStringList args;
    args << "-i" << fileInfo.absoluteFilePath() << "-show_entries" << "format=duration" << "-v"
         << "quiet" << "-of" << "csv=p=0";

    QProcess* process = new QProcess;
    process->setProcessChannelMode(QProcess::MergedChannels);

    connect(process, &QProcess::stateChanged, [process](QProcess::ProcessState newState)
    {
        if(newState == QProcess::NotRunning)
        {
            process->deleteLater();
        }
    });

    connect(process, &QProcess::finished, this, [this]
    {
        cutPushButton->setEnabled(true);
    });
    connect(process, &QProcess::readyRead, this, [this, process]
    {
        QString output = process->readAllStandardOutput();

        output.remove(QRegularExpression("\\..*\r\n"));

        if( ! output.isEmpty())
        {
            slider->setEnabled(true);
            int duration = output.toInt();
            slider->setMaximum(duration);
            slider->setMaximumValue(duration);
            slider->setMinimumValue(0);
            QString formattedDuration = formattedTime(duration);
            audioDurationLabel.setText(formattedDuration);
            fromDurationLabel.setText("00:00");
            toDurationLabel.setText(formattedDuration);
        }
        else
        {
            notificationLabel.setText("Error");
        }

        cutPushButton->setEnabled(true);
    });

    process->start("ffprobe", args);
}

QString MainWindow::formattedTime(int secs)
{
    QTime time = QTime(0, 0);
    time = time.addSecs(secs);

    QString displayFormat = "mm:ss";
    if (time.hour() > 0)
        displayFormat = "hh:mm:ss";

    return  time.toString(displayFormat);;
}

void MainWindow::setDefaultLabels()
{
    fromDurationLabel.setText("--:--");
    toDurationLabel.setText("--:--");
    audioDurationLabel.setText("--:--");
}

void MainWindow::connectCutButton()
{
    connect(cutPushButton, &QPushButton::clicked, this, [this]
    {
        if(fileInfo.exists())
            cutFile();
    });
}

MainWindow::~MainWindow()
{
}

