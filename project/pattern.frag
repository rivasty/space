#version 330 compatibility
in vec2 vST;
in vec3 fpos;
uniform sampler2D tex;
uniform float uSunx;
uniform float uSuny;
uniform float uSunz;
uniform int   uSun;

flat in vec3 vNf;
     in vec3 vNs;
flat in vec3 vLf;
     in vec3 vLs;
flat in vec3 vEf;
     in vec3 vEs;

void
main( )
{
	vec3 normal = normalize(vNs);
    vec3 light  = normalize(vLs);
    vec3 eye    = normalize(vEs);
    
    vec4 color = texture(tex,vST);

    vec3 ambient = 0.1 * color.rgb;
    float d = max(dot(normal,light),0.0);
    vec3 diffuse;
    if(uSun == 1)
    {
        diffuse = 1.0 * d * color.rgb;
    }
    else
    {
        diffuse = 0.75 * d * color.rgb;
    }

    gl_FragColor = vec4((ambient + diffuse) * color.rgb, 1.0);
}