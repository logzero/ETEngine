#include "stdafx.h"

#include "PackageWriter.h"

#include <EtCore/FileSystem/Entry.h>
#include <EtCore/FileSystem/FileUtil.h>


namespace et {
namespace cooker {


//===================================
// Package Writer :: File Entry Info
//===================================


//---------------------------------------------
// PackageWriter::FileEntryInfo::FileEntryInfo
//
// File entry info constructor that assigns all members
//
PackageWriter::FileEntryInfo::FileEntryInfo(core::PkgEntry const& lEntry, core::File* const lFile, std::string const& lRelName) 
	: entry(lEntry)
	, file(lFile)
	, relName(lRelName)
{ }


//======================
// Package Writer
//======================


//---------------------------------
// PackageWriter::d-tor
//
PackageWriter::~PackageWriter()
{
	Cleanup();
}

//---------------------------------
// PackageWriter::AddFile
//
// Add a file to the writer and create a package entry for it - takes ownership
//
void PackageWriter::AddFile(core::File* const file, std::string const& rootDir, core::E_CompressionType const compression)
{
	m_Files.emplace_back(core::PkgEntry(), file, std::string());
	core::PkgEntry& entry = m_Files[m_Files.size() - 1].entry;
	std::string& relName = m_Files[m_Files.size() - 1].relName;

	// assign the name the relative path of the file compared to the root directory of the package writer
	relName = core::FileUtil::GetRelativePath(file->GetName(), rootDir);

	entry.fileId = GetHash(relName);
	entry.compressionType = compression; 
	entry.nameLength = static_cast<uint16>(relName.size());

	// try opening the file for now, so that we can get the size, keep it open for copying content later
	if (!file->Open(core::FILE_ACCESS_MODE::Read))
	{
		LOG("PackageWriter::AddFile > unable to open file '" + relName + std::string("'!"), core::LogLevel::Warning);
		return;
	}

	// get the file size
	entry.size = file->GetSize();
}

//---------------------------------
// PackageWriter::RemoveFile
//
// Remove a file without deleting it
//
void PackageWriter::RemoveFile(core::File* const file)
{
	auto entryIt = std::find_if(m_Files.begin(), m_Files.end(), [file](FileEntryInfo const& info)
		{
			return info.file == file;
		});

	if (entryIt != m_Files.cend())
	{
		m_Files.erase(entryIt);
	}
}

//---------------------------------
// PackageWriter::Cleanup
//
// Close all files
//
void PackageWriter::Cleanup()
{
	for (FileEntryInfo& entryFile : m_Files)
	{
		if (entryFile.file->IsOpen())
		{
			entryFile.file->Close();

			// delete the file
			delete entryFile.file;
			entryFile.file = nullptr;
		}
	}

	m_Files.clear();
}

//---------------------------------
// PackageWriter::Write
//
// Write the listed files to the data vector
//
void PackageWriter::Write(std::vector<uint8>& data)
{
	// we do a first pass where we figure out the relative offset for all files
	//---------------------------
	uint64 offset = 0u;

	// as we go we can figure out the header
	core::PkgHeader header;
	header.numEntries = static_cast<uint64>(m_Files.size());
	offset += static_cast<uint64>(sizeof(core::PkgHeader));

	// and the size that our central directory will be
	offset += static_cast<uint64>(sizeof(core::PkgFileInfo)) * header.numEntries;

	// as we go we can fill in the entry information
	std::vector<core::PkgFileInfo> fileInfos;
	for (FileEntryInfo& entryFile : m_Files)
	{
		core::PkgFileInfo info;
		info.fileId = entryFile.entry.fileId;
		info.offset = offset;

		// we already know the offset so we can add the file entry
		fileInfos.emplace_back(info);

		// accumulate our offset
		core::PkgEntry& entry = entryFile.entry;
		offset += sizeof(core::PkgEntry) + static_cast<uint64>(entry.nameLength) + entry.size;
	}

	// now we know the total size of the package file will be our offset, so we can initialize our data vector starting with the right size
	//---------------------------
	data = std::vector<uint8>(static_cast<size_t>(offset), 0u);
	uint8* raw = data.data();

	// finally we can write our data to the vector
	//---------------------------

	// header
	memcpy(raw, &header, sizeof(core::PkgHeader));
	offset = sizeof(core::PkgHeader);	// we can reuse it now that our vector has bin initialized

	// central dir
	for (core::PkgFileInfo const& info : fileInfos)
	{
		memcpy(raw + offset, &info, sizeof(core::PkgFileInfo));
		offset += sizeof(core::PkgFileInfo);
	}

	// files
	for (size_t entryIndex = 0u; entryIndex < m_Files.size(); ++entryIndex)
	{
		FileEntryInfo& entryFile = m_Files[entryIndex];

		if (offset != fileInfos[entryIndex].offset)
		{
			LOG("PackageWriter::Write > Entry offset doesn't match expected offset - " + entryFile.relName, core::LogLevel::Error);
		}

		// copy the entry
		memcpy(raw + offset, &entryFile.entry, sizeof(core::PkgEntry));
		offset += sizeof(core::PkgEntry);

		// copy the file name string
		if (entryFile.entry.nameLength != entryFile.relName.size())
		{
			LOG("PackageWriter::Write > Entry name length doesn't match file name length - " + entryFile.relName, core::LogLevel::Error);
		}

		memcpy(raw + offset, entryFile.relName.c_str(), entryFile.entry.nameLength);
		offset += entryFile.entry.nameLength;

		// copy the file content
		std::vector<uint8> fileContent = entryFile.file->Read();

		if (entryFile.entry.size != static_cast<uint64>(fileContent.size()))
		{
			LOG("PackageWriter::Write > Entry size doesn't match read file contents size - " + entryFile.relName, core::LogLevel::Error);
		}

		memcpy(raw + offset, fileContent.data(), entryFile.entry.size);
		offset += entryFile.entry.size;
	}
}


} // namespace cooker
} // namespace et
