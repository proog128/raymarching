attribute vec4 position;
varying vec2 screenPos;

uniform float screenWidth;
uniform float screenHeight;

void main()
{
    screenPos.x = (position.x + 1.) / 2. * screenWidth;
    screenPos.y = (position.y + 1.) / 2. * screenHeight;
    
    gl_Position = vec4(position.x, position.y, -1, 1);
}
