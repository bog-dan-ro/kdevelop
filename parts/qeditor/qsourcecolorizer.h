/* $Id$
 *
 *  This file is part of Klint
 *  Copyright (C) 2001 Roberto Raggi (raggi@cli.di.unipi.it)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 */

#ifndef qsourcecolorizer_h
#define qsourcecolorizer_h

#include <private/qrichtext_p.h>
#include <qintdict.h>
#include <qlist.h>
#include <qmap.h>
#include <qregexp.h>
#include <qpair.h>


#define DECLARE_FORMAT_ITEM(type, id, f, c)\
{\
    QFont font = f; \
    QColor color = c; \
    font = config->readFontEntry( QString("Font ") + id, &font ); \
    color = config->readColorEntry( QString("Color ") + id, &color ); \
    m_formats.insert( type, qMakePair(QString(id), new QTextFormat(font, color)) ); \
}

#define UPDATE_FORMAT_ITEM(type, f, c)\
    m_formats[ type ].second->setFont( f ); \
    m_formats[ type ].second->setColor( c );

#define STORE_FORMAT_ITEM(type)\
{\
    QString id = m_formats[ type ].first; \
    QTextFormat* fmt = m_formats[ type ].second; \
    config->writeEntry( QString("Font ") + id, fmt->font() ); \
    config->writeEntry( QString("Color ") + id, fmt->color() ); \
}

class QEditor;

class HLItem{
public:
    HLItem( int state=0, int context=0 )
        : m_state( state ), m_context( context ) {}
    virtual ~HLItem() {}

    virtual bool checkStart( const QChar& ) const { return TRUE; }
    virtual bool checkEnd( const QChar& ) const { return TRUE; }

    virtual int attr() const { return m_state; }
    virtual int context() const { return m_context; }

    virtual int checkHL( QTextDocument*, QTextParagraph*, int pos, int*, int* ) =0;

private:
    int m_state;
    int m_context;
};


class IdentifierHLItem: public HLItem{
public:
    IdentifierHLItem( int state, int context )
        : HLItem( state, context ) {}

    bool checkStart( const QChar& ch ) const {
        return ch.isLetter() || ch == '_';
    }

    int checkHL( QTextDocument*, QTextParagraph* p, int pos, int*, int* ){
        while( pos < p->length() ){
            QChar ch = p->string()->at(pos).c;
            if( ch.isLetterOrNumber() || ch == '_' )
                ++pos;
            else
                break;
        }
        return pos;
    }
};

class NumberHLItem: public HLItem{
public:
    NumberHLItem( int state, int context )
        : HLItem( state, context ) {}

    int checkHL( QTextDocument*, QTextParagraph* p, int pos, int*, int* ){
        while( pos < p->length() ){
            if( p->string()->at(pos).c.isNumber() )
                ++pos;
            else
                break;
        }
        return pos;
    }
};

class CommentHLItem: public HLItem{
public:
    CommentHLItem( const QChar ch, int state, int context )
        : HLItem( state, context ), m_char1( ch ) {}

    int checkHL( QTextDocument*, QTextParagraph* p, int pos, int*, int* ){
        QChar ch = p->string()->at( pos ).c;
        if( ch == m_char1 )
            return p->length();
        return pos;
    }

private:
    QChar m_char1;
};


class KeywordsHLItem: public HLItem{
public:
    KeywordsHLItem( const char** keywords, int state, int context )
        : HLItem( state, context ) {
            int i = 1;
            while( *keywords ){
                m_keywords.insert( QString(*keywords++), i++ );
            }
    }

    bool checkStart( const QChar& ch ) const {
        return ch.isLetter() || ch == '_';
    }

    int checkHL( QTextDocument*, QTextParagraph* p, int pos, int*, int* ){
        int save_pos = pos;
        QString word;
        while( pos < p->length() ){
            QChar ch = p->string()->at(pos).c;
            if( ch.isLetterOrNumber() || ch == '_' ){
                word += ch;
                ++pos;
            } else
                break;
        }

        if( m_keywords.contains(word) )
            return pos;
        return save_pos;
    }

private:
    QMap<QString, int> m_keywords;
};

class StringHLItem: public HLItem{
public:
    StringHLItem( const QString& text, int state, int context )
        : HLItem( state, context ), m_text(text) {}

    QString text() const { return m_text; }
    int checkHL( QTextDocument*, QTextParagraph* p, int pos, int*, int* ){
        QString s = p->string()->toString();
        s.truncate( s.length() - 1 );
        if( s.mid( pos, m_text.length() ) == m_text )
            return pos + m_text.length();
        return pos;
    }

private:
    QString m_text;
};

class RegExpHLItem: public HLItem{
public:
    RegExpHLItem( QString pattern, int state, int context )
        : HLItem( state, context ), m_rx( pattern ) {}

    int checkHL( QTextDocument*, QTextParagraph* p, int pos, int*, int* ){
        QString s = p->string()->toString();
        s.truncate( s.length() - 1 );
        int idx = m_rx.search(s, pos);
        if( idx == pos )
            return pos + m_rx.matchedLength();
        return pos;
    }

private:
    QRegExp m_rx;
};


class HLItemCollection: public HLItem{
public:
    HLItemCollection( int state=0, int context=0 ): HLItem( state, context )
       { m_items.setAutoDelete( TRUE ); }

    void appendChild( HLItem* item ) { m_items.append( item ); }


    int checkHL( QTextDocument* doc, QTextParagraph* p, int pos,
                 int* state, int* next){
        QListIterator<HLItem> it( m_items );
        QTextString* s = p->string();
        QChar ch = s->at( pos ).c;

        while( it.current() ){
            HLItem* item = it.current();
            if( item->checkStart( ch ) ){
                int npos = item->checkHL( doc, p, pos, state, next );
                if( npos > pos ){
                    pos = npos;
                    if( state )
                        *state = item->attr();
                    if( next )
                        *next = item->context();
                    break;
                }
            }
            ++it;
        }
        return pos;
    }

private:
    QList<HLItem> m_items;
};

class QSourceColorizer: public QTextPreProcessor{
public:
    enum Type {
	Normal=0,
	PreProcessor,
	Keyword,
        Operator,
	Comment,
	Constant,
	String,

	Custom = 1000
    };

public:
    QSourceColorizer( QEditor* );
    virtual ~QSourceColorizer();

    QEditor* editor() const { return m_editor; }

    void insertHLItem( int, HLItemCollection* );

    void setSymbols( const QString&, const QString& );
    QChar matchFor( const QChar& ) const;
    QString leftSymbols() const { return m_left; }
    QString rightSymbols() const { return m_right; }

    virtual QTextFormat* format( int key ) { return m_formats[ key ].second; }
    virtual QTextFormat* formatFromId( const QString& id );

    QStringList styleList() const;
    virtual void updateStyles( QMap<QString, QPair<QFont, QColor> >& values );
    virtual void process( QTextDocument*, QTextParagraph*, int, bool=FALSE );

protected:
    QEditor* m_editor;
    QMap<int, QPair<QString, QTextFormat*> > m_formats;
    QList<HLItemCollection> m_items;
    QString m_left;
    QString m_right;
};


#endif
