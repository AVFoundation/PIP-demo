#pragma once

#include <assert.h>
#include <EGL/egl.h>
#include <gles2/gl2.h>

#include "freeImage/FreeImage.h"

class    ShaderId
{
public:
    ShaderId()
    {
        _shaderId   =   -1;
    }
    int _shaderId;
};

class TextureId
{
public:
	int		_textureId;
public:
	TextureId()
	{
		_textureId = -1;
	}

	void activeTexture()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _textureId);
	}

	void getTexture(const char * fileName)
	{
		_textureId = createTextureFromImage(fileName, false);
	}

	void getSubTexture(const char * fileName)
	{
		 createTextureFromImage(fileName, true);
	}

	void    releaseTexture(GLuint texId)
	{
		glDeleteTextures(1, &texId);
	}
protected:
	/**
    *   使用FreeImage加载图片
    */
    unsigned    createTextureFromImage(const char* fileName, bool isSubTexture)
    {
        //1 获取图片格式
        FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(fileName, 0);
        if (fifmt == FIF_UNKNOWN)
        {
            return  0;
        }
        //2 加载图片
        FIBITMAP    *dib = FreeImage_Load(fifmt, fileName, 0);

        FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType(dib);

        //! 获取数据指针
        FIBITMAP*   temp = dib;
        dib = FreeImage_ConvertTo32Bits(dib);
        FreeImage_Unload(temp);

        BYTE*   pixels = (BYTE*)FreeImage_GetBits(dib);
        int     width = FreeImage_GetWidth(dib);
        int     height = FreeImage_GetHeight(dib);

		unsigned    res;

		if(!isSubTexture)
			   res = createTexture(width, height, pixels, GL_RGBA);
		else
			   res = createSubTexture(width, height, pixels, GL_RGBA);

        FreeImage_Unload(dib);
        return      res;
    }

	unsigned    createTexture(int w, int h, const void* data, GLenum type)
    {
        unsigned    texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, data);


        return  texId;
    }


	unsigned    createSubTexture(int w, int h, const void* data, GLenum type)
    {
       
        glTexSubImage2D(
				GL_TEXTURE_2D,
				0,				//指定是第一级别的纹理
				100,			//这里subTexture的xoffset和yoffset固定为100
				100,			//
				w,
				h,
				type,			//数据的格式，即windows下操作的图片数据
				GL_UNSIGNED_BYTE,
				data
				);
		return 0;
    }
};


/**
*   程序
*/
class   ProgramId
{
public:
    int         _programId;
    ShaderId    _vertex;
    ShaderId    _fragment;
public:
    ProgramId()
    {
        _programId  =   -1;
    }
public:
    /**
    *   加载函数
    */
    bool    createProgram( const char* vertex,const char* fragment )
    {
        bool        error   =   false;
        do 
        {
            if (vertex)
            {
                _vertex._shaderId   = glCreateShader( GL_VERTEX_SHADER );
                glShaderSource( _vertex._shaderId, 1, &vertex, 0 );
                glCompileShader( _vertex._shaderId );

                GLint   compileStatus;
                glGetShaderiv( _vertex._shaderId, GL_COMPILE_STATUS, &compileStatus );
                error   =   compileStatus == GL_FALSE;
                if( error )
                {
                    GLchar messages[256];
                    glGetShaderInfoLog( _vertex._shaderId, sizeof(messages), 0,messages);
                    assert( messages && 0 != 0);
                    break;
                }
            }
            if (fragment)
            {
                _fragment._shaderId   = glCreateShader( GL_FRAGMENT_SHADER );
                glShaderSource( _fragment._shaderId, 1, &fragment, 0 );
                glCompileShader( _fragment._shaderId );

                GLint   compileStatus;
                glGetShaderiv( _fragment._shaderId, GL_COMPILE_STATUS, &compileStatus );
                error   =   compileStatus == GL_FALSE;

                if( error )
                {
                    GLchar messages[256];
                    glGetShaderInfoLog( _fragment._shaderId, sizeof(messages), 0,messages);
                    assert( messages && 0 != 0);
                    break;
                }
            }
            _programId  =   glCreateProgram( );

            if (_vertex._shaderId)
            {
                glAttachShader( _programId, _vertex._shaderId);
            }
            if (_fragment._shaderId)
            {
                glAttachShader( _programId, _fragment._shaderId);
            }

            glLinkProgram( _programId );

            GLint linkStatus;
            glGetProgramiv( _programId, GL_LINK_STATUS, &linkStatus );
            if (linkStatus == GL_FALSE)
            {
                GLchar messages[256];
                glGetProgramInfoLog( _programId, sizeof(messages), 0, messages);
                break;
            }
            glUseProgram(_programId);

        } while(false);

        if (error)
        {
            if (_fragment._shaderId)
            {
                glDeleteShader(_fragment._shaderId);
                _fragment._shaderId =   0;
            }
            if (_vertex._shaderId)
            {
                glDeleteShader(_vertex._shaderId);
                _vertex._shaderId   =   0;
            }
            if (_programId)
            {
                glDeleteProgram(_programId);
                _programId          =   0;
            }
        }
        return  true;
    }

    /**
    *   使用程序
    */
    virtual void    begin()
    {
        glUseProgram(_programId);
        
    }
    /**
    *   使用完成
    */
    virtual void    end()
    {
        glUseProgram(0);
    }
};


class   PROGRAM_P2_C4 :public ProgramId
{
public:
    typedef int attribute; 
    typedef int uniform;
public:
    attribute   _position;
    uniform     _color;
    uniform     _MVP;
public:
    PROGRAM_P2_C4()
    {
        _position   =   -1;
        _color      =   -1;
        _MVP        =   -1;
    }
    ~PROGRAM_P2_C4()
    {
    }
   
    /// 初始化函数
    virtual bool    initialize()
    {
        const char* vs  =   
        {
            "precision lowp float; "
            "uniform   mat4 _MVP;"
            "attribute vec2 _position;"

            "void main()"
            "{"
            "   vec4    pos =   vec4(_position,0,1);"
            "   gl_Position =   _MVP * pos;"
            "}"
        };
        const char* ps  =   
        {
            "precision  lowp float; "
            "uniform    vec4 _color;"
            "void main()"
            "{"
            "   gl_FragColor   =   _color;"
            "}"
        };
        
        bool    res =   createProgram(vs,ps);
        if(res)
        {
			//获取变量的句柄
            _position   =   glGetAttribLocation(_programId,"_position");
            _color      =   glGetUniformLocation(_programId,"_color");
            _MVP        =   glGetUniformLocation(_programId,"_MVP");
        }
        return  res;
    }

    /**
    *   使用程序
    */
    virtual void    begin()
    {
        glUseProgram(_programId);
        glEnableVertexAttribArray(_position);
        
    }
    /**
    *   使用完成
    */
    virtual void    end()
    {
        glDisableVertexAttribArray(_position);
        glUseProgram(0);
    }
};

class   PROGRAM_P2_AC4 :public ProgramId
{
public:
    typedef int attribute; 
    typedef int uniform;
public:
    attribute   _position;
    uniform     _color;
    uniform     _MVP;
public:
    PROGRAM_P2_AC4()
    {
        _position   =   -1;
        _color      =   -1;
        _MVP        =   -1;
    }
    ~PROGRAM_P2_AC4()
    {
    }
   
    /// 初始化函数
    virtual bool    initialize()
    {
        const char* vs  =   
        {
            "precision lowp float; "
            "uniform   mat4 _MVP;"
            "attribute vec2 _position;"
			"attribute vec4 _color;"
			"varying vec4 _outColor;"

            "void main()"
            "{"
            "   vec4    pos =   vec4(_position,0,1);"
			"	_outColor = _color;"
            "   gl_Position =   _MVP * pos;"
            "}"
        };
        const char* ps  =   
        {
            "precision  lowp float; "
            "varying    vec4 _outColor;"
            "void main()"
            "{"
            "   gl_FragColor   =   _outColor;"
            "}"
        };
        
        bool    res =   createProgram(vs,ps);
        if(res)
        {
			//获取变量的句柄
            _position   =   glGetAttribLocation(_programId,"_position");
            _color      =   glGetAttribLocation(_programId,"_color");
            _MVP        =   glGetUniformLocation(_programId,"_MVP");
        }
        return  res;
    }

    /**
    *   使用程序
    */
    virtual void    begin()
    {
        glUseProgram(_programId);
        glEnableVertexAttribArray(_position);
		glEnableVertexAttribArray(_color);
        
    }
    /**
    *   使用完成
    */
    virtual void    end()
    {
        glDisableVertexAttribArray(_position);
		glDisableVertexAttribArray(_color);
        glUseProgram(0);
    }
};
class   PROGRAM_P2_UV_AC4 :public ProgramId
{
public:
    typedef int attribute; 
    typedef int uniform;
public:
    attribute   _position;
    attribute   _color;
	attribute	_uv;
    uniform     _MVP;
	uniform		_texture;
	//uniform		_texture1;
	//uniform		_uvAnim;
public:
    PROGRAM_P2_UV_AC4()
    {
        _position   =   -1;
        _color      =   -1;
		_uv			=	-1;
        _MVP        =   -1;
		_texture	=	-1;
		//_texture1	=	-1;
		//_uvAnim		=	-1;
    }
    ~PROGRAM_P2_UV_AC4()
    {
    }
   
    /// 初始化函数
    virtual bool    initialize()
    {
        /*const char* vs  =   
        {
            "precision lowp float; "
            "uniform   mat4 _MVP;"
            "attribute vec2 _position;"
			"attribute vec2 _uv;"
			"attribute vec4 _color;"
			"varying vec4 _outColor;"
			"varying vec2 _outUV;"

            "void main()"
            "{"
            "   vec4    pos =   vec4(_position,0,1);"
			"	_outColor = _color;"
			"	_outUV	= _uv;"
            "   gl_Position =   _MVP * pos;"
            "}"
        };
        const char* ps  =   
        {
            "precision  lowp float; "
			"uniform	sampler2D	_texture;"
			"uniform	sampler2D	_texture1;"
			"uniform	float		_uvAnim;"
            "varying    vec4 _outColor;"
			"varying    vec2 _outUV;"
            "void main()"
            "{"
			"	vec4	tColor0	=	texture2D(_texture, _outUV);"
			"	vec2	newUV= vec2(_uvAnim+_outUV.x, _outUV.y);"
			"	vec4	tColor1	=	texture2D(_texture1, newUV);"
            "   gl_FragColor   =   tColor0 + tColor1;"
            "}"
        };*/
		const char* vs  =   
        {
            "precision lowp float; "
            "uniform   mat4 _MVP;"
            "attribute vec2 _position;"
			"attribute vec2 _uv;"
			"attribute vec4 _color;"
			"varying vec4 _outColor;"
			"varying vec2 _outUV;"

            "void main()"
            "{"
            "   vec4    pos =   vec4(_position,0,1);"
			"	_outColor = _color;"
			"	_outUV	= _uv;"
            "   gl_Position =   _MVP * pos;"
            "}"
        };
        const char* ps  =   
        {
            "precision  lowp float; "
			"uniform	sampler2D	_texture;"
            "varying    vec4 _outColor;"
			"varying    vec2 _outUV;"
            "void main()"
            "{"
			"	vec4	tColor0	=	texture2D(_texture, _outUV);"
			"	tColor0 = tColor0.bgra;"
            "   gl_FragColor   =   tColor0 * _outColor;"
            "}"
        };
        bool    res =   createProgram(vs,ps);
        if(res)
        {
			//获取变量的句柄
            _position   =   glGetAttribLocation(_programId,"_position");
            _color      =   glGetAttribLocation(_programId,"_color");
			_uv			=	glGetAttribLocation(_programId,"_uv");
			//_uvAnim		=	glGetUniformLocation(_programId,"_uvAnim");
            _MVP        =   glGetUniformLocation(_programId,"_MVP");
			_texture    =   glGetUniformLocation(_programId,"_texture");
           // _texture1   =   glGetUniformLocation(_programId,"_texture1");
        }
        return  res;
    }

    /**
    *   使用程序
    */
    virtual void    begin()
    {
        glUseProgram(_programId);
        glEnableVertexAttribArray(_position);
		glEnableVertexAttribArray(_color);
		glEnableVertexAttribArray(_uv);
     
    }
    /**
    *   使用完成
    */
    virtual void    end()
    {
        glDisableVertexAttribArray(_position);
		glDisableVertexAttribArray(_color);
		glDisableVertexAttribArray(_uv);
        glUseProgram(0);
    }
};
