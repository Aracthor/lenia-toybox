#version 300 es

const vec2 quadVertices[] = vec2[]
(
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0)
);

void main()
{
    gl_Position = vec4(quadVertices[gl_VertexID], 0.0, 1.0);
}
