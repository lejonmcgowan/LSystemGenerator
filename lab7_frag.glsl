uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform float intensity1;
uniform float intensity2;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec4 vP;
varying vec3 vN;

vec3 calcColor(vec3 light, float intensity)
{
		vec3 normal = normalize(vN);
		vec3 lightVector = light - vP.xyz;
		vec3 normalLight = normalize(lightVector);

		vec3 diffuse = kd * max(dot(normalLight.xyz, normal),0.0);

		vec3 eye = normalize(-vP.xyz);
	    vec3 halfVec = normalize((normalLight + eye));
	    
	    vec3 specular = ks*pow(max(dot(halfVec,normal),0.0),s);

	    vec3 ambience = ka;
	    
	    return intensity * (ambience + diffuse + specular);
}

void main()
{			
		vec3 color = calcColor(lightPos1, intensity1);
		color = color + calcColor(lightPos2, intensity2);

		gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}