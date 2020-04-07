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

