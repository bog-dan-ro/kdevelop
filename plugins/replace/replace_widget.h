/***************************************************************************
 *   Copyright (C) 2003 by Jens Dagerbo                                    *
 *   jens.dagerbo@swipnet.se                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __REPLACE_WIDGET_H__
#define __REPLACE_WIDGET_H__

#include <ktexteditor/editinterface.h>

#include <QWidget>
#include <QString>
//Added by qt3to4:
#include <QFocusEvent>

class QPushButton;
class Q3ListViewItem;
class QDialog;

class ReplacePart;
class ReplaceDlgImpl;
class ReplaceItem;
class ReplaceView;

class ReplaceWidget : public QWidget
{
    Q_OBJECT

public:
    ReplaceWidget(ReplacePart *part);

public slots:
    void showDialog();
    void stopButtonClicked( KDevPlugin * );
    
protected:
    virtual void focusInEvent(QFocusEvent *e);

private slots:
    void find();
    void replace();
    void clear();
    void editDocument( const QString & ,int );

private:
    bool showReplacements();
    bool makeReplacements();
    bool shouldTerminate();

    QString relativeProjectPath( QString );
    QString fullProjectPath( QString );

    QStringList workFiles();
    QStringList allProjectFiles();
    QStringList subProjectFiles( QString const & );
    QStringList openProjectFiles();

    KTextEditor::EditInterface * getEditInterfaceForFile( QString const & file );

    ReplacePart * m_part;
    ReplaceDlgImpl * m_dialog;

    ReplaceView * _listview;
    QPushButton * _cancel;
    QPushButton * _replace;

    bool _terminateOperation;
};


#endif
