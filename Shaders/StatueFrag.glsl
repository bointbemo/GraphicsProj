#version 330 core
uniform sampler2D diffuseTex;

in Vertex {
vec2 texCoord;
} IN;

out vec4 fragColour;
void main(void) {

fragColour = texture(diffuseTex , IN.texCoord)-vec4(0.0f,0.0f,0.0f,0.8f);


}