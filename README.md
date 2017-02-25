# PIP-demo

画中画示例
-----

关键知识点

1.读取jpg文件，默认格式是bgr，我们需要转化。 这里有两种方法

第一种方法：修改shader，使用cpu，交换pixels数组元素
效率很慢 
```
for(size_t i = 0; i <  width*height*3; i+=3)
			{
				BYTE temp	= pixels[i];
				pixels[i]	= pixels[i+2];
				pixels[i + 2]	= temp;
			}
```



第二种方法：修改shader，用gpu来实现

```
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
            " gl_FragColor   =   tColor0 * _outColor;"
            "}"
        };
```
2.抽象出来ProgramId类和TextureId

我们以后可以方便地写各种shader，只需要继承programId就可以了
而TextureId对glTexturexx函数进行了包装，我们就不需要关注具体流程。
