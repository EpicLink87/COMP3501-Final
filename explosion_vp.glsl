#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color; //Actually contains a number between 0 and 1 in the R variable

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;
uniform float progress;

// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
float grav = 0.005; // Gravity
float speed = 6.0; // Allows to control the speed of the explosion


void main()
{
    float phase = color.r - 0.5;
    float t = (progress + phase); // Our time parameter
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 1.0);

    // Move point along normal and down with t*t (acceleration under gravity)
    position.x += norm.x*t*speed;
    position.y += norm.y*t*speed;
    position.z += norm.z*t*speed;
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    // Define color of vertex
    vertex_color = color.rgb; // Color defined during the construction of the particles

    // Forward time step to geometry shader
    timestep = t;
}
