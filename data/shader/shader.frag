#version 130

in vec2 screenPos;

uniform float screenWidth;
uniform float screenHeight;

uniform vec3 cameraPos;
uniform vec3 cameraUp;
uniform vec3 cameraDir;
uniform float stepX;
uniform float stepY;

uniform sampler3D worldTex;
uniform ivec3 worldSize;

void getRay(out vec3 rayOrigin, out vec3 rayDir, in vec2 delta)
{
    rayOrigin = cameraPos;
    rayDir =   cameraDir
             + cross(cameraDir, cameraUp) * ((screenPos.x - screenWidth/2.) * stepX + delta.x * stepX)
             + cameraUp * ((screenPos.y - screenHeight/2.) * stepY + delta.y * stepY); 
    rayDir = normalize(rayDir);
}

// mat4 bspCoeffs = mat4(-1, 3,-3, 1,
//                        3,-6, 3, 0,
//                       -3, 0, 3, 0,
//                        1, 4, 1, 0);
mat4 bspCoeffs = 1./6.*mat4(-1, 3,-3, 1,
                       3,-6, 0, 4,
                      -3, 3, 3, 1,
                       1, 0, 0, 0);

float bspline_2d_fp(sampler3D tex, vec3 pos, ivec3 texSize)
{
    pos *= texSize;
    vec3 tc = floor(pos - 0.5) + 0.5;

    vec3 alpha = pos - tc;
    vec3 alpha2 = alpha*alpha;
    vec3 alpha3 = alpha2*alpha;
    vec3 w0 = 1./6. * (-alpha3 + 3.*alpha2 - 3.*alpha + 1.);
    vec3 w1 = 1./6. * (3.*alpha3 - 6.*alpha2 + 4.);
    vec3 w2 = 1./6. * (-3.*alpha3 + 3.*alpha2 + 3.*alpha + 1.);
    vec3 w3 = 1./6. * alpha3;
    vec3 s0 = w0 + w1;// or: s0 = 1 - s1
    vec3 s1 = w2 + w3;

    vec3 t0 = tc - 1. + w1 / (w0 + w1);
    vec3 t1 = tc + 1. + w3 / (w2 + w3);

    // vec4 alpha  = vec4(pos - tc, 0);
    // vec4 alpha2 = vec4(alpha*alpha);
    // vec4 alpha3 = vec4(alpha2*alpha);
    // mat4 alphaMat = mat4(alpha3, alpha2, alpha, vec4(1,1,1,0));
    // mat4 w = alphaMat * bspCoeffs;

    // vec3 s0 = w[0] + w[1];// or: s0 = 1 - s1
    // vec3 s1 = w[2] + w[3];

    // vec3 t0 = tc - 1. + w[1] / (w[0] + w[1]);
    // vec3 t1 = tc + 1. + w[3] / (w[2] + w[3]);

    t0 /= texSize;
    t1 /= texSize;

    float p000 = texture(tex, vec3(t0.x, t0.y, t0.z)).x;
    float p001 = texture(tex, vec3(t0.x, t0.y, t1.z)).x;
    float p010 = texture(tex, vec3(t0.x, t1.y, t0.z)).x;
    float p011 = texture(tex, vec3(t0.x, t1.y, t1.z)).x;
    float p100 = texture(tex, vec3(t1.x, t0.y, t0.z)).x;
    float p101 = texture(tex, vec3(t1.x, t0.y, t1.z)).x;
    float p110 = texture(tex, vec3(t1.x, t1.y, t0.z)).x;
    float p111 = texture(tex, vec3(t1.x, t1.y, t1.z)).x;

    float a = mix(p000, p100, s1.x);
    float b = mix(p010, p110, s1.x);
    float ab = mix(a, b, s1.y);

    float c = mix(p001, p101, s1.x);
    float d = mix(p011, p111, s1.x);
    float cd = mix(c, d, s1.y);

    return mix(ab, cd, s1.z);
}

float distToScene(vec3 pos)
{
    return bspline_2d_fp(worldTex, pos.xyz, worldSize);
}

float rand2(vec3 n)
{
    return fract(sin(dot(n.xyz, vec3(10, 300, 400)))* 43758.5453);
}

float interp_rand2(vec3 n, float gs)
{
    vec3 l = floor(n * gs) / gs;
    vec3 r = (floor(n * gs)+1.) / gs;

    vec3 f = 1. - (r - n) * gs;

    float p000 = rand2(vec3(l.x, l.y, l.z));
    float p001 = rand2(vec3(l.x, l.y, r.z));
    float p010 = rand2(vec3(l.x, r.y, l.z));
    float p011 = rand2(vec3(l.x, r.y, r.z));
    float p100 = rand2(vec3(r.x, l.y, l.z));
    float p101 = rand2(vec3(r.x, l.y, r.z));
    float p110 = rand2(vec3(r.x, r.y, l.z));
    float p111 = rand2(vec3(r.x, r.y, r.z));

    float c = mix(mix(mix(p000, p100, f.x),
                      mix(p001, p101, f.x),
                      f.z),
                  mix(mix(p010, p110, f.x),
                      mix(p011, p111, f.x),
                      f.z),
                  f.y);

    return c;
}

float rand(vec3 n, float dist)
{
    float c0 = interp_rand2(n, 100.);
    float c1 = interp_rand2(n, 1000.);
    float c2 = interp_rand2(n, 7000.);

    float c = mix(c1, c0, dist);

    c = mix(c2, c, min(1.,dist*4.));

    return 3. * ((c * 0.5) + 0.5);
}

vec3 sceneColor(vec3 pos, vec3 n, float dist)
{
    vec3 green = vec3(0.27, 0.42, 0.19);
    vec3 gray = vec3(0.2, 0.2, 0.19);
    float intensity = rand(pos, dist);

    vec3 color = vec3(intensity,intensity,intensity);

    float grayness = exp(-dot(n,vec3(0,1,0))/0.9);

    color = mix(green, gray, min(1.,grayness)) * intensity;

    return color;
}

vec3 normal(vec3 pos)
{
    float eps_x = 1./float(worldSize.x);
    float eps_y = 1./float(worldSize.y);
    float eps_z = 1./float(worldSize.z);
    vec3 t = vec3(distToScene(vec3(pos.x-eps_x, pos.y, pos.z)) - distToScene(vec3(pos.x+eps_x, pos.y, pos.z)),
                  distToScene(vec3(pos.x, pos.y-eps_y, pos.z)) - distToScene(vec3(pos.x, pos.y+eps_y, pos.z)),
                  distToScene(vec3(pos.x, pos.y, pos.z-eps_z)) - distToScene(vec3(pos.x, pos.y, pos.z+eps_z)));
    t = -normalize(t);
    return t;
}

vec3 fog(vec3 color, float dist, float maxDist, vec3 fogColor)
{
    float amount = exp(-dist/maxDist*10.);
    return mix(color, fogColor, 1.-amount);
}

void main(void)
{
    vec3 lightDir = vec3(0,1,1);
    lightDir = normalize(lightDir);

    vec3 sunColor = vec3(253./255., 254./255., 208./255.);
    vec3 skyColor = vec3(0.738,0.8125,0.8789);

    vec3 color = skyColor;
    bool hit = false;

    vec3 o, d;
    getRay(o, d, vec2(0,0));

    float maxT = 6.;
    float t = 0.;

    while(t < maxT)
    {
        vec3 p = o + t*d;
        if(p.y > 1. && d.y > 0.)
            break;
        float dist = distToScene(p);
        if(dist > 0.)
        {
            t += max(dist, 0.003*t);
        }
        else
        {
            vec3 n = normal(p);
            color = max(0.3,dot(n, lightDir)) * sceneColor(p, n, t);
            hit = true;
            break;
        }
    }
    color = fog(color, t, maxT, skyColor);

    if(!hit)
    {
        float d = max(0.,dot(d,lightDir)) - 1.;
        float sigma = 0.003;
        float a = exp(-0.5*(d / sigma)*(d / sigma))*1.3;
        color = mix(skyColor, sunColor*1.3, a);
    }

    gl_FragColor = vec4(color, 1.);
}
