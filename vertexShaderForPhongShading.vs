#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//the job is to send the vertices after transformation in the nesxt stage of pipeline

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    FragPos = vec3(model * vec4(aPos, 1.0)); //position of the pixel/vertex after applying modeling transformation , because the applying need to be applied on world space
    Normal = mat3(transpose(inverse(model))) * aNormal; //transformed surface normal.
    
}
