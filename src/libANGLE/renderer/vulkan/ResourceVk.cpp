//
// Copyright 2017 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Resource:
//    Resource lifetime tracking in the Vulkan back-end.
//

#include "libANGLE/renderer/vulkan/ResourceVk.h"

#include "libANGLE/renderer/vulkan/ContextVk.h"

namespace rx
{
namespace vk
{
// Resource implementation.
Resource::Resource()
{
    mUse.init();
}

Resource::~Resource()
{
    mUse.release();
}

angle::Result Resource::finishRunningCommands(ContextVk *contextVk)
{
    return contextVk->finishToSerial(mUse.getSerial());
}

// SharedGarbage implementation.
SharedGarbage::SharedGarbage() = default;

SharedGarbage::SharedGarbage(SharedGarbage &&other)
{
    *this = std::move(other);
}

SharedGarbage::SharedGarbage(SharedResourceUse &&use, std::vector<GarbageObject> &&garbage)
    : mLifetime(std::move(use)), mGarbage(std::move(garbage))
{}

SharedGarbage::~SharedGarbage() = default;

SharedGarbage &SharedGarbage::operator=(SharedGarbage &&rhs)
{
    std::swap(mLifetime, rhs.mLifetime);
    std::swap(mGarbage, rhs.mGarbage);
    return *this;
}

bool SharedGarbage::destroyIfComplete(VkDevice device, Serial completedSerial)
{
    if (mLifetime.isCurrentlyInUse(completedSerial))
        return false;

    mLifetime.release();

    for (GarbageObject &object : mGarbage)
    {
        object.destroy(device);
    }

    return true;
}

// ResourceUseList implementation.
ResourceUseList::ResourceUseList() = default;

ResourceUseList::~ResourceUseList()
{
    ASSERT(mResourceUses.empty());
}

void ResourceUseList::releaseResourceUses()
{
    for (SharedResourceUse &use : mResourceUses)
    {
        use.release();
    }

    mResourceUses.clear();
}

void ResourceUseList::releaseResourceUsesAndUpdateSerials(Serial serial)
{
    for (SharedResourceUse &use : mResourceUses)
    {
        use.releaseAndUpdateSerial(serial);
    }

    mResourceUses.clear();
}
}  // namespace vk
}  // namespace rx
