attribute vec4 vertPos; // in object space
attribute vec3 vertNor; // in object space
uniform mat4 P;
uniform mat4 MV;

varying vec4 vP; // in object space
varying vec3 vN; // in object space

//varying vec3 color; // Pass to fragment shader

void main()
{
	vP = MV * vertPos;
	vec4 vnTemp = MV * vec4(vertNor, 0.0);
	vN = vnTemp.xyz;
	
	gl_Position = P * MV * vertPos;
}
