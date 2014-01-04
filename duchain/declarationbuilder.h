/*
    This file is part of KDevelop

    Copyright 2013 Olivier de Gaalon <olivier.jg@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef DECLARATIONBUILDER_H
#define DECLARATIONBUILDER_H

#include "clangtypes.h"

#include <language/duchain/identifier.h>
#include <language/duchain/ducontext.h>
#include <language/duchain/declaration.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/types/integraltype.h>

namespace DeclarationBuilder {

using namespace KDevelop;

inline QByteArray buildComment(CXComment comment)
{
    auto kind = clang_Comment_getKind(comment);
    if (kind == CXComment_Text)
        return {ClangString(clang_TextComment_getText(comment))};

    QByteArray text;
    int numChildren = clang_Comment_getNumChildren(comment);
    for (int i = 0; i < numChildren; ++i)
        text += buildComment(clang_Comment_getChild(comment, i));
    return text;
}

KDevelop::AbstractType* createType(CXType type)
{
    switch (type.kind) {
        case CXType_Void:
            return new IntegralType(IntegralType::TypeVoid);
        case CXType_Bool:
            return new IntegralType(IntegralType::TypeBoolean);
        case CXType_Int:
            return new IntegralType(IntegralType::TypeInt);
        case CXType_Float:
            return new IntegralType(IntegralType::TypeFloat);
        case CXType_Double:
            return new IntegralType(IntegralType::TypeDouble);
        case CXType_LongDouble: {
            auto type = new IntegralType(IntegralType::TypeDouble);
            type->setModifiers(AbstractType::LongModifier);
            return type;
        }
        case CXType_Char_S: {
            auto type = new IntegralType(IntegralType::TypeChar);
            type->setModifiers(AbstractType::SignedModifier);
            return type;
        }
        case CXType_Char_U: {
            auto type = new IntegralType(IntegralType::TypeChar);
            type->setModifiers(AbstractType::UnsignedModifier);
            return type;
        }
        case CXType_Char16:
            return new IntegralType(IntegralType::TypeChar16_t);
        case CXType_Char32:
            return new IntegralType(IntegralType::TypeChar32_t);
        case CXType_Long:
            return new IntegralType(IntegralType::TypeLong);
        case CXType_LongLong: {
            auto type = new IntegralType(IntegralType::TypeLong);
            type->setModifiers(AbstractType::LongModifier);
            return type;
        }
        default:
            return nullptr;
    }
}

template<class T> T *createDeclarationCommon(CXCursor cursor, const Identifier& id)
{
    auto range = ClangRange(clang_Cursor_getSpellingNameRange(cursor, 0, 0)).toRangeInRevision();
    auto comment = buildComment(clang_Cursor_getParsedComment(cursor));
    auto decl = new T(range, nullptr);
    decl->setComment(comment);
    decl->setIdentifier(id);
    decl->setAbstractType(AbstractType::Ptr(createType(clang_getCursorType(cursor))));
    return decl;
}

template<class T> T *createDecl(CXCursor cursor, const Identifier& id, DUContext* parentContext)
{
    auto decl = createDeclarationCommon<T>(cursor, id);
    DUChainWriteLocker lock;
    decl->setContext(parentContext);
    return decl;
}

template<class T> T *createCtxtDecl(CXCursor cursor, const Identifier& id, DUContext* internalContext, DUContext* parentContext)
{
    auto decl = createDeclarationCommon<T>(cursor, id);
    DUChainWriteLocker lock;
    decl->setContext(parentContext);
    decl->setInternalContext(internalContext);
    return decl;
}

template<CXCursorKind> Declaration *build(CXCursor, const Identifier&, DUContext*);

#define AddDeclarationBuilder(CK, DT) \
template<> Declaration* build<CK>(CXCursor c, const Identifier& i, DUContext *p)\
{ return createDecl<DT>(c, i, p); }

template<CXCursorKind> Declaration *build(CXCursor, const Identifier&, DUContext*, DUContext *);

#define AddCtxtDeclBuilder(CK, DT)\
template<> Declaration* build<CK>(CXCursor c, const Identifier& id, DUContext *ic, DUContext *p)\
{ return createCtxtDecl<DT>(c, id, ic, p); }

#define AddBothBuilders(CursorKind, DeclType)\
AddDeclarationBuilder(CursorKind, DeclType)\
AddCtxtDeclBuilder(CursorKind, DeclType)

AddDeclarationBuilder(CXCursor_UnexposedDecl, Declaration);
AddBothBuilders(CXCursor_StructDecl, Declaration);
AddBothBuilders(CXCursor_UnionDecl, Declaration);
AddBothBuilders(CXCursor_ClassDecl, Declaration);
AddBothBuilders(CXCursor_EnumDecl, Declaration);
AddDeclarationBuilder(CXCursor_FieldDecl, Declaration);
AddDeclarationBuilder(CXCursor_EnumConstantDecl, Declaration);
AddBothBuilders(CXCursor_FunctionDecl, Declaration);
AddDeclarationBuilder(CXCursor_VarDecl, Declaration);
AddDeclarationBuilder(CXCursor_ParmDecl, Declaration);
AddDeclarationBuilder(CXCursor_TypedefDecl, Declaration);
AddBothBuilders(CXCursor_CXXMethod, Declaration);
AddCtxtDeclBuilder(CXCursor_Namespace, Declaration);
AddBothBuilders(CXCursor_Constructor, Declaration);
AddBothBuilders(CXCursor_Destructor, Declaration);
AddBothBuilders(CXCursor_ConversionFunction, Declaration);
AddDeclarationBuilder(CXCursor_TemplateTypeParameter, Declaration);
AddDeclarationBuilder(CXCursor_NonTypeTemplateParameter, Declaration);
AddDeclarationBuilder(CXCursor_TemplateTemplateParameter, Declaration);
AddBothBuilders(CXCursor_FunctionTemplate, Declaration);
AddBothBuilders(CXCursor_ClassTemplate, Declaration);
AddBothBuilders(CXCursor_ClassTemplatePartialSpecialization, Declaration);
AddDeclarationBuilder(CXCursor_NamespaceAlias, Declaration);
AddDeclarationBuilder(CXCursor_UsingDirective, Declaration);
AddDeclarationBuilder(CXCursor_UsingDeclaration, Declaration);
AddDeclarationBuilder(CXCursor_TypeAliasDecl, Declaration);

}

#endif // DECLARATIONBUILDER_H
