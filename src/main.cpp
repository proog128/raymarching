#include <iostream>
#include <functional>
#include "GLWrapper.h"
#include "FSUtils.h"
#include "Matrix4x4.h"
#include "World.h"

static bool gRunning = true;
static int gResX = 1024;
static int gResY = 768;
static bool gFullscreen = false;
static bool gVsync = false;
static float gSpeed = 0.002f;

static const int EVENT_RELOADSHADERCODE = 0;

static const Vec4f gVertexBufferData[] = 
{
    Vec4f(-1.f, -1.f, 0.f, 0.f),
    Vec4f(-1.f,  1.f, 0.f, 0.f),
    Vec4f( 1.f,  1.f, 0.f, 0.f),
    Vec4f( 1.f, -1.f, 0.f, 0.f),
};

static const GLuint gElementBufferData[] = {0, 1, 2, 3};

struct Resources
{
    VertexShader vs;
    FragmentShader fs;

    Program p;

    ArrayBuffer vertexBuffer;
    ElementArrayBuffer elementBuffer;

    Texture3D worldTex;
    Texture1D hgTex;
    Texture2D debugImgTex;

    Vec3i worldSize;
} gResources;

struct Camera
{
    Camera(const Vec3f& pos, const Vec3f& up, const Vec3f& dir, float fovy, int screenWidth, int screenHeight)
        : pos(pos),
          up(up),
          dir(dir)
    {
        float focalLength = 1.f;

        float aspect = (float)screenWidth / (float)screenHeight;

        float height = focalLength / (2*tan(fovy/2.f));
        float width = aspect * height;

        stepX = width / screenWidth;
        stepY = height / screenHeight;
    }

    ~Camera()
    {
    }

    void moveForward(float dt)
    {
        pos += dir * dt * 0.25f;
    }

    void moveBackward(float dt)
    {
        pos -= dir * dt * 0.25f;
    }

    void moveLeft(float dt)
    {
        pos -= dir.cross(up) * dt * 0.25f;
    }

    void moveRight(float dt)
    {
        pos += dir.cross(up) * dt * 0.25f;
    }

    void rotateX(float dt)
    {
        Matrix4x4f a = Matrix4x4f::Identity()
                       .RotateY(dt*0.01f);
        up = a*up;
        dir = a*dir;
    }

    void rotateY(float dt)
    {
        Matrix4x4f a = Matrix4x4f::Identity()
                       .Rotate(-dt*0.01f, dir.cross(up));
        Vec3f newUp = a*up;
        if(newUp.dot(Vec3f(0.f,1.f,0.f)) > 0.f)
        {
            up = newUp;
            dir = a*dir;
        }
    }

    Vec3f pos;
    Vec3f up;
    Vec3f dir;

    float stepX;
    float stepY;
} gCamera(Vec3f(0.0610065f, 0.0558378f, 0.150712f),
          Vec3f(0.114496f, 0.966392f, -0.230176f),
          Vec3f(0.430408f, 0.257082f, 0.865253f),
          45.f/180.f*(float)M_PI,
          gResX, gResY);

void InitWorld()
{
    auto distanceField = buildWorld();

    gResources.worldTex.gen();

    Texture3D::active(0);
    gResources.worldTex.bind()
        .minFilter(TextureMinFilter::Linear)
        .magFilter(TextureMagFilter::Linear)
        .wrapS(WrapMode::MirroredRepeat)
        .wrapT(WrapMode::Clamp)
        .wrapR(WrapMode::MirroredRepeat)
        .generateMipMap(false);

    gResources.worldSize = Vec3i(distanceField.width, distanceField.height, distanceField.depth);
    TextureImage3DAlpha texImg(distanceField.width, distanceField.height, distanceField.depth, distanceField.data.data(), ImageType::Float, ImageFormat::Red, ImageInternalFormat::R32F);
    Texture3D::texImage(TextureTarget3D::Texture3D, 0, Border::Zero, texImg);
}

void InitShaders()
{
    try
    {
        gResources.vs.fromFile("data/shader/shader.vert");
        gResources.vs.compile();

        gResources.fs.fromFile("data/shader/shader.frag");
        gResources.fs.compile();

        gResources.p.create();
        gResources.p.attachShader(gResources.vs);
        gResources.p.attachShader(gResources.fs);
        gResources.p.link();
        gResources.p.use();

        Uniform<float>(gResources.p, "screenWidth").set((float)gResX);
        Uniform<float>(gResources.p, "screenHeight").set((float)gResY);

        Uniform<int>(gResources.p, "debugImgTex").set(2);

        Uniform<int>(gResources.p, "worldTex").set(0);
        Uniform<Vec3i>(gResources.p, "worldSize").set(gResources.worldSize);
    }
    catch(const GLCompileError& ex)
    {
        std::cout << ex.what();
    }
}

void Init(Context& gl)
{
    gResources.vertexBuffer.gen();
    gResources.vertexBuffer.bind();
    ArrayBuffer::bufferData(sizeof(gVertexBufferData), gVertexBufferData, Usage::StaticDraw);

    gResources.elementBuffer.gen();
    gResources.elementBuffer.bind();
    ElementArrayBuffer::bufferData(sizeof(gElementBufferData), gElementBufferData, Usage::StaticDraw);

    InitWorld();
    InitShaders();
}

void Render(Context& gl)
{
    gl.clear().ColorBuffer();

    gResources.p.use();

    Uniform<Vec3f>(gResources.p, "cameraPos").set(gCamera.pos);
    Uniform<Vec3f>(gResources.p, "cameraUp").set(gCamera.up);
    Uniform<Vec3f>(gResources.p, "cameraDir").set(gCamera.dir);
    Uniform<float>(gResources.p, "stepX").set(gCamera.stepX);
    Uniform<float>(gResources.p, "stepY").set(gCamera.stepY);

    Texture1D::active(0);
    gResources.worldTex.bind();

    Texture1D::active(1);
    gResources.hgTex.bind();

    gResources.vertexBuffer.bind();

    VertexAttribArray attribPosition(gResources.p, "position");
    attribPosition.setupPointer<Vec4f>();
    attribPosition.enable();

    gResources.elementBuffer.bind();
    gl.drawElements(PrimitiveType::Quads, 4, IndicesType::UnsignedInt, 0);

    attribPosition.disable();
}

void Move()
{
    Uint8* keystate = SDL_GetKeyboardState(NULL);
    if(keystate[SDL_SCANCODE_W])
    {
        gCamera.moveForward(gSpeed);
    }
    else if(keystate[SDL_SCANCODE_S])
    {
        gCamera.moveBackward(gSpeed);
    }
    if(keystate[SDL_SCANCODE_A])
    {
        gCamera.moveLeft(gSpeed);
    }
    else if(keystate[SDL_SCANCODE_D])
    {
        gCamera.moveRight(gSpeed);
    }
}

void OnKeyDown(const SDL_KeyboardEvent& event)
{
    switch(event.keysym.sym)
    {
    case SDLK_TAB:
        std::cout << "\n"
                  << "pos = " << gCamera.pos << "\n"
                  << "dir = " << gCamera.dir << "\n"
                  << "up =  " << gCamera.up  << "\n"
                  << "dir x up =  " << gCamera.dir.cross(gCamera.up)  << "\n";
        break;
    case SDLK_ESCAPE: 
        gRunning = false; 
        break;
    case SDLK_PLUS:
        gSpeed *= 2.f;
        break;
    case SDLK_MINUS:
        gSpeed /= 2.f;
        break;
    default: 
        break;
    }
}

void OnKeyUp(const SDL_KeyboardEvent& event)
{
}

void OnMouseMotion(const SDL_MouseMotionEvent& event)
{
    if(event.state & SDL_BUTTON(SDL_BUTTON_LEFT) || gFullscreen)
    {
        gCamera.rotateX((float)event.xrel);
        gCamera.rotateY((float)event.yrel);
    }
}

void OnMouseButtonDown(const SDL_MouseButtonEvent& event)
{
}

void OnMouseButtonUp(const SDL_MouseButtonEvent& event)
{
}

bool HandleEvents()
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_KEYDOWN:
            OnKeyDown(event.key);
            break;
        case SDL_KEYUP:
            OnKeyUp(event.key);
            break;
        case SDL_MOUSEMOTION:
            OnMouseMotion(event.motion);
            break;
        case SDL_MOUSEBUTTONDOWN:
            OnMouseButtonDown(event.button);
            break;
        case SDL_MOUSEBUTTONUP:
            OnMouseButtonUp(event.button);
            break;
        case SDL_QUIT:
            return false;
        case SDL_USEREVENT:
            if(event.user.code == EVENT_RELOADSHADERCODE)
            {
                std::cout << "Change in shader code detected. Reloading...\n";
                InitShaders();
            }
            break;
        default:
            break;
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    if(argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")))
    {
        std::cout << "Usage: " << argv[0] << " <width> <height> <fullscreen> <vsync>\n";
        return 0;
    }

    if(argc >= 4)
    {
        gResX = atoi(argv[1]);
        gResY = atoi(argv[2]);
        gFullscreen = atoi(argv[3]);
        if(argc >= 5)
        {
            gVsync = atoi(argv[4]);
        }
    }

    std::cout << "Keys:\n";
    std::cout << "  WASD  Movement\n";
    std::cout << "  +     Increase speed\n";
    std::cout << "  -     Decrease speed\n";
    std::cout << "  Tab   Print position\n";
    std::cout << "  Esc   Quit\n";
    std::cout << std::endl;

    try
    {
        Window wnd("RayMarching", gResX, gResY, gFullscreen);
        Context gl = wnd.createContext(gVsync);

        if(gFullscreen)
            SDL_ShowCursor(0);

        Init(gl);

        WatchFile("data/shader/shader.vert", CreateSDLEventFunc<EVENT_RELOADSHADERCODE>);
        WatchFile("data/shader/shader.frag", CreateSDLEventFunc<EVENT_RELOADSHADERCODE>);

        while(gRunning)
        {
            if(!HandleEvents())
                break;

            Move();
            Render(gl);

            wnd.swap();
        }
    }
    catch(const std::exception& ex)
    {
        std::cout << ex.what() << "\n";
    }

    return 0;
}
