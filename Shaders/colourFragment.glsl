#version 330 core

in Vertex{ 
vec4 colour;
} IN;

out vec4 fragColour;
void main(void) {
if (IN.colour.r >= 0.5){discard;}
if (IN.colour.g >= 0.5){discard;}
if (IN.colour.b >= 0.5){discard;}

fragColour = IN.colour;
}