/*
  Copyright 2006 Hamish Rodda <rodda@kde.org>

  Permission to use, copy, modify, distribute, and sell this software and its
  documentation for any purpose is hereby granted without fee, provided that
  the above copyright notice appear in all copies and that both that
  copyright notice and this permission notice appear in supporting
  documentation.

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  KDEVELOP TEAM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
  AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef STREAM_H
#define STREAM_H

#include <cppparserexport.h>

#include <QtCore/QIODevice>

#include <editor/simplecursor.h>
#include "anchor.h"

namespace rpp {

class LocationTable;

/**
 * Stream designed for character-at-a-time processing
 *
 * @author Hamish Rodda <rodda@kde.org>
 */
class KDEVCPPRPP_EXPORT Stream
{
    static const char newline;

  public:
    Stream();
    //If the given offset anchor has the member "collapsed" set to true, the position will be locked.
    explicit Stream( QByteArray * string, const Anchor& offset = Anchor(0,0), LocationTable* table = 0 );
    explicit Stream( QByteArray * string, LocationTable* table );
    virtual ~Stream();

    bool isNull() const;

    bool atEnd() const;

    void toEnd();

    /// Returns true if toEnd was called on this stream.
    bool skippedToEnd() const;

    int offset() const;

    const char& peek(uint offset = 1) const;

    QByteArray stringFrom(int offset) const;

    /// Move back \a offset chars in the stream
    void rewind(int offset = 1);

    /// \warning the input and output lines are not updated when calling this function.
    ///          if you're seek()ing over a line boundary, you'll need to fix the line and column
    ///          numbers.
    void seek(int offset);

    /// Start from the beginning again
    void reset();

    /// Lock/unlock the input position. If the input position is locked, it will not be moved forwards.
    void lockInputPosition(bool lock);

    /// If a macro-expansion is set, all anchors given to mark() will get that macro-expansion set.
    /// It marks the position from where the macro-expansion was started that leads to the current output @see rpp::Anchor
    void setMacroExpansion(const KDevelop::SimpleCursor&);
    KDevelop::SimpleCursor macroExpansion() const;
    
    inline const char& current() const { return *c; } //krazy:exclude=inline
    inline operator const char&() const { return *c; } //krazy:exclude=inline
    inline Stream& operator++() //krazy:exclude=inline
    {
      if (c == end)
        return *this;

      if(m_inputPositionLocked)
        ++m_inputLineStartedAt;
      else if (*c == newline) {
        ++m_inputLine;
        m_inputLineStartedAt = m_pos + 1;
      }

      ++c;
      ++m_pos;

      return *this;
    }

    ///@warning Doesn't handle newlines correctly!
    Stream& operator--();

    ///Returns the cursor that points to the current input position.
    Anchor inputPosition() const;
    ///If the input position is collapsed, the input position will be locked from now on. It will stay the same until a new one is set.
    void setInputPosition(const Anchor& position);

    ///Input-position that marks the start of the topmost currently expanding macro in the original document
    KDevelop::SimpleCursor originalInputPosition() const;
    void setOriginalInputPosition(const KDevelop::SimpleCursor& position);

    ///Used for output streams to mark stream positions with input position
    ///The macroExpansion member may have no effect if macroExpansion is set for this stream.
    void mark(const Anchor& position);

    Stream & operator<< ( const char& c );
    Stream & operator<< ( const Stream& input );
    Stream& appendString( const Anchor& inputPosition, const QByteArray & string );

  private:
    Q_DISABLE_COPY(Stream)

    QByteArray* m_string;
    const char* c;
    const char* end;
    bool m_isNull, m_skippedToEnd, m_inputPositionLocked;
    KDevelop::SimpleCursor m_macroExpansion;
    int m_pos;
    int m_inputLine;
    int m_inputLineStartedAt;
    LocationTable* m_locationTable;
    KDevelop::SimpleCursor m_originalInputPosition;
};

}

#endif
