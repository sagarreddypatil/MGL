project('MGL', 'c', 'objc')

spirv_tools = dependency('SPIRV-Tools')
glslang = dependency('glslang')
frameworks = dependency('appleframeworks', modules: ['Metal', 'OpenGL', 'Foundation'])

inc_dirs = include_directories(
    [
        'include',
        'include/GL',
    ],
)

c_srcs = [
    'src/buffers.c',
    'src/compute.c',
    'src/draw_buffers.c',
    'src/error.c',
    'src/fence.c',
    'src/framebuffers.c',
    'src/get.c',
    'src/gl_core.c',
    'src/glm_context.c',
    'src/glm_dispatch.c',
    'src/glm_params.c',
    'src/hash_table.c',
    'src/mgl_funcs_to_be_implemented.c',
    'src/non_core_unimplemented.c',
    'src/pixel_utils.c',
    'src/program.c',
    'src/rendering.c',
    'src/samplers.c',
    'src/shaders.c',
    'src/state.c',
    'src/tex_param.c',
    'src/textures.c',
    'src/uniforms.c',
    'src/utils.c',
    'src/vertex_arrays.c',
    'src/vertex_buffers.c',
]

objc_srcs = ['src/MGLRenderer.m', 'src/MGLTextures.m']

mgl_lib = shared_library(
    'mgl',
    sources: [c_srcs, objc_srcs],
    include_directories: inc_dirs,
    dependencies: [frameworks, glslang, spirv_tools],
)