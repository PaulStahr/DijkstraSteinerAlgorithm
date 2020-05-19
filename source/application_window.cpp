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

#include "application_window.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

namespace GL_UTIL
{
    GlWindow::GlWindow(
                         int width,
                         int height, int bpp ):
                           _width  ( width                  ),
                           _height ( height                 ),
                           _bpp    ( bpp                    ),
                           _caption( "Tomprogs Application" ),
                           _state  ( 0                      )
    {}
    
    GlWindow::~GlWindow()
    {
        destroy();
    }
    
    bool GlWindow::create()
    {
        if( _state & OPENED_WINDOW )
            destroy();
        
        if(    !SDL_WasInit( SDL_INIT_VIDEO ) )
            SDL_InitSubSystem( SDL_INIT_VIDEO );
        SDL_WM_SetCaption(   _caption.c_str(), NULL );
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        _surface = SDL_SetVideoMode(_width, _height, _bpp,SDL_OPENGL | ( (_state & FULL_SCREEN) ? SDL_FULLSCREEN : 0 ) );
        if( !_surface ) return false;
        SDL_ShowCursor( 0 );
        _state |= OPENED_WINDOW;
        return true;
    }
    
    bool GlWindow::destroy()
    {
        if( !(_state & OPENED_WINDOW) ) return true;
        SDL_FreeSurface( _surface );
        _state &= ~OPENED_WINDOW;
        return true;
    }
    
    void GlWindow::setResolution( int width, int height )
    {
        _width  = width;
        _height = height;
    }   
    
    void GlWindow::setColorDepth( int bpp )
    {
        _bpp = bpp;
    }
    
    void GlWindow::setCaption( const std::string& caption )
    {
        _caption = caption;
    }
    
    size_t GlWindow::get_width() const
    {
        return _width;
    }

    size_t GlWindow::get_height() const
    {
        return _height;
    }
    
    void GlWindow::setFullscreen( bool enable )
    {
        if( enable )
        {
            _state |= FULL_SCREEN;
        }
        else
        {
            _state &= ~FULL_SCREEN;
        }
    }
    
    SDL_Surface* GlWindow::getSurface()
    {
        return _surface;
    }
}

