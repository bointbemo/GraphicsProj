#version 330 core

uniform sampler2D homeTex;
uniform sampler2D netherTex;
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float height;
uniform float width;

in Vertex {
vec3 colour;
vec2 texCoord;
vec3 normal;
vec3 worldPos;

} IN;
out vec4 fragColour;

void main(void) {
vec3 incident = normalize(lightPos - IN.worldPos );
vec3 viewDir = normalize(cameraPos - IN.worldPos );
vec3 halfDir = normalize(incident + viewDir );
float lambert = max(dot(incident , IN.normal), 0.0f);
float distance = length(lightPos - IN.worldPos );
float attenuation = 1.0 - clamp(distance / lightRadius , 0.0, 1.0);
float specFactor = clamp(dot(halfDir , IN.normal ) ,0.0 ,1.0);
specFactor = pow(specFactor , 60.0 );

if(IN.worldPos.z > width/2.04f){
vec4 diffuse = texture(homeTex , IN.texCoord );
vec3 surface = (diffuse.rgb * lightColour.rgb);
fragColour.rgb = surface * lambert * attenuation;
fragColour.rgb += (lightColour.rgb * specFactor )* attenuation *0.33;
fragColour.rgb += surface * 0.3f; // ambient!
fragColour.a = diffuse.a;
}
if(IN.worldPos.z<width/2.04f){
vec4 diffuse = texture(netherTex , IN.texCoord );
vec3 surface = (diffuse.rgb * lightColour.rgb);
fragColour.rgb = surface * lambert * attenuation;
fragColour.rgb += (lightColour.rgb * specFactor )* attenuation *0.33;
fragColour.rgb += surface * 0.3f; // ambient!
fragColour.a = diffuse.a;
}
if(IN.worldPos.z > width/2.04f){
if(IN.worldPos.z<width/1.9f){
if(IN.worldPos.x<height/1.4f){
if(IN.worldPos.x>height/1.9f){
float rei = (width/1.9f - IN.worldPos.z)/(width/1.9f-width/2.04f);
vec4 diffuse = mix(texture(netherTex , IN.texCoord ),texture(homeTex , IN.texCoord ),1-rei);
vec3 surface = (diffuse.rgb * lightColour.rgb);
fragColour.rgb = surface * lambert * attenuation;
fragColour.rgb += (lightColour.rgb * specFactor )* attenuation *0.33;
fragColour.rgb += surface * 0.3f; // ambient!
fragColour.a = diffuse.a;

}}}}
if(IN.worldPos.z < width/2.04f){
if(IN.worldPos.z > width/2.18f){
if(IN.worldPos.x<height/1.4f){
if(IN.worldPos.x>height/1.9f){
float rei = (width/2.18f-IN.worldPos.z)/(width/2.18f -width/2.04);
vec4 diffuse = mix(texture(netherTex , IN.texCoord ),texture(homeTex , IN.texCoord ),rei);
vec3 surface = (diffuse.rgb * lightColour.rgb);
fragColour.rgb = surface * lambert * attenuation;
fragColour.rgb += (lightColour.rgb * specFactor )* attenuation *0.33;
fragColour.rgb += surface * 0.3f; // ambient!
fragColour.a = diffuse.a;
}}}}


}