#include "mtao/opengl/tex.h"
namespace mtao {
namespace opengl {
    Texture::Texture(GLint internalFormat, GLsizei w, GLsizei h, GLsizei d)
      : w(w), h(h), d(d), m_target(h == -1 ? GL_TEXTURE_1D : (d == -1 ? GL_TEXTURE_2D : GL_TEXTURE_3D)), internalFormat(internalFormat) {
        checkOpenGLErrors("Checking error before 3D texture");
        glGenTextures(1, &m_id);
        checkOpenGLErrors("Before bind in 3D texture ctor");
        bind_enabled<Texture>::bind();
        checkOpenGLErrors("Going to set texture parameters in 3D texture ctor");
        glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        checkOpenGLErrors("Texture::Texture() just called all the glTexParameters");
        resize(w, h, d);
        checkOpenGLErrors("Texture::Texture() just called member resize");
        if (internalFormat == GL_DEPTH_COMPONENT24) {
            //glTexParameteri(type, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
            //checkOpenGLErrors("Texture::Texture() 2d, glTexParameteri setting depth texture mode to intensity");
            glTexParameteri(m_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            checkOpenGLErrors("Texture::Texture() 2d, glTexParameteri GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE");
            glTexParameteri(m_target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            checkOpenGLErrors("Texture::Texture() 2d, glTexParameteri GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL");
        }
    }
    Texture::~Texture() {
        glDeleteTextures(1, &m_id);
        checkOpenGLErrors("Texture::~Texture() glDeleteTextures");
    }

    namespace texture_internal {
        GLenum internalFormatToFormat(GLint internalFormat) {
            switch (internalFormat) {
            case GL_DEPTH_COMPONENT24:
                return GL_DEPTH_COMPONENT;
            case GL_RGBA8:
            default:
                return GL_RGBA;
            }
        }
    }// namespace texture_internal

    namespace texture_internal {
        GLenum internalFormatToSizeFormat(GLint internalFormat) {
            switch (internalFormat) {
            case GL_DEPTH_COMPONENT24:
                return GL_FLOAT;
            case GL_RGBA8:
            default:
                return GL_UNSIGNED_BYTE;
            }
        }
    }// namespace texture_internal

    Texture::Texture(Texture &&t)
      : w(t.w), h(t.h), d(t.d), m_id(t.m_id), m_target(t.m_target), internalFormat(t.internalFormat) {
    }

    void Texture::resize(GLsizei w_, GLsizei h_, GLsizei d_) {
        setData(nullptr, w_, h_, d_);
        checkOpenGLErrors("Texture::resize() glTexImage3D");
    }


    void Texture::setData(const void *data, GLsizei w_, GLsizei h_, GLsizei d_) {
        w = w_ > 0 ? w_ : w;
        h = h_ > 0 ? h_ : h;
        d = d_ > 0 ? d_ : d;
        if (h_ == -1) {
            glTexImage1D(type,// target
                         0,// level
                         internalFormat,// internalFormat
                         w,// width
                         0,// border
                         texture_internal::internalFormatToFormat(internalFormat),// format
                         texture_internal::internalFormatToSizeFormat(internalFormat),// type
                         data);// data
        } else {
            if (d_ == -1) {

                glTexImage2D(type,// target
                             0,// level
                             internalFormat,// internalFormat
                             w,// width
                             h,// height
                             0,// border
                             texture_internal::internalFormatToFormat(internalFormat),// format
                             texture_internal::internalFormatToSizeFormat(internalFormat),// type
                             data);// data
            } else {
                glTexImage3D(type,// target
                             0,// level
                             internalFormat,// internalFormat
                             w,// width
                             h,// height
                             d,// depth
                             0,// border
                             texture_internal::internalFormatToFormat(internalFormat),// format
                             texture_internal::internalFormatToSizeFormat(internalFormat),// type
                             data);// data
            }
        }
        checkOpenGLErrors("Texture::setData() glTexImage?D");
    }


    void Texture::bind(GLuint id, GLenum type) {
        glBindTexture(type, id);
        checkOpenGLErrors("Texture::bind() glBindTexture");
    }

    void Texture::release(GLuint id, GLenum type) {
        glBindTexture(type, 0);
        checkOpenGLErrors("Texture::bind() glBindTexture");
    }

    namespace fbo_internal {
        GLint attachmentToType(GLenum attachment) {
            switch (attachment) {
            case GL_DEPTH_ATTACHMENT:
                return GL_DEPTH_COMPONENT24;
            case GL_COLOR_ATTACHMENT0:
                return GL_RGBA8;
            default:
                return GL_RGBA8;
            }
        }
    }// namespace fbo_internal

    /*
        // dump the current texture to disk in a file format
        void Texture::dump( const std::string& filename )
        {
        bind();

        assert( sizeof(GLint) == sizeof(GLfloat) ); // do the code properly if this assert fails!
        size_t s = sizeof(GLint) * w * h; // same size for int or float on these machines, 4bytes.
        void *b = malloc(s);
        unsigned int *buff = (unsigned int*)b;
        float *buffFloat = (float*)b;

        assert( buff );
        memset( buff, 1, s );

        //glPushAttrib( GL_COLOR_BUFFER_BIT | GL_PIXEL_MODE_BIT );
        //checkOpenGLErrors( "Texture::dump() glPushAttrib color buffer and pixel mode bits" );

        glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
        checkOpenGLErrors( "Texture::dump() glPixelStorei GL_UNPACK_SKIP 0" );
        glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
        checkOpenGLErrors( "Texture::dump() glPixelStorei GL_UNPACK_SKIP_ROWS 0 " );
        glPixelStorei( GL_UNPACK_ROW_LENGTH, w );
        checkOpenGLErrors( "Texture::dump() GLPIXELSTOREI GL_UNPACK_ROW_LENGTH w" );


        glGetTexImage( type,                                                              // GL_TEXTURE_2D
        0,                                                                 // mipmap lvl
        texture_internal::internalFormatToFormat(internalFormat),     // GL_RGBA
        texture_internal::internalFormatToSizeFormat(internalFormat), // float|int
        (void*)buff );
        checkOpenGLErrors( "Texture::dump() glGetTexImage" );

        //glPopAttrib();
        //checkOpenGLErrors( "Texture::dump() glPopAttrib()");

        QImage img( w, h, QImage::Format_RGB32 );
        // walk all the pixels and copy?  figure out the stream options or direct mem writes.
        // Also, where is the origin for Qimage?  Ogl is bottom left 0,0...
        // check the index walk here...
        for( int i = 0; i < h; ++i )     // row
        {
        for( int j = 0; j < w; ++j ) // column
        {
        unsigned int idx = i*w + j;
        // TODO - look at the native format and endianess of target system....
        static unsigned int a( 0xFF000000 ),
        b( 0x00FF0000 ),
        g( 0x0000FF00 ),
        r( 0x000000FF );

        QRgb p;

        if( internalFormat != GL_DEPTH_COMPONENT24 ) // slow to put an if in a loop...
        {
        unsigned int t( buff[idx] );
        p = QRgb( qRgba((t & r),
        (t & g) >> 8,
        (t & b) >> 16,
        (t & a) >> 24) );

        }
        else
        {
        // map to monocromatic image but let's use all channels since there's no
        // qt format that supports > 8 bits
        float f( buffFloat[idx] );
        unsigned int val = (int)( (1.f-f) * 255.f );
        p = QRgb( qRgba( val, val, val, 1 ) );
        }

        img.setPixel( j, i, p );

    }
    }

    img.save( filename.c_str(), "PNG", 100);

    free( buff ); // there's no school like the old school ;)

    release();
    }
    */
}// namespace opengl
}// namespace mtao
