#pragma once

#include <assert.h>

class    ShaderId
{
public:
    ShaderId()
    {
        _shaderId   =   -1;
    }
    int _shaderId;
};


/**
*   ����
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
    *   ���غ���
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
    *   ʹ�ó���
    */
    virtual void    begin()
    {
        glUseProgram(_programId);
        
    }
    /**
    *   ʹ�����
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
   
    /// ��ʼ������
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
			//��ȡ�����ľ��
            _position   =   glGetAttribLocation(_programId,"_position");
            _color      =   glGetUniformLocation(_programId,"_color");
            _MVP        =   glGetUniformLocation(_programId,"_MVP");
        }
        return  res;
    }

    /**
    *   ʹ�ó���
    */
    virtual void    begin()
    {
        glUseProgram(_programId);
        glEnableVertexAttribArray(_position);
        
    }
    /**
    *   ʹ�����
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
   
    /// ��ʼ������
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
			//��ȡ�����ľ��
            _position   =   glGetAttribLocation(_programId,"_position");
            _color      =   glGetAttribLocation(_programId,"_color");
            _MVP        =   glGetUniformLocation(_programId,"_MVP");
        }
        return  res;
    }

    /**
    *   ʹ�ó���
    */
    virtual void    begin()
    {
        glUseProgram(_programId);
        glEnableVertexAttribArray(_position);
		glEnableVertexAttribArray(_color);
        
    }
    /**
    *   ʹ�����
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
   
    /// ��ʼ������
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
            "   gl_FragColor   =   tColor0 * _outColor;"
            "}"
        };
        bool    res =   createProgram(vs,ps);
        if(res)
        {
			//��ȡ�����ľ��
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
    *   ʹ�ó���
    */
    virtual void    begin()
    {
        glUseProgram(_programId);
        glEnableVertexAttribArray(_position);
		glEnableVertexAttribArray(_color);
		glEnableVertexAttribArray(_uv);
     
    }
    /**
    *   ʹ�����
    */
    virtual void    end()
    {
        glDisableVertexAttribArray(_position);
		glDisableVertexAttribArray(_color);
		glDisableVertexAttribArray(_uv);
        glUseProgram(0);
    }
};
