// Copyright 2011-2020, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once


PSD_NAMESPACE_BEGIN

/// \ingroup Types
/// \class ExportGroup
/// \brief A struct representing a group as exported to the Layer Mask section.
struct ExportGroup
{
	ExportGroup* parent = nullptr;
	ExportGroupMask* mask;

	// Pointers to children are stored in here
	static const unsigned int MAX_CHILD_COUNT = 128u;
	ExportLayer* childLayers[MAX_CHILD_COUNT];
	uint16_t layerCount;
	ExportGroup* childGroups[MAX_CHILD_COUNT];
	uint16_t groupCount;

	char* name;
};

PSD_NAMESPACE_END
