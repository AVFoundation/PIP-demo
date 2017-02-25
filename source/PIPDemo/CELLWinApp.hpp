#pragma once

#include <Windows.h>
#include <tchar.h>

/*#include <EGL/egl.h>
#include <gles2/gl2.h>

#include "freeImage/FreeImage.h"*/

#include "CELLMath.hpp"
#include "CELLShader.hpp"

namespace   CELL
{
    class   CELLWinApp
    {
    public:
        //! 实例句柄
        HINSTANCE   _hInstance;
        //! 窗口句柄
        HWND        _hWnd;
        //! 窗口的高度
        int         _width;
        //! 窗口的宽度
        int         _height;
        /// for gles2.0
        EGLConfig   _config;
        EGLSurface  _surface;
        EGLContext  _context;
        EGLDisplay  _display;

		PROGRAM_P2_UV_AC4 _shader;
		TextureId        _textureId;
		TextureId        _textureId2;


    public:
        CELLWinApp(HINSTANCE hInstance)
            :_hInstance(hInstance)
        {
            WNDCLASSEX  winClass;
            winClass.lpszClassName  =   _T("CELLWinApp");
            winClass.cbSize         =   sizeof(winClass);
            winClass.style          =   CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            winClass.lpfnWndProc    =   wndProc;
            winClass.hInstance      =   hInstance;
            winClass.hIcon	        =   0;
            winClass.hIconSm	    =   0;
            winClass.hCursor        =   LoadCursor(hInstance, IDC_ARROW);
            winClass.hbrBackground  =   (HBRUSH)GetStockObject(BLACK_BRUSH);
            winClass.lpszMenuName   =   NULL;
            winClass.cbClsExtra     =   0;
            winClass.cbWndExtra     =   0;
            RegisterClassEx(&winClass);
        }
        virtual ~CELLWinApp()
        {
            UnregisterClass(_T("CELLWinApp"),_hInstance);
        }

        /**
        *   初始化 OpenGLES2.0
        */
        bool    initOpenGLES20()
        {
            const EGLint attribs[] =
            {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE,24,
                EGL_NONE
            };
            EGLint 	format(0);
            EGLint	numConfigs(0);
            EGLint  major;
            EGLint  minor;

            //! 1
            _display	    =	eglGetDisplay(EGL_DEFAULT_DISPLAY);

            //! 2init
            eglInitialize(_display, &major, &minor);

            //! 3
            eglChooseConfig(_display, attribs, &_config, 1, &numConfigs);

            eglGetConfigAttrib(_display, _config, EGL_NATIVE_VISUAL_ID, &format);
            //! 4 
            _surface	    = 	eglCreateWindowSurface(_display, _config, _hWnd, NULL);

            //! 5
            EGLint attr[]   =   { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
            _context 	    = 	eglCreateContext(_display, _config, 0, attr);
            //! 6
            if (eglMakeCurrent(_display, _surface, _surface, _context) == EGL_FALSE)
            {
                return false;
            }

            eglQuerySurface(_display, _surface, EGL_WIDTH,  &_width);
            eglQuerySurface(_display, _surface, EGL_HEIGHT, &_height);

            return  true;

        }
        /**
        *   销毁OpenGLES2.0
        */
        void    destroyOpenGLES20()
        {
            if (_display != EGL_NO_DISPLAY)
            {
                eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                if (_context != EGL_NO_CONTEXT) 
                {
                    eglDestroyContext(_display, _context);
                }
                if (_surface != EGL_NO_SURFACE) 
                {
                    eglDestroySurface(_display, _surface);
                }
                eglTerminate(_display);
            }
            _display    =   EGL_NO_DISPLAY;
            _context    =   EGL_NO_CONTEXT;
            _surface    =   EGL_NO_SURFACE;
        }


    protected:
        static  LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            
            CELLWinApp*  pThis   =   (CELLWinApp*)GetWindowLong(hWnd,GWL_USERDATA);
            if (pThis)
            {
                return  pThis->onEvent(hWnd,msg,wParam,lParam);
            }
            if (WM_CREATE == msg)
            {
                CREATESTRUCT*   pCreate =   (CREATESTRUCT*)lParam;
                SetWindowLong(hWnd,GWL_USERDATA,(DWORD_PTR)pCreate->lpCreateParams);
            }
            return  DefWindowProc( hWnd, msg, wParam, lParam );
        }
    public:
        /**
        *   事件函数
        */
        virtual LRESULT onEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            switch (msg)
            {
            case WM_CLOSE:
            case WM_DESTROY:
                {
                    ::PostQuitMessage(0);
                }
                break;
            case WM_MOUSEMOVE:
                break;
            default:
                return  DefWindowProc( hWnd, msg, wParam, lParam ); 
            }
            return  S_OK;
            
        }
        /**
        *   绘制函数
        */
        virtual void    render()
        {
			struct Vertex
			{
				CELL::float2 pos;
				CELL::float2 uv;
				CELL::Rgba4Byte color;
			};
            /*glClearColor(1,0,0,1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0,0,_width,_height);*/
			//清空缓冲区
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//定义视口
			glViewport(0,0,_width,_height);

			CELL::matrix4 screen_prj = CELL::ortho<float>(0
														,float(_width)
														,float(_height)
														,0
														, -100.0f
														,100);
			_shader.begin();
			
			{
				static float incUV = 0;
				incUV += 0.01f;
				float	x = 100;
				float	y = 100;
				float	w = 400;
				float	h = 400;
				struct Vertex vertex[] =
				{
					CELL::float2(x,y),          CELL::float2(0,0),      CELL::Rgba4Byte(255,255,255,255),
                    CELL::float2(x + w,y),      CELL::float2(1,0),      CELL::Rgba4Byte(255,255,255,255),
                    CELL::float2(x,y + h),      CELL::float2(0,1),      CELL::Rgba4Byte(255,255,255,255),
                    CELL::float2(x + w, y + h), CELL::float2(1,1),      CELL::Rgba4Byte(255,255,255,255),
				};

		
				CELL::matrix4 matMVP;
			
				matMVP = screen_prj;

				_textureId.activeTexture();

				
				//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				/*glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, _textureId2);*/
				glUniformMatrix4fv(_shader._MVP, 1, false, matMVP.data());//uniform是全局的，attribute是局部变量
				glUniform1i(_shader._texture, 0);
				//glUniform1i(_shader._texture1, 1);
				//glUniform1f(_shader._uvAnim, incUV);
				glVertexAttribPointer(_shader._color, 4, GL_UNSIGNED_BYTE, true, sizeof(struct Vertex), &vertex[0].color);
				glVertexAttribPointer(_shader._uv, 2, GL_FLOAT, false, sizeof(struct Vertex), &vertex[0].uv);
				glVertexAttribPointer(_shader._position, 2, GL_FLOAT, false, sizeof(struct Vertex), vertex);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				

				
			}
#if 0
			{
				Vertex circle[362];
				float xCenter	= 200;
				float yCenter	= 200;
				float radius	= 200;

				circle[0].pos = CELL::float2(xCenter, yCenter);
				circle[0].color	= CELL::Rgba4Byte(255, 0, 0);
				for(size_t i = 0; i <= 360; ++i)
				{
					float x			=	xCenter + radius*cos(float(i)* PI/180.0);
					float y			=	yCenter + radius*sin(float(i)* PI/180.0);
					circle[i+1].pos	=	CELL::float2(x,y);
					//circle[i].color	= CELL::Rgba4Byte(255, 0, 0);
				}
				//glUniformMatrix4fv(_shader._MVP, 1, false, screen_prj.data());//uniform是全局的，attribute是局部变量
				glVertexAttribPointer(_shader._color, 4, GL_UNSIGNED_BYTE, true, sizeof(struct Vertex), &circle[0].color);
				glVertexAttribPointer(_shader._position, 2, GL_FLOAT, false, sizeof(struct Vertex), circle);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 362);
			}
#endif
			_shader.end();

        }
        /**
        *   主函数
        */
        int     main(int width,int height)
        {
            _hWnd   =   CreateWindowEx( NULL,
                                        _T("CELLWinApp"),
                                        _T("CELLWinApp"),
                                        WS_OVERLAPPEDWINDOW,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        width,
                                        height, 
                                        NULL, 
                                        NULL,
                                        _hInstance, 
                                        this
                                        );

            if (_hWnd == 0)
            {
                return  -1;
            }
            ShowWindow(_hWnd,SW_SHOW);

            if (!initOpenGLES20())
            {
                return  false;
            }
			_shader.initialize();
			
			_textureId.getTexture("data/image/main.jpg");
			_textureId.getSubTexture("data/image/grass.png");
			//加载第二张纹理
			//_textureId2 = loadTexture("data/image/fog.bmp");
            MSG msg =   {0};
            while(msg.message != WM_QUIT)
            {
                if (msg.message == WM_DESTROY || 
                    msg.message == WM_CLOSE)
                {
                    break;
                }
                /**
                *   有消息，处理消息，无消息，则进行渲染绘制
                */
                if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
                { 
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                {
                    render();
                    eglSwapBuffers(_display,_surface);
                }
            }
            /**
            *   销毁OpenGLES20
            */
			_textureId.releaseTexture(_textureId._textureId);
            destroyOpenGLES20();

            return  0;
        }
    };
}