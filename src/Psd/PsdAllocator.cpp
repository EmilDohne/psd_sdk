// Copyright 2011-2020, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "PsdPch.h"
#include "PsdLog.h"
#include "PsdAllocator.h"

#include <inttypes.h>
#include <limits>

PSD_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
Allocator::~Allocator(void)
{
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void* Allocator::Allocate(uint64_t size, size_t alignment)
{
	if (size > std::numeric_limits<size_t>::max())
	{
		PSD_ERROR("Allocator", "Size %" PRIu64 " would overflow allocation", size);
		return nullptr;
	}
	return DoAllocate(static_cast<size_t>(size), alignment);
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void Allocator::Free(void* ptr)
{
	DoFree(ptr);
}

PSD_NAMESPACE_END
