#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec3 color;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; i < 4; i++)
        {
            if(ubo.proj[i][j] == 1)
            {
                color = vec3(0.0, 0.3, inColor.z);
            }
        }
    }
    
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    fragColor = color;
}

