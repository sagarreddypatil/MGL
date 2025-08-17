/*
 * Copyright (C) Michael Larson on 1/6/2022
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * glm_params.c
 * MGL
 *
 */

#include <assert.h>
#include "glcorearb.h"

#include "glm_context.h"

#include <unistd.h>
#include <dlfcn.h>
#include <OpenGL/OpenGL.h>

typedef struct GLMContextRec_t *GLMContext;


void getMacOSDefaults(GLMContext glm_ctx)
{
    void *OpenGL, *libGL;
    const char *OpenGLPath = "/System/Library/Frameworks/OpenGL.framework/OpenGL";
    const char *libGLPath = "/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib";
    CGLError (*CGLChoosePixelFormat)(const CGLPixelFormatAttribute *attribs, CGLPixelFormatObj OPENGL_NULLABLE * OPENGL_NONNULL pix, GLint *npix);
    CGLError (*CGLDestroyPixelFormat)(CGLPixelFormatObj pix);
    CGLError (*CGLCreateContext)(CGLPixelFormatObj pix, CGLContextObj OPENGL_NULLABLE share, CGLContextObj OPENGL_NULLABLE * OPENGL_NONNULL ctx);
    CGLError (*CGLDestroyContext)(CGLContextObj ctx);
    void (*glGetIntegerv)(GLenum param, GLuint *params);
    void (*glGetDoublev)(GLenum param, GLdouble *params);
    void (*glGetFloatv)(GLenum param, GLfloat *params);
    void (*glGetBooleanv)(GLenum param, GLboolean *params);

    CGLContextObj ctx;

    CGLPixelFormatAttribute attributes[4] = {
      kCGLPFAAccelerated,   // no software rendering
      kCGLPFAOpenGLProfile, // core profile with the version stated below
      (CGLPixelFormatAttribute) kCGLOGLPVersion_GL4_Core,
      (CGLPixelFormatAttribute) 0
    };

    OpenGL = dlopen(OpenGLPath, RTLD_LAZY | RTLD_LOCAL); assert(OpenGL);
    libGL = dlopen(libGLPath, RTLD_LAZY | RTLD_LOCAL); assert(libGL);

    CGLChoosePixelFormat = dlsym(OpenGL, "CGLChoosePixelFormat"); assert(CGLChoosePixelFormat);
    CGLDestroyPixelFormat = dlsym(OpenGL, "CGLDestroyPixelFormat"); assert(CGLDestroyPixelFormat);
    CGLCreateContext = dlsym(OpenGL, "CGLCreateContext"); assert(CGLCreateContext);
    CGLDestroyContext = dlsym(OpenGL, "CGLDestroyContext"); assert(CGLDestroyContext);
    glGetIntegerv = dlsym(libGL, "glGetIntegerv"); assert(glGetIntegerv);
    glGetDoublev = dlsym(libGL, "glGetDoublev"); assert(glGetDoublev);
    glGetFloatv = dlsym(libGL, "glGetFloatv"); assert(glGetFloatv);
    glGetBooleanv = dlsym(libGL, "glGetBooleanv"); assert(glGetBooleanv);

    CGLPixelFormatObj pix;
    CGLError errorCode;
    GLint num; // stores the number of possible pixel formats
    errorCode = CGLChoosePixelFormat( attributes, &pix, &num ); assert(errorCode == kCGLNoError);
    // add error checking here
    errorCode = CGLCreateContext( pix, NULL, &ctx ); assert(errorCode == kCGLNoError);
    CGLDestroyPixelFormat( pix );

    errorCode = CGLSetCurrentContext( ctx ); assert(errorCode == kCGLNoError);

    glGetFloatv(GL_POINT_SIZE,&glm_ctx->state.var.point_size);
    glGetIntegerv(GL_POINT_SIZE_RANGE,&glm_ctx->state.var.point_size_range);
    glGetIntegerv(GL_POINT_SIZE_GRANULARITY,&glm_ctx->state.var.point_size_granularity);
    glGetFloatv(GL_LINE_WIDTH,&glm_ctx->state.var.line_width);
    glGetIntegerv(GL_LINE_WIDTH_RANGE,&glm_ctx->state.var.line_width_range);
    glGetIntegerv(GL_LINE_WIDTH_GRANULARITY,&glm_ctx->state.var.line_width_granularity);
    glGetIntegerv(GL_POLYGON_MODE,&glm_ctx->state.var.polygon_mode);
    glGetIntegerv(GL_CULL_FACE_MODE,&glm_ctx->state.var.cull_face_mode);
    glGetIntegerv(GL_FRONT_FACE,&glm_ctx->state.var.front_face);
    glGetDoublev(GL_DEPTH_RANGE,glm_ctx->state.var.depth_range);

    glGetBooleanv(GL_DEPTH_WRITEMASK,&glm_ctx->state.var.depth_writemask);

    glGetDoublev(GL_DEPTH_CLEAR_VALUE,&glm_ctx->state.var.depth_clear_value);
    glGetIntegerv(GL_DEPTH_FUNC,&glm_ctx->state.var.depth_func);
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE,&glm_ctx->state.var.stencil_clear_value);
    glGetIntegerv(GL_STENCIL_FUNC,&glm_ctx->state.var.stencil_func);
    glGetIntegerv(GL_STENCIL_VALUE_MASK,&glm_ctx->state.var.stencil_value_mask);
    glGetIntegerv(GL_STENCIL_FAIL,&glm_ctx->state.var.stencil_fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,&glm_ctx->state.var.stencil_pass_depth_fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,&glm_ctx->state.var.stencil_pass_depth_pass);
    glGetIntegerv(GL_STENCIL_REF,&glm_ctx->state.var.stencil_ref);
    glGetIntegerv(GL_STENCIL_WRITEMASK,&glm_ctx->state.var.stencil_writemask);
    glGetIntegerv(GL_VIEWPORT,glm_ctx->state.viewport);

    glGetIntegerv(GL_BLEND_DST,&glm_ctx->state.var.blend_dst_rgb[0]);
    glGetIntegerv(GL_BLEND_SRC,&glm_ctx->state.var.blend_src_rgb[0]);

    glGetIntegerv(GL_LOGIC_OP_MODE,&glm_ctx->state.var.logic_op_mode);
    glGetIntegerv(GL_DRAW_BUFFER,&glm_ctx->state.draw_buffer);
    glGetIntegerv(GL_READ_BUFFER,&glm_ctx->state.read_buffer);

    glGetIntegerv(GL_SCISSOR_BOX,glm_ctx->state.var.scissor_box);

    glGetFloatv(GL_COLOR_CLEAR_VALUE,glm_ctx->state.color_clear_value);

    glGetBooleanv(GL_COLOR_WRITEMASK,glm_ctx->state.var.color_writemask[0]);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&glm_ctx->state.var.max_texture_size);
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS,&glm_ctx->state.var.max_viewport_dims);
    glGetIntegerv(GL_SUBPIXEL_BITS,&glm_ctx->state.var.subpixel_bits);
    glGetIntegerv(GL_POLYGON_OFFSET_UNITS,&glm_ctx->state.var.polygon_offset_units);
    glGetIntegerv(GL_POLYGON_OFFSET_FACTOR,&glm_ctx->state.var.polygon_offset_factor);
    glGetIntegerv(GL_TEXTURE_BINDING_1D,&glm_ctx->state.var.texture_binding_1d);
    glGetIntegerv(GL_TEXTURE_BINDING_2D,&glm_ctx->state.var.texture_binding_2d);
    glGetIntegerv(GL_TEXTURE_BINDING_3D,&glm_ctx->state.var.texture_binding_3d);
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,&glm_ctx->state.var.max_3d_texture_size);
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES,&glm_ctx->state.var.max_elements_vertices);
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES,&glm_ctx->state.var.max_elements_indices);
    glGetIntegerv(GL_SMOOTH_POINT_SIZE_RANGE,&glm_ctx->state.var.smooth_point_size_range);
    glGetIntegerv(GL_SMOOTH_POINT_SIZE_GRANULARITY,&glm_ctx->state.var.smooth_point_size_granularity);
    glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE,&glm_ctx->state.var.smooth_line_width_range);
    glGetIntegerv(GL_SMOOTH_LINE_WIDTH_GRANULARITY,&glm_ctx->state.var.smooth_line_width_granularity);
    glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE,&glm_ctx->state.var.aliased_line_width_range);
    glGetIntegerv(GL_SAMPLE_COVERAGE_VALUE,&glm_ctx->state.var.sample_coverage_value);
    glGetIntegerv(GL_SAMPLE_COVERAGE_INVERT,&glm_ctx->state.var.sample_coverage_invert);
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP,&glm_ctx->state.var.texture_binding_cube_map);
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,&glm_ctx->state.var.max_cube_map_texture_size);
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS,&glm_ctx->state.var.num_compressed_texture_formats);
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS,&glm_ctx->state.var.compressed_texture_formats);

    glGetIntegerv(GL_BLEND_DST_RGB,&glm_ctx->state.var.blend_dst_rgb[0]);
    glGetIntegerv(GL_BLEND_SRC_RGB,&glm_ctx->state.var.blend_src_rgb[0]);
    glGetIntegerv(GL_BLEND_DST_ALPHA,&glm_ctx->state.var.blend_dst_alpha[0]);
    glGetIntegerv(GL_BLEND_SRC_ALPHA,&glm_ctx->state.var.blend_src_alpha[0]);

    glGetIntegerv(GL_MAX_TEXTURE_LOD_BIAS,&glm_ctx->state.var.max_texture_lod_bias);

    glGetFloatv(GL_BLEND_COLOR,glm_ctx->state.var.blend_color);

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&glm_ctx->state.var.array_buffer_binding);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,&glm_ctx->state.var.element_array_buffer_binding);

    glGetIntegerv(GL_BLEND_EQUATION_RGB,&glm_ctx->state.var.blend_equation_rgb[0]);

    glGetIntegerv(GL_STENCIL_BACK_FUNC,&glm_ctx->state.var.stencil_back_func);
    glGetIntegerv(GL_STENCIL_BACK_FAIL,&glm_ctx->state.var.stencil_back_fail);
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL,&glm_ctx->state.var.stencil_back_pass_depth_fail);
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS,&glm_ctx->state.var.stencil_back_pass_depth_pass);
    glGetIntegerv(GL_MAX_DRAW_BUFFERS,&glm_ctx->state.var.max_draw_buffers);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA,&glm_ctx->state.var.blend_equation_alpha[0]);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&glm_ctx->state.max_vertex_attribs);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&glm_ctx->state.var.max_texture_image_units);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,&glm_ctx->state.var.max_fragment_uniform_components);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS,&glm_ctx->state.var.max_vertex_uniform_components);
    glGetIntegerv(GL_MAX_VARYING_FLOATS,&glm_ctx->state.var.max_varying_floats);
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,&glm_ctx->state.var.max_vertex_texture_image_units);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,&glm_ctx->state.var.max_combined_texture_image_units);
    glGetIntegerv(GL_CURRENT_PROGRAM,&glm_ctx->state.var.current_program);
    glGetIntegerv(GL_STENCIL_BACK_REF,&glm_ctx->state.var.stencil_back_ref);
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK,&glm_ctx->state.var.stencil_back_value_mask);
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK,&glm_ctx->state.var.stencil_back_writemask);
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING,&glm_ctx->state.var.pixel_pack_buffer_binding);
    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING,&glm_ctx->state.var.pixel_unpack_buffer_binding);
    glGetIntegerv(GL_MAX_CLIP_DISTANCES,&glm_ctx->state.var.max_clip_distances);

    //glGetIntegerv(GL_MAJOR_VERSION,&glm_ctx->state.var.major_version);
    glm_ctx->state.var.major_version = 4;

    //glGetIntegerv(GL_MINOR_VERSION,&glm_ctx->state.var.minor_version);
    glm_ctx->state.var.minor_version = 6;

    //glGetIntegerv(GL_NUM_EXTENSIONS,&glm_ctx->state.var.num_extensions);
    glm_ctx->state.var.num_extensions = 0;

    glGetIntegerv(GL_CONTEXT_FLAGS,&glm_ctx->state.var.context_flags);
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,&glm_ctx->state.var.max_array_texture_layers);
    glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET,&glm_ctx->state.var.min_program_texel_offset);
    glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET,&glm_ctx->state.var.max_program_texel_offset);
    glGetIntegerv(GL_MAX_VARYING_COMPONENTS,&glm_ctx->state.var.max_varying_components);
    glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY,&glm_ctx->state.var.texture_binding_1d_array);
    glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY,&glm_ctx->state.var.texture_binding_2d_array);
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&glm_ctx->state.var.max_renderbuffer_size);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&glm_ctx->state.var.draw_framebuffer_binding);
    glGetIntegerv(GL_RENDERBUFFER_BINDING,&glm_ctx->state.var.renderbuffer_binding);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&glm_ctx->state.var.read_framebuffer_binding);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING,&glm_ctx->state.var.vertex_array_binding);
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE,&glm_ctx->state.var.max_texture_buffer_size);
    glGetIntegerv(GL_TEXTURE_BINDING_BUFFER,&glm_ctx->state.var.texture_binding_buffer);
    glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE,&glm_ctx->state.var.texture_binding_rectangle);
    glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE,&glm_ctx->state.var.max_rectangle_texture_size);
    glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX,&glm_ctx->state.var.primitive_restart_index);
    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING,&glm_ctx->state.var.uniform_buffer_binding);
    glGetIntegerv(GL_UNIFORM_BUFFER_START,&glm_ctx->state.var.uniform_buffer_start);
    glGetIntegerv(GL_UNIFORM_BUFFER_SIZE,&glm_ctx->state.var.uniform_buffer_size);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS,&glm_ctx->state.var.max_vertex_uniform_blocks);
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS,&glm_ctx->state.var.max_geometry_uniform_blocks);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS,&glm_ctx->state.var.max_fragment_uniform_blocks);
    glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS,&glm_ctx->state.var.max_combined_uniform_blocks);
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS,&glm_ctx->state.var.max_uniform_buffer_bindings);
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE,&glm_ctx->state.var.max_uniform_block_size);
    glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,&glm_ctx->state.var.max_combined_vertex_uniform_components);
    glGetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS,&glm_ctx->state.var.max_combined_geometry_uniform_components);
    glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,&glm_ctx->state.var.max_combined_fragment_uniform_components);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,&glm_ctx->state.var.uniform_buffer_offset_alignment);
    glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,&glm_ctx->state.var.max_geometry_texture_image_units);
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,&glm_ctx->state.var.max_geometry_uniform_components);
    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS,&glm_ctx->state.var.max_vertex_output_components);
    glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS,&glm_ctx->state.var.max_geometry_input_components);
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS,&glm_ctx->state.var.max_geometry_output_components);
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS,&glm_ctx->state.var.max_fragment_input_components);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK,&glm_ctx->state.var.context_profile_mask);
    glm_ctx->state.var.context_profile_mask = GL_CONTEXT_CORE_PROFILE_BIT;
    glGetIntegerv(GL_PROVOKING_VERTEX,&glm_ctx->state.var.provoking_vertex);
    glGetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT,&glm_ctx->state.var.max_server_wait_timeout);
    glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS,&glm_ctx->state.var.max_sample_mask_words);
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE,&glm_ctx->state.var.texture_binding_2d_multisample);
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,&glm_ctx->state.var.texture_binding_2d_multisample_array);
    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES,&glm_ctx->state.var.max_color_texture_samples);
    glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES,&glm_ctx->state.var.max_depth_texture_samples);
    glGetIntegerv(GL_MAX_INTEGER_SAMPLES,&glm_ctx->state.var.max_integer_samples);
    glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS,&glm_ctx->state.var.max_dual_source_draw_buffers);
    glGetIntegerv(GL_SAMPLER_BINDING,&glm_ctx->state.var.sampler_binding);
    glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS,&glm_ctx->state.var.max_tess_control_uniform_blocks);
    glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS,&glm_ctx->state.var.max_tess_evaluation_uniform_blocks);
    glGetIntegerv(GL_SHADER_COMPILER,&glm_ctx->state.var.shader_compiler);
    glGetIntegerv(GL_SHADER_BINARY_FORMATS,&glm_ctx->state.var.shader_binary_formats);
    glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS,&glm_ctx->state.var.num_shader_binary_formats);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS,&glm_ctx->state.var.max_vertex_uniform_vectors);
    glGetIntegerv(GL_MAX_VARYING_VECTORS,&glm_ctx->state.var.max_varying_vectors);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS,&glm_ctx->state.var.max_fragment_uniform_vectors);
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS,&glm_ctx->state.var.num_program_binary_formats);
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS,&glm_ctx->state.var.program_binary_formats);
    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING,&glm_ctx->state.var.program_pipeline_binding);
    glGetIntegerv(GL_MAX_VIEWPORTS,&glm_ctx->state.var.max_viewports);
    glGetIntegerv(GL_VIEWPORT_SUBPIXEL_BITS,&glm_ctx->state.var.viewport_subpixel_bits);
    glGetIntegerv(GL_VIEWPORT_BOUNDS_RANGE,&glm_ctx->state.var.viewport_bounds_range);
    glGetIntegerv(GL_LAYER_PROVOKING_VERTEX,&glm_ctx->state.var.layer_provoking_vertex);
    glGetIntegerv(GL_VIEWPORT_INDEX_PROVOKING_VERTEX,&glm_ctx->state.var.viewport_index_provoking_vertex);
    glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT,&glm_ctx->state.var.min_map_buffer_alignment);
    glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS,&glm_ctx->state.var.max_vertex_atomic_counters);
    glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS,&glm_ctx->state.var.max_tess_control_atomic_counters);
    glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS,&glm_ctx->state.var.max_tess_evaluation_atomic_counters);
    glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS,&glm_ctx->state.var.max_geometry_atomic_counters);
    glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS,&glm_ctx->state.var.max_fragment_atomic_counters);
    glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS,&glm_ctx->state.var.max_combined_atomic_counters);
    glGetIntegerv(GL_MAX_ELEMENT_INDEX,&glm_ctx->state.var.max_element_index);
    glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS,&glm_ctx->state.var.max_compute_uniform_blocks);
    glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS,&glm_ctx->state.var.max_compute_texture_image_units);
    glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS,&glm_ctx->state.var.max_compute_uniform_components);
    glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS,&glm_ctx->state.var.max_compute_atomic_counter_buffers);
    glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS,&glm_ctx->state.var.max_compute_atomic_counters);
    glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,&glm_ctx->state.var.max_combined_compute_uniform_components);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,&glm_ctx->state.var.max_compute_work_group_invocations);

    //glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,glm_ctx->state.var.max_compute_work_group_count);
    //glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,glm_ctx->state.var.max_compute_work_group_size);

    glGetIntegerv(GL_DISPATCH_INDIRECT_BUFFER_BINDING,&glm_ctx->state.var.dispatch_indirect_buffer_binding);
    glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH,&glm_ctx->state.var.max_debug_group_stack_depth);
    glGetIntegerv(GL_DEBUG_GROUP_STACK_DEPTH,&glm_ctx->state.var.debug_group_stack_depth);
    glGetIntegerv(GL_MAX_LABEL_LENGTH,&glm_ctx->state.var.max_label_length);
    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS,&glm_ctx->state.var.max_uniform_locations);
    glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH,&glm_ctx->state.var.max_framebuffer_width);
    glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT,&glm_ctx->state.var.max_framebuffer_height);
    glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS,&glm_ctx->state.var.max_framebuffer_layers);
    glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES,&glm_ctx->state.var.max_framebuffer_samples);
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,&glm_ctx->state.max_color_attachments);
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING,&glm_ctx->state.var.shader_storage_buffer_binding);
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_START,&glm_ctx->state.var.shader_storage_buffer_start);
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_SIZE,&glm_ctx->state.var.shader_storage_buffer_size);
    glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,&glm_ctx->state.var.max_vertex_shader_storage_blocks);
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,&glm_ctx->state.var.max_geometry_shader_storage_blocks);
    glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,&glm_ctx->state.var.max_tess_control_shader_storage_blocks);
    glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,&glm_ctx->state.var.max_tess_evaluation_shader_storage_blocks);
    glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,&glm_ctx->state.var.max_fragment_shader_storage_blocks);
    glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,&glm_ctx->state.var.max_compute_shader_storage_blocks);
    glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS,&glm_ctx->state.var.max_combined_shader_storage_blocks);
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,&glm_ctx->state.var.max_shader_storage_buffer_bindings);
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,&glm_ctx->state.var.shader_storage_buffer_offset_alignment);
    glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT,&glm_ctx->state.var.texture_buffer_offset_alignment);
    glGetIntegerv(GL_VERTEX_BINDING_DIVISOR,&glm_ctx->state.var.vertex_binding_divisor);
    glGetIntegerv(GL_VERTEX_BINDING_OFFSET,&glm_ctx->state.var.vertex_binding_offset);
    glGetIntegerv(GL_VERTEX_BINDING_STRIDE,&glm_ctx->state.var.vertex_binding_stride);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,&glm_ctx->state.var.max_vertex_attrib_relative_offset);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS,&glm_ctx->state.var.max_vertex_attrib_bindings);
    glGetIntegerv(GL_TEXTURE_BINDING_1D,&glm_ctx->state.var.texture_binding_1d);
    glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY,&glm_ctx->state.var.texture_binding_1d_array);
    glGetIntegerv(GL_TEXTURE_BINDING_2D,&glm_ctx->state.var.texture_binding_2d);
    glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY,&glm_ctx->state.var.texture_binding_2d_array);
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE,&glm_ctx->state.var.texture_binding_2d_multisample);
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,&glm_ctx->state.var.texture_binding_2d_multisample_array);
    glGetIntegerv(GL_TEXTURE_BINDING_3D,&glm_ctx->state.var.texture_binding_3d);
    glGetIntegerv(GL_TEXTURE_BINDING_BUFFER,&glm_ctx->state.var.texture_binding_buffer);
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP,&glm_ctx->state.var.texture_binding_cube_map);
    glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE,&glm_ctx->state.var.texture_binding_rectangle);

    CGLSetCurrentContext( NULL );
    CGLDestroyContext( ctx );

    dlclose(OpenGL);
    dlclose(libGL);
}
