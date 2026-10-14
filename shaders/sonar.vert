#version 330 core

uniform mat4 uInvViewProj; // Inverse view-projection matrix

out vec2 vWorld; //every pixel 's world position

void main(){

	//giant triangle without buffer , vertexId = 0,1,2
	vec2 p =vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
	vec2 ndc = p * 2.0 - 1.0; // Convert to normalized device coordinates

	gl_Position = vec4(ndc, 0.0, 1.0);

	vWorld = (uInvViewProj * vec4(ndc, 0.0, 1.0)).xy;

}