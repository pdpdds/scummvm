#include "StdAfx.h"
#include "XMLSerializer.h"
#include "tinyxml.h"

#include "SFXMLStreamWriter.h"
#include "SFXMLStreamReader.h"


XMLSerializer::XMLSerializer(void)
{
}

XMLSerializer::~XMLSerializer(void)
{
}

bool XMLSerializer::Initialize()
{
	m_ShouterInfo.Clear();

	return TRUE;
}

bool XMLSerializer::Read(const wchar_t* pFileName)
{
	IXMLStreamReader* pReader = SFXMLStreamReader::CreateXmlStreamReader(pFileName);

	if(NULL == pReader)
		return FALSE;

	pReader->Read(pFileName, *this);

	delete pReader;

	return TRUE;
}

bool XMLSerializer::Write(const wchar_t* pFileName)
{
	IXMLStreamWriter* pWriter = SFXMLStreamWriter::CreateXmlStreamWriter(pFileName);
	if(NULL == pWriter)
		return FALSE;

	pWriter->Write(pFileName, *this);

	delete pWriter;

	return TRUE;
}

