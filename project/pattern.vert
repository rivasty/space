#version 330 compatibility
out vec2 vST;
out vec3 fpos;
uniform float uSunx;
uniform float uSuny;
uniform float uSunz;
uniform int   uSun;

flat out vec3 vNf;
     out vec3 vNs;
flat out vec3 vLf;
     out vec3 vLs;
flat out vec3 vEf;
     out vec3 vEs;

vec3 LIGHTPOSITION = vec3(uSunx, uSuny, uSunz);
void
main( )
{
	vec4 ECposition = gl_ModelViewMatrix * gl_Vertex;
	vST = gl_MultiTexCoord0.st;
	vNf = normalize( gl_NormalMatrix * gl_Normal );
	vNs = vNf;

	vLf = LIGHTPOSITION - ECposition.xyz;
	vLs = vLf;
	vEf = vec3( 0., 0., 0. ) - ECposition.xyz;
	vEs = vEf;
	
	fpos = gl_Vertex.xyz;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}