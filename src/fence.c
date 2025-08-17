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
 * fence.c
 * MGL
 *
 */

#include <strings.h>

#include "glm_context.h"

Sync *newSync(GLMContext ctx)
{
    Sync *ptr;

    ptr = (Sync *)malloc(sizeof(Sync));
    assert(ptr);

    bzero(ptr, sizeof(Sync));

    ptr->name = STATE(sync_name)++;

    return ptr;
}

int isSync(GLMContext ctx, GLsync sync)
{
    if (sync->name < STATE(sync_name))
        return 1;

    return 0;
}

GLsync mglFenceSync(GLMContext ctx, GLenum condition, GLbitfield flags)
{
    Sync *ptr;

    switch(condition)
    {
        case GL_SYNC_GPU_COMMANDS_COMPLETE:
            break;

        default:
            assert(0);
    }

    // must be zero
    assert(flags == 0);

    ptr = newSync(ctx);

    ctx->mtl_funcs.mtlGetSync(ctx, ptr);

    return ptr;
}


GLboolean mglIsSync(GLMContext ctx, GLsync sync)
{
    if (sync == NULL)
    {
        return false;
    }

    return isSync(ctx, sync);
}

void mglDeleteSync(GLMContext ctx, GLsync sync)
{
    if (isSync(ctx, sync) == GL_FALSE)
    {
        assert(0);

        return;
    }

    if (sync->mtl_event)
    {
        ctx->mtl_funcs.mtlWaitForSync(ctx, sync);

        // should be null
        assert(sync->mtl_event == NULL);
    }

    free(sync);
}

GLenum  mglClientWaitSync(GLMContext ctx, GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    if (flags & ~GL_SYNC_FLUSH_COMMANDS_BIT)
    {
        assert(0);

        return GL_INVALID_VALUE;
    }

    if (isSync(ctx, sync) == GL_FALSE)
    {
        assert(0);

        return GL_INVALID_VALUE;
    }

    if (sync->mtl_event == NULL)
    {
        return GL_ALREADY_SIGNALED;
    }

    ctx->mtl_funcs.mtlWaitForSync(ctx, sync);

    assert(sync->mtl_event == NULL);

    return GL_CONDITION_SATISFIED;
}

void mglWaitSync(GLMContext ctx, GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    if (isSync(ctx, sync) == GL_FALSE)
    {
        assert(0);
    }

    assert(timeout == GL_TIMEOUT_IGNORED);

    ctx->mtl_funcs.mtlWaitForSync(ctx, sync);

    assert(sync->mtl_event == NULL);
}

void mglGetSynciv(GLMContext ctx, GLsync sync, GLenum pname, GLsizei count, GLsizei *length, GLint *values)
{
    if (isSync(ctx, sync) == GL_FALSE)
    {
        assert(0);
    }

    assert(count);
    assert(length);
    assert(values);

    if (*length < count * sizeof(GLuint))
    {
        assert(0);
    }

    while(count--)
    {
        switch(pname)
        {
            case GL_OBJECT_TYPE:
                *values = GL_SYNC_FENCE;
                break;

            case GL_SYNC_STATUS:
                if (sync->mtl_event)
                    *values = GL_UNSIGNALED;
                else
                    *values = GL_SIGNALED;
                break;

            case GL_SYNC_CONDITION:
                if (sync->mtl_event == NULL)
                    *values = GL_SYNC_GPU_COMMANDS_COMPLETE;
                break;

            case GL_SYNC_FLAGS:
                *values = 0;
                break;

            default:
                assert(0);
                break;
        }

        values++;
    }
}

void mglTextureBarrier(GLMContext ctx)
{
    // Unimplemented function
    assert(0);
}

void mglMemoryBarrier(GLMContext ctx, GLbitfield barriers)
{
    if (barriers & ~(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_ELEMENT_ARRAY_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT |  GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_COMMAND_BARRIER_BIT | GL_PIXEL_BUFFER_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT | GL_FRAMEBUFFER_BARRIER_BIT | GL_TRANSFORM_FEEDBACK_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT))
    {
        // extra bits...
        ERROR_RETURN(GL_INVALID_VALUE);
    }
}

void mglMemoryBarrierByRegion(GLMContext ctx, GLbitfield barriers)
{

    if (barriers & ~(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_FRAMEBUFFER_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT))
    {
        // extra bits...
        ERROR_RETURN(GL_INVALID_VALUE);
    }
}

