// Copyright 2011-2020, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "PsdPch.h"
#include "PsdSyncFileReader.h"

#include "PsdFile.h"

#include <limits>


PSD_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
SyncFileReader::SyncFileReader(File* file)
	: m_file(file)
	, m_position(0ull)
{
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void SyncFileReader::Read(void* buffer, uint64_t count)
{
	// Read normally if value fits into uint32_t
	if (count <= std::numeric_limits<uint32_t>::max())
	{
		// do an asynchronous read, wait until it's finished, and update the file position
		PSD_DISABLE_WARNING(26472)
		File::ReadOperation op = m_file->Read(buffer, static_cast<uint32_t>(count), m_position);
		PSD_ENABLE_WARNING(26472);
		m_file->WaitForRead(op);
	}
	// Read into the buffer using a low and high operation 
	// This should only be happening when, for example, reading a channel larger than 32,768 x 32,768 at 32-bit-depth
	else
	{
		File::ReadOperation op_low = m_file->Read(buffer, std::numeric_limits<uint32_t>::max(), m_position);
		m_file->WaitForRead(op_low);

		const uint64_t toRead = count - std::numeric_limits<uint32_t>::max();
		void* incrementedBuffer = static_cast<char*>(buffer) + std::numeric_limits<uint32_t>::max();

		PSD_DISABLE_WARNING(26472);
		File::ReadOperation op_high = m_file->Read(incrementedBuffer, static_cast<uint32_t>(toRead), m_position);
		PSD_ENABLE_WARNING(26472);
		m_file->WaitForRead(op_high);
	}
	m_position += count;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void SyncFileReader::Skip(uint64_t count)
{
	m_position += count;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void SyncFileReader::SetPosition(uint64_t position)
{
	m_position = position;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
uint64_t SyncFileReader::GetPosition(void) const
{
	return m_position;
}

PSD_NAMESPACE_END
