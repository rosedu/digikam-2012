/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2011-03-22
 * @brief  a MediaWiki C++ interface for KDE
 *
 * @author Copyright (C) 2011 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2010 by Alexandre Mendes
 *         <a href="mailto:alex dot mendes1988 at gmail dot com">alex dot mendes1988 at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "mainwindow.moc"

// Qt includes

#include <QDebug>

// Local includes

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mediawiki(QUrl("http://test.wikipedia.org/w/api.php"))
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Load page
void MainWindow::on_pushButton2_clicked()
{
    QueryRevision* const queryrevision(new QueryRevision(mediawiki));
    queryrevision->setPageName(this->ui->mPageEdit->text());
    queryrevision->setProperties(QueryRevision::Content);
    queryrevision->setExpandTemplates(true);
    queryrevision->setLimit(1);

    connect(queryrevision, SIGNAL(revision(const QList<Revision>&)),
            this, SLOT(revisionHandle(const QList<Revision>&)));

    connect(queryrevision, SIGNAL(result(KJob* )),
            this, SLOT(revisionError(KJob*)));

    queryrevision->start();
}

void MainWindow::revisionHandle(const QList<Revision>& revisions)
{
    if(revisions.isEmpty())
    {
        QMessageBox popup;
        popup.setText("This page doesn't exist.");
        popup.exec();
        return;
    }
    this->ui->plainTextEdit->setPlainText(revisions[0].content());
}

//Send page
void MainWindow::on_pushButton1_clicked()
{
    Login* login = new Login(mediawiki, this->ui->mLoginEdit->text(), this->ui->mMdpEdit->text());
    connect(login, SIGNAL(result(KJob* )),
            this, SLOT(loginHandle(KJob*)));
    login->start();
}

void MainWindow::loginHandle(KJob* login)
{
    if(login->error()!= 0)
    {
        QMessageBox popup;
        popup.setText("Wrong authentication.");
        popup.exec();
    }
    else
    {
        Edit* job = new Edit( mediawiki,NULL);
        job->setPageName(this->ui->mPageEdit->text());
        job->setText(this->ui->plainTextEdit->toPlainText());
        connect(job, SIGNAL(result(KJob *)),
                this, SLOT(editError(KJob*)));
        job->start();
    }
}

void MainWindow::editError(KJob* job)
{
    QString errorMessage;
    if(job->error() == 0) errorMessage = "The Wiki page modified successfully.";
    else errorMessage = "The Wiki page can't be modified.";
    QMessageBox popup;
    popup.setText(errorMessage);
    popup.exec();
}

void MainWindow::revisionError(KJob* job)
{
    if(job->error() != 0)
    {
        QMessageBox popup;
        popup.setText("The Wiki page can't be loaded.");
        popup.exec();
    }
}

void MainWindow::on_mPageEdit_textChanged(QString text)
{
    this->ui->pushButton2->setEnabled(!text.isEmpty() && !text.isNull());
}

void MainWindow::on_plainTextEdit_textChanged()
{
    QString text = this->ui->plainTextEdit->toPlainText();
    this->ui->pushButton1->setEnabled(!text.isEmpty() && !text.isNull());
}