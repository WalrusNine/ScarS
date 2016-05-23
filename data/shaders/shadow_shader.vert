#version 330

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 depthModel;
uniform mat4 depthVP;

void main(){
	gl_Position = depthVP * depthModel * vec4(vertexPosition_modelspace,1);
}

