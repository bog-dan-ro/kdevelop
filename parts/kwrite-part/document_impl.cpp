#include <qmultilineedit.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtabwidget.h>


#include "kwrite_factory.h"
#include "document_impl.h"
#include "clipboard_iface_impl.h"
#include "undo_iface_impl.h"
#include "cursor_iface_impl.h"


#include "kwrite/kwdoc.h"
#include "kwrite/kwview.h"


using namespace KEditor;


DocumentImpl::DocumentImpl(Editor *parent)
  : Document(parent)
{
  setInstance( KWritePartFactory::instance() );
  
  m_document = new KWriteDoc(HlManager::self(), "");
  m_view = static_cast<KWrite*>(m_document->createView(0, ""));
  setWidget(m_view);

  // register with the view manager
  parent->addView(m_view);
     
  // create interfaces
  new ClipboardIfaceImpl(m_view, this, parent);
  new CursorIfaceImpl(m_view, this, parent);
  new UndoIfaceImpl(m_view, this, parent);

  setXMLFile("kwriteeditor_part.rc", true);
   
  // register with the part manager
  parent->addPart(this);
  
  m_view->setFocus();
}


DocumentImpl::~DocumentImpl()
{
  delete m_document;
}


bool DocumentImpl::load(QString filename)
{
  QFile f(filename);
  if (!f.open(IO_ReadOnly))
	return false;

  QTextStream ts(&f);
  m_document->setText(ts.read());

  f.close();

  rename(filename);
  
  return true;
}


bool DocumentImpl::save(QString filename)
{
  QFile f(filename);
  if (!f.open(IO_WriteOnly))
	return false;

  QTextStream ts(&f);
  ts << m_document->text();

  f.close();

  rename(filename);

  return true;
}


#include "document_impl.moc"
