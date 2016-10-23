varying vec4 vP;
varying vec3 vN;

void main()
{
	float threshold = 0.25;
	vec3 eye = normalize(-vP.xyz);
	vec3 normal = normalize(vN);

	if (abs(dot(normal,eye)) < threshold) //in threshold, should color
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	else 
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);

}