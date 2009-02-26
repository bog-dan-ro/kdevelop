/*
 * KDevelop C++ Code Completion Support
 *
 * Copyright 2006-2007 Hamish Rodda <rodda@kde.org>
 * Copyright 2007-2008 David Nolden <david.nolden.kdevelop@art-master.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "model.h"

#include <QIcon>
#include <QMetaType>
#include <QTextFormat>
#include <QBrush>
#include <QDir>
#include <kdebug.h>
#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <kiconloader.h>


#include "../cppduchain/cppduchain.h"
#include "../cppduchain/typeutils.h"

#include "../cppduchain/overloadresolutionhelper.h"

#include <language/duchain/declaration.h>
#include "../cppduchain/cpptypes.h"
#include "../cppduchain/typeutils.h"
#include <language/duchain/classfunctiondeclaration.h>
#include <language/duchain/ducontext.h>
#include <language/duchain/duchain.h>
#include <language/duchain/namespacealiasdeclaration.h>
#include <language/duchain/parsingenvironment.h>
#include <language/editor/editorintegrator.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/duchainbase.h>
#include <language/duchain/topducontext.h>
#include <language/duchain/dumpchain.h>
#include <language/codecompletion/codecompletioncontext.h>
#include "../cppduchain/navigation/navigationwidget.h"
#include "../preprocessjob.h"
#include <language/duchain/duchainutils.h>
#include "worker.h"
#include "../cpplanguagesupport.h"
#include <language/editor/modificationrevision.h>
#include <language/duchain/specializationstore.h>
#include "implementationhelperitem.h"

using namespace KTextEditor;
using namespace KDevelop;
using namespace TypeUtils;

namespace Cpp {

CodeCompletionModel::CodeCompletionModel( QObject * parent )
  : KDevelop::CodeCompletionModel(parent)
{
}

#if KDE_IS_VERSION(4,2,62)
KTextEditor::CodeCompletionModelControllerInterface2::MatchReaction CodeCompletionModel::matchingItem(const QModelIndex& matched) {
  KSharedPtr<CompletionTreeElement> element = itemForIndex(matched);
  //Do not hide the completion-list if the matched item is an implementation-helper
  if(dynamic_cast<ImplementationHelperItem*>(element.data()))
    return KTextEditor::CodeCompletionModelControllerInterface2::None;
  else
    return CodeCompletionModelControllerInterface2::matchingItem(matched);
}
#endif

bool CodeCompletionModel::shouldStartCompletion(KTextEditor::View* view, const QString& inserted, bool userInsertion, const KTextEditor::Cursor& position) {
  kDebug() << inserted;
  QString insertedTrimmed = inserted.trimmed();
  if(insertedTrimmed.endsWith('\"'))
    return false; //Never start completion behind a string literal
  if(insertedTrimmed.endsWith( '(' ) || insertedTrimmed.endsWith(',') || insertedTrimmed.endsWith('<') || insertedTrimmed.endsWith(":") )
    return true;
  
  return CodeCompletionModelControllerInterface::shouldStartCompletion(view, inserted, userInsertion, position);
}

void CodeCompletionModel::aborted(KTextEditor::View* view) {
    kDebug() << "aborting";
    worker()->abortCurrentCompletion();
    
    KTextEditor::CodeCompletionModelControllerInterface::aborted(view);
}

bool CodeCompletionModel::shouldAbortCompletion(KTextEditor::View* view, const KTextEditor::SmartRange& range, const QString& currentCompletion) {
  bool ret = CodeCompletionModelControllerInterface::shouldAbortCompletion(view, range, currentCompletion);
  
  return ret;
}

KDevelop::CodeCompletionWorker* CodeCompletionModel::createCompletionWorker() {
  return new CodeCompletionWorker(this);
}

CodeCompletionModel::~CodeCompletionModel()
{
}

void CodeCompletionModel::updateCompletionRange(KTextEditor::View* view, KTextEditor::SmartRange& range) {
  if(completionContext()) {
    Cpp::CodeCompletionContext* cppContext = dynamic_cast<Cpp::CodeCompletionContext*>(completionContext().data());
    Q_ASSERT(cppContext);
    cppContext->setFollowingText(range.text().join("\n"));
    bool didReset = false;
    if(completionContext()->ungroupedElements().size()) {
      //Update the ungrouped elements, since they may have changed their text
      int row = rowCount() - completionContext()->ungroupedElements().size();
      
      foreach(KDevelop::CompletionTreeElementPointer item, completionContext()->ungroupedElements()) {
        
        QModelIndex parent = index(row, 0);
        
        KDevelop::CompletionCustomGroupNode* group = dynamic_cast<KDevelop::CompletionCustomGroupNode*>(item.data());
        if(group) {
          int subRow = 0;
          foreach(KDevelop::CompletionTreeElementPointer item, group->children) {
            if(item->asItem() && item->asItem()->dataChangedWithInput()) {
//               dataChanged(index(subRow, Name, parent), index(subRow, Name, parent));
              kDebug() << "doing dataChanged";
              reset(); ///@todo This is very expensive, but kate doesn't listen for dataChanged(..). Find a cheaper way to achieve this.
              didReset = true;
              break;
            }
            ++subRow;
          }
        }
        
        if(didReset)
          break;
        
        if(item->asItem() && item->asItem()->dataChangedWithInput()) {
          reset();
          didReset = true;
          break;
        }
        ++row;
      }
//       dataChanged(index(rowCount() - completionContext()->ungroupedElements().size(), 0), index(rowCount()-1, columnCount()-1 ));
    }
  }
  KTextEditor::CodeCompletionModelControllerInterface::updateCompletionRange(view, range);
}

void CodeCompletionModel::foundDeclarations(QList<KSharedPtr<KDevelop::CompletionTreeElement> > item, KSharedPtr<KDevelop::CodeCompletionContext> completionContext) {
  //Set the static match-context, in case the argument-hints are not shown
  
  setStaticMatchContext(QList<IndexedType>());
  
  if(completionContext) {
    Cpp::CodeCompletionContext* argumentFunctions = dynamic_cast<Cpp::CodeCompletionContext*>(completionContext->parentContext());
    if(argumentFunctions) {
      QList<IndexedType> types;
      bool abort = false;
      foreach(CompletionTreeItemPointer item, argumentFunctions->completionItems(SimpleCursor::invalid(), abort, false))
        types += item->typeForArgumentMatching();
      
      setStaticMatchContext(types);
    }
  }
  KDevelop::CodeCompletionModel::foundDeclarations(item, completionContext);
}

}

#include "model.moc"