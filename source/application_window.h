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

      //----------------------------------------------------------------

      int           _width;
      int           _height;
      int           _bpp;

      std::string   _caption;

      SDL_Surface  *_surface;
      char          _state;

      //----------------------------------------------------------------
      ///
      /// Don't allow any copying of this class

      GlWindow(const GlWindow& src);
      GlWindow& operator=(const GlWindow& src);
  };

} // namespace TomprogsTutorial_

#endif /*APPLICATION_WINDOW_H_*/

