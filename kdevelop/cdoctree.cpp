/***************************************************************************
           cdoctree.cpp -
                             -------------------                                         

    begin                : 3 Oct 1998                                        
    copyright            : (C) 1998 by Sandy Meier                         
    email                : smeier@rz.uni-potsdam.de                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/
#include "cdoctree.h"
#include <iostream.h>
#include <kmsgbox.h>
#include "cdoctreepropdlg.h"

CDocTree::CDocTree(QWidget*parent,const char* name,KConfig* config) : KTreeList(parent,name){
  config_kdevelop = config;
  icon_loader = KApplication::getKApplication()->getIconLoader();
  left_button = true;
  right_button = false;
  others_pop = new KPopupMenu();
  others_pop->setTitle("Others:");
  others_pop->insertItem(i18n("Add Entry..."),this,SLOT(slotAddDocumentation()));
  
  doc_pop = new KPopupMenu();
  doc_pop->setTitle("Others:");
  doc_pop->insertItem(i18n("Add Entry..."),this,SLOT(slotAddDocumentation()));
  doc_pop->insertItem(i18n("Remove Entry"),this,SLOT(slotRemoveDocumentation()));
  doc_pop->insertSeparator();
  doc_pop->insertItem(i18n("Properties..."),this,SLOT(slotDocumentationProp()));
  connect(this,SIGNAL(singleSelected(int)),SLOT(slotSingleSelected(int)));

}

CDocTree::~CDocTree(){
}
void CDocTree::mousePressEvent(QMouseEvent* event){
  if(event->button() == RightButton){    
    left_button = false;
    right_button = true;
  }
  if(event->button() == LeftButton){
    left_button = true;
    right_button = false;
  }
  mouse_pos.setX(event->pos().x());
  mouse_pos.setY(event->pos().y());
  KTreeList::mousePressEvent(event); 
}
void CDocTree::refresh(CProject* prj){ 
  project = prj;
  clear();
 
  KPath path;
  QString str;
  QString file;
  QString str_path;
  QString str_sub_path;

  QPixmap folder_pix = icon_loader->loadMiniIcon("folder.xpm");
  QPixmap book_pix = icon_loader->loadMiniIcon("mini-book1.xpm");
  str = "Documentation";
  path.push(&str);   
  insertItem("Documentation",&folder_pix);
  
  
  
  addChildItem("KDevelop",&folder_pix,&path);
  addChildItem("Qt/KDE Libraries",&folder_pix,&path);
  addChildItem("Others",&folder_pix,&path);
  addChildItem("Current Project",&folder_pix,&path);
  

  //  add KDevelop
  str_path = "KDevelop";
  path.push(&str_path);
  addChildItem("Manual",&book_pix,&path);
  addChildItem("Tutorial",&book_pix,&path);
 
  
  //  add the Libraries
  str_path = "Qt/KDE Libraries";
  path.pop();
  path.push(&str_path);
  addChildItem("Qt-Library",&book_pix,&path);
  addChildItem("KDE-Core-Library",&book_pix,&path);
  addChildItem("KDE-UI-Library",&book_pix,&path);
  addChildItem("KDE-KFile-Library",&book_pix,&path);
  addChildItem("KDE-HTMLW-Library",&book_pix,&path);
  addChildItem("KDE-KFM-Library",&book_pix,&path);
  addChildItem("KDE-KAB-Library",&book_pix,&path);
  addChildItem("KDE-KSpell-Library",&book_pix,&path);

  // add the others
  str_path = "Others";
  path.pop();
  path.push(&str_path);

  config_kdevelop->setGroup("Other_Doc_Location");
  QStrList others_list;
  QString others_str;
  config_kdevelop->readListEntry("others_list",others_list);
  for(others_str=others_list.first();others_str !=0;others_str=others_list.next()){
    addChildItem(others_str,&book_pix,&path);
  }
  
  
  // add the Project-Doc
  str_path = "Current Project";
  path.pop();
  path.push(&str_path);
  if(project){
    if(prj->valid){
      addChildItem("User-Manual",&book_pix,&path);
      addChildItem("API-Documentation",&book_pix,&path);  
    }
  }
  
  setExpandLevel(2);
}
void CDocTree::slotSingleSelected(int index){
  if(right_button){
    //cerr << itemAt(index)->getText();
    if(QString(itemAt(index)->getText()) == "Others" ){
      others_pop->popup(this->mapToGlobal(mouse_pos));
     
    }
    else if(QString(itemAt(index)->getParent()->getText()) == "Others"){
      doc_pop->popup(this->mapToGlobal(mouse_pos));
    }
  }
  
}

void CDocTree::slotAddDocumentation(){
  CDocTreePropDlg dlg;
  dlg.setCaption("Add Entry...");
  QStrList others_list;
  int pos;
  if(dlg.exec()){
    config_kdevelop->setGroup("Other_Doc_Location");
    // add the entry to the list
    config_kdevelop->readListEntry("others_list",others_list);

    // find the correct place
    if(QString(getCurrentItem()->getText()) == "Others"){ 
      others_list.insert(0,dlg.name_edit->text());
    }
    else{
      pos = others_list.find(QString(getCurrentItem()->getText()));
      others_list.insert(pos+1,dlg.name_edit->text());
    }
    //write the list
    config_kdevelop->writeEntry("others_list",others_list);
    // write the props
    config_kdevelop->writeEntry(dlg.name_edit->text(),dlg.file_edit->text());
    config_kdevelop->sync();
    refresh(project);
  }
}

void CDocTree::slotRemoveDocumentation(){
  QString name = QString(getCurrentItem()->getText());
  QStrList others_list;

  config_kdevelop->setGroup("Other_Doc_Location");
  config_kdevelop->readListEntry("others_list",others_list);
  others_list.remove(name);
  config_kdevelop->writeEntry("others_list",others_list);
  
  refresh(project);
}
void CDocTree::slotDocumentationProp(){
  QString name = QString(getCurrentItem()->getText());
  config_kdevelop->setGroup("Other_Doc_Location");
  QString filename = config_kdevelop->readEntry(name);

  CDocTreePropDlg dlg;
  dlg.setCaption("Properties...");
  dlg.name_edit->setText(name);
  dlg.name_edit->setEnabled(false);
  dlg.file_edit->setText(filename);

  if(dlg.exec()){
    config_kdevelop->setGroup("Other_Doc_Location");
    config_kdevelop->writeEntry(name,dlg.file_edit->text());
    config_kdevelop->sync();
  }
}
