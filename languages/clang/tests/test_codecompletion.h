/*
 * Copyright 2014  David Stevens <dgedstevens@gmail.com>
 * Copyright 2014  Kevin Funk <kfunk@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TESTCODECOMPLETION_H
#define TESTCODECOMPLETION_H

#include <QObject>

class TestCodeCompletion : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testIncludePathCompletion_data();
    void testIncludePathCompletion();
    void testIncludePathCompletionLocal();

    void testClangCodeCompletion();
    void testClangCodeCompletion_data();
    void testVirtualOverride();
    void testVirtualOverride_data();
    void testImplement();
    void testImplement_data();
    void testImplementOtherFile();
    void testInvalidCompletions();
    void testInvalidCompletions_data();
    void testCompletionPriority();
    void testCompletionPriority_data();
    void testReplaceMemberAccess();
    void testReplaceMemberAccess_data();
    void testArgumentHintCompletion();
    void testArgumentHintCompletion_data();

    void testOverloadedFunctions();
    void testVariableScope();
    void testArgumentHintCompletionDefaultParameters();
};

#endif // TESTCODECOMPLETION_H
