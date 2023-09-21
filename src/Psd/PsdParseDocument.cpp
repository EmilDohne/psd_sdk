// Copyright 2011-2020, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "PsdPch.h"
#include "PsdParseDocument.h"

#include "PsdDocument.h"
#include "PsdSyncFileReader.h"
#include "PsdSyncFileUtil.h"
#include "PsdKey.h"
#include "PsdMemoryUtil.h"
#include "PsdAllocator.h"
#include "PsdFile.h"
#include "PsdLog.h"
#include <cstring>


PSD_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
Document* CreateDocument(File* file, Allocator* allocator)
{
	SyncFileReader reader(file);
	reader.SetPosition(0u);

	// check signature, must be "8BPS"
	{
		const uint32_t signature = fileUtil::ReadFromFileBE<uint32_t>(reader);
		if (signature != util::Key<'8', 'B', 'P', 'S'>::VALUE)
		{
			PSD_ERROR("PsdExtract", "File seems to be corrupt, signature does not match \"8BPS\".");
			return nullptr;
		}
	}

	// check version, must be 1 for PSD or 2 for PSB
	const uint16_t version = fileUtil::ReadFromFileBE<uint16_t>(reader);
	if (version != 1 && version != 2)
	{
		PSD_ERROR("PsdExtract", "File seems to be corrupt, version does not match 1 or 2.");
		return nullptr;
	}

	// check reserved bytes, must be zero
	{
		const uint8_t expected[6] = { 0u, 0u, 0u, 0u, 0u, 0u };
		uint8_t zeroes[6] = {};
		reader.Read(zeroes, 6u);

		if (memcmp(zeroes, expected, sizeof(uint8_t)*6) != 0)
		{
			PSD_ERROR("PsdExtract", "File seems to be corrupt, reserved bytes are not zero.");
			return nullptr;
		}
	}

	Document* document = memoryUtil::Allocate<Document>(allocator);

	// Set the version we grabbed earlier, this will allow for further parsing of PSD/PSB
	document->version = version;

	// read in the number of channels.
	// this is the number of channels contained in the document for all layers, including any alpha channels.
	// e.g. for an RGB document with 3 alpha channels, this would be 3 (RGB) + 3 (Alpha) = 6 channels.
	// however, note that individual layers can have extra channels for transparency masks, vector masks, and user masks.
	// this is different from layer to layer.
	document->channelCount = fileUtil::ReadFromFileBE<uint16_t>(reader);

	// read rest of header information
	document->height = fileUtil::ReadFromFileBE<uint32_t>(reader);
	document->width = fileUtil::ReadFromFileBE<uint32_t>(reader);
	document->bitsPerChannel = fileUtil::ReadFromFileBE<uint16_t>(reader);
	document->colorMode = fileUtil::ReadFromFileBE<uint16_t>(reader);

	// grab offsets into different sections
	{
		const uint32_t length = fileUtil::ReadFromFileBE<uint32_t>(reader);

		document->colorModeDataSection.offset = reader.GetPosition();
		document->colorModeDataSection.length = static_cast<uint64_t>(length);

		reader.Skip(length);
	}
	{
		const uint32_t length = fileUtil::ReadFromFileBE<uint32_t>(reader);

		document->imageResourcesSection.offset = reader.GetPosition();
		document->imageResourcesSection.length = static_cast<uint64_t>(length);

		reader.Skip(length);
	}
	{
		uint64_t length = 0;
		// For PSBs the length marker is 8 bytes instead of 4
		if (document->version == 1)
		{
			length = static_cast<uint64_t>(fileUtil::ReadFromFileBE<uint32_t>(reader));
		}
		else if (document->version == 2)
		{
			length = fileUtil::ReadFromFileBE<uint64_t>(reader);
		}

		document->layerMaskInfoSection.offset = reader.GetPosition();
		document->layerMaskInfoSection.length = length;
		reader.Skip(length);
	}
	{
		// note that the image data section does NOT store its length in the first 4 bytes
		document->imageDataSection.offset = reader.GetPosition();
		document->imageDataSection.length = file->GetSize() - reader.GetPosition();	
	}

	return document;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void DestroyDocument(Document*& document, Allocator* allocator)
{
	PSD_ASSERT_NOT_NULL(document);
	PSD_ASSERT_NOT_NULL(allocator);

	memoryUtil::Free(allocator, document);
}

PSD_NAMESPACE_END
