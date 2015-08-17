/*
 * Vulkan Samples Kit
 *
 * Copyright (C) 2015 LunarG, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
VULKAN_SAMPLE_SHORT_DESCRIPTION
Create Uniform Buffer
*/

#include <util_init.hpp>
#include <assert.h>
#include <string.h>
#include <cstdlib>

int main(int argc, char **argv)
{
    VkResult res;
    struct sample_info info = {};
    char sample_title[] = "Uniform Buffer Sample";

    init_global_layer_properties(info);
    init_instance(info, sample_title);
    init_enumerate_device(info);
    init_device(info);
    res = vkGetPhysicalDeviceMemoryProperties(info.gpu, &info.memory_properties);
    assert(!res);
    info.width = info.height = 50;

    info.Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    info.View       = glm::lookAt(
                          glm::vec3(0,3,10), // Camera is at (0,3,10), in World Space
                          glm::vec3(0,0,0), // and looks at the origin
                          glm::vec3(0,-1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                          );
    info.Model = glm::mat4(1.0f);
    info.MVP = info.Projection * info.View * info.Model;

    /* VULKAN_KEY_START */
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buf_info.size = sizeof(info.MVP);
    res = vkCreateBuffer(info.device, &buf_info, &info.uniform_data.buf);
    assert(!res);

    VkMemoryRequirements mem_reqs;
    res = vkGetBufferMemoryRequirements(info.device, info.uniform_data.buf, &mem_reqs);
    assert(!res);

    VkMemoryAllocInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOC_INFO;
    alloc_info.pNext = NULL;
    alloc_info.memoryTypeIndex = 0;

    alloc_info.allocationSize = mem_reqs.size;
    res = memory_type_from_properties(info,
                                      mem_reqs.memoryTypeBits,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                      &alloc_info.memoryTypeIndex);
    assert(!res);

    res = vkAllocMemory(info.device, &alloc_info, &(info.uniform_data.mem));
    assert(!res);

    uint8_t *pData;
    res = vkMapMemory(info.device, info.uniform_data.mem, 0, 0, 0, (void **) &pData);
    assert(!res);

    memcpy(pData, &info.MVP, sizeof(info.MVP));

    res = vkUnmapMemory(info.device, info.uniform_data.mem);
    assert(!res);

    res = vkBindBufferMemory(info.device,
            info.uniform_data.buf,
            info.uniform_data.mem, 0);
    assert(!res);

    VkBufferViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    view_info.buffer = info.uniform_data.buf;
    view_info.viewType = VK_BUFFER_VIEW_TYPE_RAW;
    view_info.offset = 0;
    view_info.range = sizeof(info.MVP);

    res = vkCreateBufferView(info.device, &view_info, &info.uniform_data.view);
    assert(!res);

    info.uniform_data.desc.bufferView = info.uniform_data.view;
    /* VULKAN_KEY_END */

    vkFreeMemory(info.device, info.uniform_data.mem);
    vkDestroyBufferView(info.device, info.uniform_data.view);
    vkDestroyBuffer(info.device, info.uniform_data.buf);
    vkDestroyDevice(info.device);
}
