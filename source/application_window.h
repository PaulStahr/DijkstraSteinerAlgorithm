/*******************************************************************************
 * Copyright (c) 2019 Paul Stahr
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef APPLICATION_WINDOW_H_
#define APPLICATION_WINDOW_H_

#include <SDL/SDL_video.h>
#include <string>

namespace GL_UTIL
{
  class GlWindow
  {
    public:
      GlWindow( int width = 0, int height = 0, int bpp = 0);
      virtual ~GlWindow();
      bool create();
      bool destroy();
      void setResolution( int width, int height );
      void setColorDepth( int bpp );
      void setCaption( const std::string& caption );
      void setFullscreen( bool enable = true );
      SDL_Surface* getSurface();
      size_t get_width() const;
      size_t get_height() const;
      enum
      {
        OPENED_WINDOW = 0x0001,
        FULL_SCREEN   = 0x0002
      };

    private:
      int           _width;
      int           _height;
      int           _bpp;

      std::string   _caption;

      SDL_Surface  *_surface;
      char          _state;

      GlWindow(const GlWindow& src);
      GlWindow& operator=(const GlWindow& src);
  };

}

#endif

