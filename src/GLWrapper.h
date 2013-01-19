#ifndef GLWRAPPER_H
#define GLWRAPPER_H

#include <stdexcept>
#include <memory>
#include <GL/glew.h>
#ifdef WIN32
#include <GL/wglew.h>
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "Vector.h"

class GLException;
class Window;
class Context;

template<class T>
inline std::string toString(T n)
{
    std::stringstream strstream;
    strstream << n;
    return strstream.str();
}

class SDLException : public std::runtime_error
{
public:
    SDLException(const std::string& message)
        : std::runtime_error(message.c_str()) {}
    ~SDLException() throw() {}
};

class GLException : public std::runtime_error
{
public:
    GLException(const std::string& message)
        : std::runtime_error(message.c_str()) {}
    ~GLException() throw() {}
};

class GLCompileError : public GLException
{
public:
    GLCompileError(const std::string& errorMsg)
        : GLException(errorMsg) {}
    ~GLCompileError() throw() {}
};

class FileNotFoundException : public std::runtime_error
{
public:
    FileNotFoundException(const std::string& filename)
        : std::runtime_error(std::string("File ") + filename + std::string(" not found")) {}
    ~FileNotFoundException() throw() {}
};

enum class CullMode : GLenum
{
    Front = GL_FRONT,
    Back = GL_BACK
};

enum class Capability : GLenum
{
    AlphaTest = GL_ALPHA_TEST,
    AutoNormal = GL_AUTO_NORMAL,
    Blend = GL_BLEND,
    ClipPlane0 = GL_CLIP_PLANE0,
    ClipPlane1 = GL_CLIP_PLANE1,
    ClipPlane2 = GL_CLIP_PLANE2,
    ClipPlane3 = GL_CLIP_PLANE3,
    ClipPlane4 = GL_CLIP_PLANE4,
    ClipPlane5 = GL_CLIP_PLANE5,
    ColorLogicOp = GL_COLOR_LOGIC_OP,
    ColorMaterial = GL_COLOR_MATERIAL,
    ColorSum = GL_COLOR_SUM,
    ColorTable = GL_COLOR_TABLE,
    Convolution1D = GL_CONVOLUTION_1D,
    Convolution2D = GL_CONVOLUTION_2D,
    CullFace = GL_CULL_FACE,
    DepthTest = GL_DEPTH_TEST,
    Dither = GL_DITHER,
    Fog = GL_FOG,
    Histogram = GL_HISTOGRAM,
    IndexLogicOp = GL_INDEX_LOGIC_OP,
    Light0 = GL_LIGHT0,
    Light1 = GL_LIGHT1,
    Light2 = GL_LIGHT2,
    Light3 = GL_LIGHT3,
    Light4 = GL_LIGHT4,
    Light5 = GL_LIGHT5,
    Light6 = GL_LIGHT6,
    Light7 = GL_LIGHT7,
    Lighting = GL_LIGHTING,
    LineSmooth = GL_LINE_SMOOTH,
    LineStipple = GL_LINE_STIPPLE,
    Map1Color4 = GL_MAP1_COLOR_4,
    Map1Index = GL_MAP1_INDEX,
    Map1Normal = GL_MAP1_NORMAL,
    Map1TextureCoord1 = GL_MAP1_TEXTURE_COORD_1,
    Map1TextureCoord2 = GL_MAP1_TEXTURE_COORD_2,
    Map1TextureCoord3 = GL_MAP1_TEXTURE_COORD_3,
    Map1TextureCoord4 = GL_MAP1_TEXTURE_COORD_4,
    Map1Vertex3 = GL_MAP1_VERTEX_3,
    Map1Vertex4 = GL_MAP1_VERTEX_4,
    Map2Color4 = GL_MAP2_COLOR_4,
    Map2Map2Index = GL_MAP2_INDEX,
    Map2Normal = GL_MAP2_NORMAL,
    Map2TextureCoord1 = GL_MAP2_TEXTURE_COORD_1,
    Map2TextureCoord2 = GL_MAP2_TEXTURE_COORD_2,
    Map2TextureCoord3 = GL_MAP2_TEXTURE_COORD_3,
    Map2TextureCoord4 = GL_MAP2_TEXTURE_COORD_4,
    Map2Vertex3 = GL_MAP2_VERTEX_3,
    Map2Vertex4 = GL_MAP2_VERTEX_4,
    MinMax = GL_MINMAX,
    Multisample = GL_MULTISAMPLE,
    Normalize = GL_NORMALIZE,
    PointSmooth = GL_POINT_SMOOTH,
    PointSprite = GL_POINT_SPRITE,
    PolygonOffsetFill = GL_POLYGON_OFFSET_FILL,
    PolygonOffsetLine = GL_POLYGON_OFFSET_LINE,
    PolygonOffsetPoint = GL_POLYGON_OFFSET_POINT,
    PolygonSmooth = GL_POLYGON_SMOOTH,
    PolygonStipple = GL_POLYGON_STIPPLE,
    PostColorMatrixColorTable = GL_POST_COLOR_MATRIX_COLOR_TABLE,
    PostConvolutionColorTable = GL_POST_CONVOLUTION_COLOR_TABLE,
    RescaleNormal = GL_RESCALE_NORMAL,
    SampleAlphaToCoverage = GL_SAMPLE_ALPHA_TO_COVERAGE,
    SampleAlphaToOne = GL_SAMPLE_ALPHA_TO_ONE,
    SampleCoverage = GL_SAMPLE_COVERAGE,
    Separable2D = GL_SEPARABLE_2D,
    ScissorTest = GL_SCISSOR_TEST,
    StencilTest = GL_STENCIL_TEST,
    Texture1D = GL_TEXTURE_1D,
    Texture2D = GL_TEXTURE_2D,
    Texture3D = GL_TEXTURE_3D,
    TextureCubeMap = GL_TEXTURE_CUBE_MAP,
    TextureGenQ = GL_TEXTURE_GEN_Q,
    TextureGenR = GL_TEXTURE_GEN_R,
    TextureGenS = GL_TEXTURE_GEN_S,
    TextureGenT = GL_TEXTURE_GEN_T,
    VertexProgramPointSize = GL_VERTEX_PROGRAM_POINT_SIZE,
    VertexProgramTwoSide = GL_VERTEX_PROGRAM_TWO_SIDE,
};

class ClearBits
{
public:
	ClearBits();
	ClearBits(const ClearBits&);

    ClearBits(GLbitfield bit)
	 : _bits(bit)
	{}

    ClearBits(ClearBits&& temp)
        : _bits(temp.forward())
    {}

    ~ClearBits()
    {
        if(_bits)
        {
            glClear(_bits);
        }
    }

    GLbitfield forward()
    {
        GLbitfield result = _bits;
        _bits = 0;
        return result;
    }

    ClearBits ColorBuffer()
    {
        return _bits | GL_COLOR_BUFFER_BIT;
    }

    ClearBits DepthBuffer()
    {
        return _bits | GL_DEPTH_BUFFER_BIT;
    }

    ClearBits StencilBuffer()
    {
        return _bits | GL_STENCIL_BUFFER_BIT;
    }

    ClearBits AccumBuffer()
    {
        return _bits | GL_ACCUM_BUFFER_BIT;
    }

private:
    GLbitfield _bits;
};

enum class PrimitiveType : GLenum
{
    Points = GL_POINTS,
    LineStrip = GL_LINE_STRIP,
    LineLoop = GL_LINE_LOOP,
    Lines = GL_LINES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
    Triangles = GL_TRIANGLES,
    QuadStrip = GL_QUAD_STRIP,
    Quads = GL_QUADS,
    Polygon = GL_POLYGON 
};

enum class IndicesType : GLenum
{
    UnsignedByte = GL_UNSIGNED_BYTE,
    UnsignedShort = GL_UNSIGNED_SHORT,
    UnsignedInt = GL_UNSIGNED_INT
};

class Context
{
public:
    Context(SDL_Window* _sdlWindow)
        : _valid(false)
    {
        _glContext = SDL_GL_CreateContext(_sdlWindow);
        glewInit();

        _valid = true;
    }

    ~Context()
    {
    }

    Context(Context&& other)
        : _valid(other._valid)
    {
        other._valid = false;
    }

    Context& operator=(Context&& other)
    {
        _valid = other._valid;
        other._valid = false;

        return *this;
    }

    bool isValid() const { return _valid; }

    void cullFace(CullMode mode)
    {
        glCullFace((GLenum)mode);
    }

    void enable(Capability capability)
    {
        glEnable((GLenum)capability);
    }

    void disable(Capability capability)
    {
        glDisable((GLenum)capability);
    }

    ClearBits clear()
    {
        return ClearBits(0);
    }

    void drawElements(PrimitiveType mode, GLsizei count, IndicesType type, ptrdiff_t location)
    {
        glDrawElements((GLenum)mode, count, (GLenum)type, (const GLvoid*)location);
    }

    void drawArrays(PrimitiveType mode, GLint first, GLsizei count)
    {
        glDrawArrays((GLenum)mode, first, count);
    }

private:
    Context(const Context&);
    Context& operator=(const Context&);

    bool _valid;

    SDL_GLContext _glContext;
};

enum class ShaderType : GLenum
{
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER
};

template<typename Type>
class DeleterBase
{
public:
    virtual void delete_(const Type& v) = 0;
    virtual ~DeleterBase() {}
};

template<typename Type, typename Deleter>
class DeleterImpl : public DeleterBase<Type>
{
public:
    DeleterImpl(Deleter d)
      : _d(d) {}
    ~DeleterImpl() {}
    
    void delete_(const Type& v) { _d(v); }

    Deleter _d;
};

template<typename Type>
class Resource
{
public:
    Resource() : _di(nullptr), _v(NULL) {}

    template<typename Deleter>
    Resource(const Type& v, Deleter d)
        : _v(v)
    {
        DeleterBase<Type>* p = new DeleterImpl<Type, Deleter>(d);
        _di = std::unique_ptr<DeleterBase<Type>>(p);
    }
    ~Resource()
    {
        if(_di)
        {
            _di->delete_(_v);
        }
    }

    Resource(Resource&& other)
        : _di(other._di), _v(other._v)
    {
        other._di = nullptr;
    }

    Resource& operator=(Resource&& other)
    {
        _di = std::move(other._di);
        _v = std::move(other._v);
        other._di = nullptr;
        other._v = 0;
        return *this;
    }

    operator const Type&()
    {
        return _v;
    }

    const Type& get() const
    { 
        return _v;
    }

private:
    Resource& operator=(const Resource& other);
    Resource(const Resource& other);

    Type _v;
    std::unique_ptr<DeleterBase<Type>> _di;
};

template<ShaderType type>
class Shader
{
public:
    Shader()
        : _ok(false)
    {
    }
    ~Shader() 
    {
    }

    void fromSource(const std::string& source)
    {
        _shaderResource = Resource<GLuint>(glCreateShader((GLenum)type), glDeleteShader);

        const GLchar* strings[] = { (const GLchar*)source.c_str() };
        GLint lengths[] = { (GLint)source.size() };
        glShaderSource(_shaderResource, 1, strings, lengths);
    }

    void fromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if(!file.is_open())
            throw FileNotFoundException(filename);

        std::stringstream buffer;
        buffer << file.rdbuf();

        fromSource(buffer.str());
    }

    void compile()
    {
        glCompileShader(_shaderResource);
        
        GLint ok = GL_FALSE;
        glGetShaderiv(_shaderResource, GL_COMPILE_STATUS, &ok);

        GLint msgLen = 0;
        glGetShaderiv(_shaderResource, GL_INFO_LOG_LENGTH, &msgLen);

        std::vector<GLchar> msg(msgLen + 1);
        glGetShaderInfoLog(_shaderResource, msgLen, NULL, msg.data());

        //std::cout << (char*)msg.data() << std::endl;

        if(!ok)
            throw GLCompileError(msg.data());

        _ok = ok!=0;
    }

    bool isOk() const { return _ok; }

    GLuint getResource() const { return _shaderResource.get(); }
private:
    bool _ok;
    Resource<GLuint> _shaderResource;
};

typedef Shader<ShaderType::Vertex> VertexShader;
typedef Shader<ShaderType::Fragment> FragmentShader;

class Program
{
public:
    Program() : _ok(false) {}
    
    void create()
    {
        _programResource = Resource<GLuint>(glCreateProgram(), glDeleteProgram);
    }

    template<ShaderType type>
    void attachShader(const Shader<type>& shader)
    {
        glAttachShader(_programResource, shader.getResource());
    }

    void link()
    {
        glLinkProgram(_programResource);

        GLint ok = GL_FALSE;
        glGetProgramiv(_programResource, GL_LINK_STATUS, &ok);

        GLint msgLen = 0;
        glGetProgramiv(_programResource, GL_INFO_LOG_LENGTH, &msgLen);

        std::vector<GLchar> msg(msgLen + 1);
        glGetProgramInfoLog(_programResource, msgLen, NULL, &msg[0]);

        if(!ok)
            throw GLCompileError(msg.data());

        _ok = ok!=0;
    }

    void use()
    {
        glUseProgram(_programResource);
    }

    GLuint getResource() const { return _programResource.get(); }

    std::vector<std::string> listUniforms()
    {
        std::vector<std::string> list;

        int count = 0;
        glGetObjectParameterivARB(_programResource, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &count);
        for(int i=0; i<count; ++i)
        {
            char buf[256];
            int length;
            int size;
            GLenum type;
            glGetActiveUniform(_programResource, i, 256, &length, &size, &type, buf);
            list.push_back(buf);
        }

        return list;
    }
private:
    bool _ok;
    Resource<GLuint> _programResource;
};

template<class Type>
class Uniform
{
public:
    Uniform(const Program& program, const std::string& name)
    {
        location = glGetUniformLocation(program.getResource(), name.c_str());
        /*if(location == -1)
        {
            throw GLException("Name of uniform '" + name + "' does not correspond to an uniform variable in the active program.");
        }*/
    }

    ~Uniform()
    {
    }

    void set(const Type& v);

private:
    GLuint location;
};

#define UNIFORM_HELPER(SHORT_TYPE, TYPE, GLTYPE, COUNT) \
    template<> \
    inline void Uniform<TYPE>::set(const TYPE& v) \
    { \
        glUniform##COUNT##SHORT_TYPE##v(location, 1, (const GLTYPE*)&v); \
    } 

UNIFORM_HELPER(i, int, GLint, 1);
UNIFORM_HELPER(i, Vec2i, GLint, 2);
UNIFORM_HELPER(i, Vec3i, GLint, 3);
UNIFORM_HELPER(i, Vec4i, GLint, 4);

UNIFORM_HELPER(f, float, GLfloat, 1);
UNIFORM_HELPER(f, Vec2f, GLfloat, 2);
UNIFORM_HELPER(f, Vec3f, GLfloat, 3);
UNIFORM_HELPER(f, Vec4f, GLfloat, 4);

UNIFORM_HELPER(d, double, GLdouble, 1);
UNIFORM_HELPER(d, Vec2d, GLdouble, 2);
UNIFORM_HELPER(d, Vec3d, GLdouble, 3);
UNIFORM_HELPER(d, Vec4d, GLdouble, 4);

/*#define UNIFORMMAT_HELPER(SHORT_TYPE, TYPE, GLTYPE, M)   \
    template<> \
    inline void Uniform<TYPE>::set(const TYPE& mat) \
    { \
        glUniformMatrix##M##SHORT_TYPE##v(location, M*M, GL_FALSE, (GLTYPE*)&mat); \
    }
#define UNIFORMMAT_HELPER2(SHORT_TYPE, TYPE, GLTYPE, M, N)   \
    template<> \
    inline void Uniform<TYPE>::set(const TYPE& mat) \
    { \
        glUniformMatrix##N##x##M##fv(location, M*N, GL_FALSE, (GLTYPE*)&mat); \
    }

UNIFORMMAT_HELPER (f, Matrix2x2f, GLfloat, 2);
UNIFORMMAT_HELPER2(f, Matrix2x3f, GLfloat, 2, 3);
UNIFORMMAT_HELPER2(f, Matrix2x4f, GLfloat, 2, 4);
UNIFORMMAT_HELPER2(f, Matrix3x2f, GLfloat, 3, 2);
UNIFORMMAT_HELPER (f, Matrix3x3f, GLfloat, 3);
UNIFORMMAT_HELPER2(f, Matrix3x4f, GLfloat, 3, 4);
UNIFORMMAT_HELPER2(f, Matrix4x2f, GLfloat, 4, 2);
UNIFORMMAT_HELPER2(f, Matrix4x3f, GLfloat, 4, 3);
UNIFORMMAT_HELPER (f, Matrix4x4f, GLfloat, 4);*/

enum class Target : GLenum
{
    Array = GL_ARRAY_BUFFER,
    ElementArray = GL_ELEMENT_ARRAY_BUFFER,
    PixelPackBuffer = GL_PIXEL_PACK_BUFFER,
    PixelUnpackBuffer = GL_PIXEL_UNPACK_BUFFER
};

enum class Usage : GLenum
{
    StreamDraw = GL_STREAM_DRAW, 
    StreamRead = GL_STREAM_READ, 
    StreamCopy = GL_STREAM_COPY, 
    StaticDraw = GL_STATIC_DRAW, 
    StaticRead = GL_STATIC_READ, 
    StaticCopy = GL_STATIC_COPY, 
    DynamicDraw = GL_DYNAMIC_DRAW, 
    DynamicRead = GL_DYNAMIC_READ, 
    DynamicCopy = GL_DYNAMIC_COPY
};

template<Target TARGET>
class Buffer
{
public:
    Buffer() {}

    void gen()
    {
        GLuint buffer;
        glGenBuffers(1, &buffer);
        _bufferResource = Resource<GLuint>(buffer, [](const GLuint& v) {
            glDeleteBuffers(1, &v);
        });
    }

    void bind()
    {
        glBindBuffer((GLenum)TARGET, _bufferResource);
    }

    static void bufferData(size_t size, const void* data, Usage usage)
    {
        glBufferData((GLenum)TARGET, size, data, (GLenum)usage);
    }

    template<class TYPE>
    static void bufferData(const std::vector<TYPE>& data, Usage usage)
    {
        glBufferData((GLenum)TARGET, data.size()*sizeof(TYPE), data.data(), (GLenum)usage);
    }

    GLuint getResource() const { return _bufferResource.get(); }

private:
    Resource<GLuint> _bufferResource;
};

typedef Buffer<Target::Array> ArrayBuffer;
typedef Buffer<Target::ElementArray> ElementArrayBuffer;
typedef Buffer<Target::PixelPackBuffer> PixelPackBuffer;
typedef Buffer<Target::PixelUnpackBuffer> PixelUnpackBuffer;

enum class Type : GLenum
{
    Byte = GL_BYTE,
    UnsignedByte = GL_UNSIGNED_BYTE,
    Short = GL_SHORT,
    UnsignedShort = GL_UNSIGNED_SHORT,
    Int = GL_INT,
    UnsignedInt = GL_UNSIGNED_INT,
    Float = GL_FLOAT,
    Double = GL_DOUBLE 
};

class VertexAttribArray
{
public:
    VertexAttribArray(const Program& program, const std::string& name)
    {
        location = glGetAttribLocation(program.getResource(), name.c_str());
    }

    void setupPointer(int size = 4, Type type = Type::Float, bool normalized = false, int stride = 0, ptrdiff_t pointer = 0)
    {
        glVertexAttribPointer(location, size, (GLenum)type, normalized?GL_TRUE:GL_FALSE, stride, (const GLvoid*)pointer);
    }

    template<typename TYPE>
    void setupPointer(bool normalized = false, int stride = 0, ptrdiff_t pointer = 0);

    void enable() const
    {
        glEnableVertexAttribArray(location);
    }

    void disable() const
    {
        glDisableVertexAttribArray(location);
    }

private:
    GLuint location;
};
#define VERTEXATTRIBARRAY_HELPER(TYPE, NUM, GLTYPE) \
    template<> \
    inline void VertexAttribArray::setupPointer<TYPE>(bool normalized, int stride, ptrdiff_t pointer) \
    { \
        glVertexAttribPointer(location, NUM, GLTYPE, normalized?GL_TRUE:GL_FALSE, stride, (const GLvoid*)pointer); \
    }
VERTEXATTRIBARRAY_HELPER(Vec2i, 2, GL_INT);
VERTEXATTRIBARRAY_HELPER(Vec2f, 2, GL_FLOAT);
VERTEXATTRIBARRAY_HELPER(Vec2d, 2, GL_DOUBLE);
VERTEXATTRIBARRAY_HELPER(Vec3i, 3, GL_INT);
VERTEXATTRIBARRAY_HELPER(Vec3f, 3, GL_FLOAT);
VERTEXATTRIBARRAY_HELPER(Vec3d, 3, GL_DOUBLE);
VERTEXATTRIBARRAY_HELPER(Vec4i, 4, GL_INT);
VERTEXATTRIBARRAY_HELPER(Vec4f, 4, GL_FLOAT);
VERTEXATTRIBARRAY_HELPER(Vec4d, 4, GL_DOUBLE);
    
class EnableVertexAttribArray
{
public:
    EnableVertexAttribArray(const VertexAttribArray& vertexAttribArray)
        : _vertexAttribArray(vertexAttribArray)
    {
        _vertexAttribArray.enable();
    }

    ~EnableVertexAttribArray()
    {
        _vertexAttribArray.disable();
    }

private:
    const VertexAttribArray& _vertexAttribArray;
};

enum class TextureTarget : GLenum
{
    Texture1D = GL_TEXTURE_1D,
    Texture2D = GL_TEXTURE_2D,
    Texture3D = GL_TEXTURE_3D,
    TextureCubeMap = GL_TEXTURE_CUBE_MAP
};

enum class TextureTarget1D : GLenum
{
    Texture1D = GL_TEXTURE_1D,
    ProxyTexture1D = GL_PROXY_TEXTURE_1D
};

enum class TextureTarget2D : GLenum
{
    Texture2D = GL_TEXTURE_2D,
    ProxyTexture2D = GL_PROXY_TEXTURE_2D,
    TextureCubeMapPositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    TextureCubeMapNegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    TextureCubeMapPositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    TextureCubeMapNegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    TextureCubeMapPositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    TextureCubeMapNegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    ProxyTextureCubeMap = GL_PROXY_TEXTURE_CUBE_MAP
};

enum class TextureTarget3D : GLenum
{
    Texture3D = GL_TEXTURE_3D,
    ProxyTexture3D = GL_PROXY_TEXTURE_3D,
};

enum class ImageInternalFormat : GLint
{
    One = 1,
    Two = 2,
    Three = 3,
    Four = 4,
    Alpha = GL_ALPHA,
    Alpha4 = GL_ALPHA4,
    Alpha8 = GL_ALPHA8,
    Alpha12 = GL_ALPHA12,
    Alpha16 = GL_ALPHA16,
    CompressedAlpha = GL_COMPRESSED_ALPHA,
    CompressedLuminance = GL_COMPRESSED_LUMINANCE,
    CompressedLuminanceAlpha = GL_COMPRESSED_LUMINANCE_ALPHA,
    CompressedIntensity = GL_COMPRESSED_INTENSITY,
    CompresedRGB = GL_COMPRESSED_RGB,
    CompressedRGBA = GL_COMPRESSED_RGBA,
    DepthComponent = GL_DEPTH_COMPONENT,
    DepthComponent16 = GL_DEPTH_COMPONENT16,
    DepthComponent24 = GL_DEPTH_COMPONENT24,
    DepthComponent32 = GL_DEPTH_COMPONENT32,
    Luminance = GL_LUMINANCE,
    Luminance4 = GL_LUMINANCE4,
    Luminance8 = GL_LUMINANCE8,
    Luminance12 = GL_LUMINANCE12,
    Luminance16 = GL_LUMINANCE16,
    LuminanceAlpha = GL_LUMINANCE_ALPHA,
    Luminance4Alpha4 = GL_LUMINANCE4_ALPHA4,
    Luminance6Alpha2 = GL_LUMINANCE6_ALPHA2,
    Luminance8Alpha8 = GL_LUMINANCE8_ALPHA8,
    Luminance12Alpha4 = GL_LUMINANCE12_ALPHA4,
    Luminance12Alpha12 = GL_LUMINANCE12_ALPHA12,
    Luminance16Alpha16 = GL_LUMINANCE16_ALPHA16,
    Intensity = GL_INTENSITY,
    Intensity4 = GL_INTENSITY4,
    Intensity8 = GL_INTENSITY8,
    Intensity12 = GL_INTENSITY12,
    Intensity16 = GL_INTENSITY16,
    R3G3B2 = GL_R3_G3_B2,
    RGB = GL_RGB,
    RGB4 = GL_RGB4,
    RGB5 = GL_RGB5,
    RGB8 = GL_RGB8,
    RGB10 = GL_RGB10,
    RGB12 = GL_RGB12,
    RGB16 = GL_RGB16,
    RGBA = GL_RGBA,
    RGBA2 = GL_RGBA2,
    RGBA4 = GL_RGBA4,
    RGB5A1 = GL_RGB5_A1,
    RGBA8 = GL_RGBA8,
    RGBA10A2 = GL_RGB10_A2,
    RGBA12 = GL_RGBA12,
    RGBA16 = GL_RGBA16,
    SLuminance = GL_SLUMINANCE,
    SLuminance8 = GL_SLUMINANCE8,
    SLuminanceAlpha = GL_SLUMINANCE_ALPHA,
    SLuminanceAlpha8 = GL_SLUMINANCE8_ALPHA8,
    SRGB = GL_SRGB,
    SRGB8 = GL_SRGB8,
    SRGBAlpha = GL_SRGB_ALPHA,
    SRGB8Alpha8 = GL_SRGB8_ALPHA8,
    RGBA32F = GL_RGBA32F,
    R32F = GL_R32F
};

enum class ImageFormat : GLint
{
    ColorIndex = GL_COLOR_INDEX,
    Red = GL_RED,
    Green = GL_GREEN,
    Blue = GL_BLUE,
    Alpha = GL_ALPHA,
    RGB = GL_RGB,
    BGR = GL_BGR,
    RGBA = GL_RGBA,
    BGRA = GL_BGRA,
    Luminance = GL_LUMINANCE,
    LuminanceAlpha = GL_LUMINANCE_ALPHA
};

enum class ImageType : GLint
{
    UnsignedByte = GL_UNSIGNED_BYTE,
    Byte = GL_BYTE,
    Bitmap = GL_BITMAP,
    UnsignedShort = GL_UNSIGNED_SHORT,
    Short = GL_SHORT,
    UnsignedInt = GL_UNSIGNED_INT,
    Int = GL_INT,
    Float = GL_FLOAT,
    UnsignedByte332 = GL_UNSIGNED_BYTE_3_3_2,
    UnsignedByte233Rev = GL_UNSIGNED_BYTE_2_3_3_REV,
    UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,
    UnsignedShort565Rev = GL_UNSIGNED_SHORT_5_6_5_REV,
    UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,
    UnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
    UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,
    UnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
    UnsignedInt8888 = GL_UNSIGNED_INT_8_8_8_8,
    UnsignedInt8888Rev = GL_UNSIGNED_INT_8_8_8_8_REV,
    UnsignedInt1010102 = GL_UNSIGNED_INT_10_10_10_2,
    UnsignedInt2101010 = GL_UNSIGNED_INT_2_10_10_10_REV
};

enum class Border : GLint
{
    Zero = 0,
    One = 1
};

template<typename T, int DIM>
class TextureImage
{
public:
    TextureImage(size_t width, T* data, ImageType type, ImageFormat format, ImageInternalFormat internalFormat)
        : _data(data), _format(format), _internalFormat(internalFormat), _type(type)
    {
        _dims[0] = width;
    }

    TextureImage(size_t width, size_t height, T* data, ImageType type, ImageFormat format, ImageInternalFormat internalFormat)
        : _data(data), _format(format), _internalFormat(internalFormat), _type(type)
    {
        _dims[0] = width;
        _dims[1] = height;
    }

    TextureImage(size_t width, size_t height, size_t depth, T* data, ImageType type, ImageFormat format, ImageInternalFormat internalFormat)
        : _data(data), _format(format), _internalFormat(internalFormat), _type(type)
    {
        _dims[0] = width;
        _dims[1] = height;
        _dims[2] = depth;
    }

    ImageInternalFormat getInternalFormat() const { return _internalFormat; }
    ImageFormat getFormat() const { return _format; }
    ImageType getType() const { return _type; }

    size_t getWidth() const { return _dims[0]; }
    size_t getHeight() const { return _dims[1]; }
    size_t getDepth() const { return _dims[2]; }

    T* getData() const { return _data; }
private:
    ImageInternalFormat _internalFormat;
    ImageFormat _format;
    ImageType _type;
    size_t _dims[DIM];
    T* _data;
};

typedef TextureImage<Vec4f, 1> TextureImage1DRGBAF;
typedef TextureImage<Vec4ub, 2> TextureImage2DRGBA;
typedef TextureImage<float, 3> TextureImage3DAlpha;

enum class TextureMinFilter : GLint
{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
};

enum class TextureMagFilter : GLint
{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
};

enum class WrapMode : GLint
{
    Clamp = GL_CLAMP,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    Repeat = GL_REPEAT,
};

enum class CompareMode : GLint
{
    CompareRToTexture = GL_COMPARE_R_TO_TEXTURE,
    None = GL_NONE
};

enum class CompareFunc : GLint
{
    LEqual = GL_LEQUAL,
    GEqueal = GL_GEQUAL,
    Less = GL_LESS,
    Greater = GL_GREATER,
    Equal = GL_EQUAL,
    NotEqual = GL_NOTEQUAL,
    Always = GL_ALWAYS,
    Never = GL_NEVER
};

enum class DepthTextureMode : GLint
{
    DepthTextureMode = GL_LUMINANCE,
    Intensity = GL_INTENSITY,
    Alpha = GL_ALPHA
};

template<TextureTarget TARGET>
class TextureTargetParameters
{
public:

    TextureTargetParameters<TARGET>& minFilter(TextureMinFilter value)
    {
        glTexParameteri((GLenum)TARGET, GL_TEXTURE_MIN_FILTER, (GLint)value);
        return *this;
    }

    TextureTargetParameters<TARGET>& magFilter(TextureMagFilter value)
    {
        glTexParameteri((GLenum)TARGET, GL_TEXTURE_MAG_FILTER, (GLint)value);
        return *this;
    }

    TextureTargetParameters<TARGET>& minLod(GLfloat value)
    {
        glTexParameterf((GLenum)TARGET, GL_TEXTURE_MIN_LOD, value);
        return *this;
    }

    TextureTargetParameters<TARGET>& maxLod(GLfloat value)
    {
        glTexParameterf((GLenum)TARGET, GL_TEXTURE_MAX_LOD, value);
        return *this;
    }

    TextureTargetParameters<TARGET>& baseLevel(GLint value)
    {
        glTexParameteri((GLenum)TARGET, GL_TEXTURE_BASE_LEVEL, value);
        return *this;
    }

    TextureTargetParameters<TARGET>& maxLevel(GLint value)
    {
        glTexParameteri((GLenum)TARGET, GL_TEXTURE_MAX_LEVEL, value);
        return *this;
    }

    TextureTargetParameters<TARGET>& wrapS(WrapMode value)
    {
        glTexParameteri((GLenum)TARGET, GL_TEXTURE_WRAP_S, (GLint)value);
        return *this;
    }

    TextureTargetParameters<TARGET>& wrapT(WrapMode value)
    {
        glTexParameteri((GLenum)TARGET, GL_TEXTURE_WRAP_T, (GLint)value);
        return *this;
    }

    TextureTargetParameters<TARGET>& wrapR(WrapMode value)
    {
        glTexParameteri((GLenum)TARGET, GL_TEXTURE_WRAP_R, (GLint)value);
        return *this;
    }

    TextureTargetParameters<TARGET>& borderColor(const Vec4f& value)
    {
        glTexParameterfv((GLenum)TARGET, GL_TEXTURE_BORDER_COLOR, &value[0]);
        return *this;
    }

    TextureTargetParameters<TARGET>& borderColor(const Vec4i& value)
    {
        glTexParameteriv((GLenum)TARGET, GL_TEXTURE_BORDER_COLOR, &value[0]);
        return *this;
    }
    
    TextureTargetParameters<TARGET>& priority(GLfloat value)
    {
        glTexParameterf((GLenum)TARGET, GL_TEXTURE_PRIORITY, value);
        return *this;
    }
    
    TextureTargetParameters<TARGET>& compareMode(CompareMode value)
    {
        glTexParameteri((GLenum)TARGET, GL_TEXTURE_COMPARE_MODE, (GLint)value);
        return *this;
    }

    TextureTargetParameters<TARGET>& compareFunc(CompareFunc value)
    {
        glTexParameteri((GLenum)TARGET, GL_TEXTURE_COMPARE_FUNC, (GLint)value);
        return *this;
    }

    TextureTargetParameters<TARGET>& depthTextureMode(DepthTextureMode value)
    {
        glTexParameteri((GLenum)TARGET, GL_DEPTH_TEXTURE_MODE, (GLint)value);
        return *this;
    }

    TextureTargetParameters<TARGET>& generateMipMap(bool value)
    {
        glTexParameteri((GLenum)TARGET, GL_GENERATE_MIPMAP, value?GL_TRUE:GL_FALSE);
        return *this;
    }
};

template<int DIM, TextureTarget TARGET>
class Texture
{
public:
    Texture()
    {
    }
    ~Texture()
    {
    }

    void gen()
    {
        GLuint tex;
        glGenTextures(1, &tex);
        _textureResource = Resource<GLuint>(tex, [](const GLuint& v) {
            glDeleteTextures(1, &v);
        });
    }

    TextureTargetParameters<TARGET> bind()
    {
        glBindTexture((GLenum)TARGET, _textureResource);
        return parameters();
    }

    TextureTargetParameters<TARGET> parameters()
    {
        return TextureTargetParameters<TARGET>();
    }

    static void active(int tex)
    {
        glActiveTexture(GL_TEXTURE0 + tex);
    }

    template<typename T>
    static void texImage(TextureTarget1D target, GLint level, Border border, TextureImage<T, DIM>& image)
    {
        glTexImage1D((GLenum)target, level, (GLint)image.getInternalFormat(), image.getWidth(), (GLint)border, (GLenum)image.getFormat(), (GLenum)image.getType(), image.getData());
    }

    template<typename T>
    static void texImage(TextureTarget2D target, GLint level, Border border, TextureImage<T, DIM>& image)
    {
        glTexImage2D((GLenum)target, level, (GLint)image.getInternalFormat(), image.getWidth(), image.getHeight(), (GLint)border, (GLenum)image.getFormat(), (GLenum)image.getType(), image.getData());
    }

    template<typename T>
    static void texImage(TextureTarget3D target, GLint level, Border border, TextureImage<T, DIM>& image)
    {
        glTexImage3D((GLenum)target, level, (GLint)image.getInternalFormat(), image.getWidth(), image.getHeight(), image.getDepth(), (GLint)border, (GLenum)image.getFormat(), (GLenum)image.getType(), image.getData());
        GLenum err = glGetError();
    }

    template<typename T>
    static void texSubImage(TextureTarget2D target, GLint level, GLint x, GLint y, GLint width, GLint height, Border border, TextureImage<T, DIM>& image)
    {
        glTexSubImage2D((GLenum)target, level, x, y, width, height, (GLenum)image.getFormat(), (GLenum)image.getType(), image.getData());
    }

    template<typename T>
    static void texSubImage(TextureTarget2D target, GLint level, GLint x, GLint y, GLint z, GLint width, GLint height, GLint depth, Border border, TextureImage<T, DIM>& image)
    {
        glTexSubImage3D((GLenum)target, level, x, y, z, width, height, depth, (GLenum)image.getFormat(), (GLenum)image.getType(), image.getData());
    }


private:
    Resource<GLuint> _textureResource;
};

typedef Texture<1, TextureTarget::Texture1D> Texture1D;
typedef Texture<2, TextureTarget::Texture2D> Texture2D;
typedef Texture<3, TextureTarget::Texture3D> Texture3D;

class Window
{
public:
    Window(const std::string& windowTitle, int width, int height, bool fullscreen)
        : _windowTitle(windowTitle)
    {
        if(SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            throw SDLException("SDL_Init failed");
        }

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        _mainWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (fullscreen?SDL_WINDOW_FULLSCREEN:0));

        _t0 = 0;
        _t1 = 0;
        _frame = 0;
    }

    virtual ~Window()
    {
        SDL_Quit();
    }

    Context createContext(bool vsync)
    {
        Context ctx(_mainWindow);
        SDL_GL_SetSwapInterval(vsync?1:0);  // 1 for vsync
        return ctx;
    }

    void swap()
    {
	    SDL_GL_SwapWindow(_mainWindow);

        _t1 = SDL_GetTicks();
        _frame++;
        if(_t1 - _t0 > 500.f)
        {
            float fps = 1000.f / (_t1 - _t0) * _frame;
            _t0 = _t1;
            _frame = 0;
            
            char title[256];
            _snprintf(title, 256, "%g fps - %s", fps, _windowTitle.c_str());
            SDL_SetWindowTitle(_mainWindow, title);
        }
    }

private:
    SDL_Window* _mainWindow;
    std::string _windowTitle;

    unsigned int _t0;
    unsigned int _t1;
    int _frame;
};

#endif
