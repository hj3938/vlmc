/*****************************************************************************
 * ClipProperty.cpp: Handle the clip properties and meta tags edition
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include <QtDebug>
#include <QTime>
#include <QPushButton>
#include <QStringListModel>
#include <QInputDialog>

#include "ClipProperty.h"
#include "ui_ClipProperty.h"

ClipProperty::ClipProperty( Clip* clip, QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::ClipProperty ),
    m_clip( clip )
{
    QTime   duration;
    duration = duration.addSecs( m_clip->getLengthSecond() );

    ui->setupUi(this);
    connect( this, SIGNAL( accepted() ), this, SLOT( deleteLater() ) );
    connect( this, SIGNAL( rejected() ), this, SLOT( deleteLater() ) );
    ui->durationValueLabel->setText( duration.toString( "hh:mm:ss" ) );
    ui->nameValueLabel->setText( m_clip->getParent()->getFileInfo()->fileName() );
    ui->resolutionValueLabel->setText( QString::number( m_clip->getParent()->getWidth() )
                                       + " x " + QString::number( m_clip->getParent()->getHeight() ) );
    ui->fpsValueLabel->setText( QString::number( m_clip->getParent()->getFps() ) );
    ui->snapshotLabel->setPixmap( m_clip->getParent()->getSnapshot().scaled( 128, 128, Qt::KeepAspectRatio ) );
    setWindowTitle( m_clip->getParent()->getFileInfo()->fileName() + " " + tr( "properties" ) );

    const QPushButton* button = ui->buttonBox->button( QDialogButtonBox::Apply );
    Q_ASSERT( button != NULL);
    connect( button, SIGNAL( clicked() ), this, SLOT( apply() ) );

    QStringListModel*   model = new QStringListModel( m_clip->getMetaTags(), this );
    ui->metaTagsView->setModel( model );

    connect( ui->addTagsButton, SIGNAL( clicked() ), this, SLOT( addTagsRequired() ) );
    connect( ui->deleteTagsButton, SIGNAL( clicked() ), this, SLOT( removeTagsRequired() ) );
}

ClipProperty::~ClipProperty()
{
    delete ui;
}

void ClipProperty::changeEvent( QEvent *e )
{
    QDialog::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void    ClipProperty::apply()
{
    QStringListModel* model = dynamic_cast<QStringListModel*>( ui->metaTagsView->model() );
    if ( model == NULL )
        return ;
    m_clip->setMetaTags( model->stringList() );
}

void    ClipProperty::addTagsRequired()
{
    bool                ok;
    QString             newTags = QInputDialog::getText( this, tr( "New tags edition" ),
                                            tr( "Enter tags (you can enter multiple tags, separated by a comma)" ),
                                            QLineEdit::Normal, "", &ok );
    if ( ok == true && newTags.length() > 0 )
    {
        QStringListModel*   model = dynamic_cast<QStringListModel*>( ui->metaTagsView->model() );
        if ( model == NULL )
            return ;
        QStringList         list = model->stringList();
        QStringList         toAdd = newTags.split( ",", QString::SkipEmptyParts );
        list.append( toAdd );
        model->setStringList( list );
    }
}

void    ClipProperty::removeTagsRequired()
{
    QStringListModel*   model = dynamic_cast<QStringListModel*>( ui->metaTagsView->model() );
    if ( model == NULL )
        return ;
    QItemSelectionModel*    selected = ui->metaTagsView->selectionModel();
    QModelIndexList         listSelected = selected->selectedIndexes();
    QStringList             list = model->stringList();
    while ( listSelected.empty() == false )
    {
        QVariant    elem = model->data( listSelected.first(), Qt::DisplayRole );
        list.removeOne( elem.toString() );
        listSelected.removeFirst();
    }
    model->setStringList( list );
}