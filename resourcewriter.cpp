#include "resourcewriter.h"
#include <mylly/core/string.h>

// -------------------------------------------------------------------------------------------------

bool ResourceWriter::BeginWrite(const char *path)
{
	if (IsWritable()) {
		return true;
	}

	m_file = fopen(path, "w");

	if (m_file == nullptr) {
		return false;
	}

	fprintf(m_file, "{\n");
	IncIndent();

	return true;
}

bool ResourceWriter::FinishWrite(void)
{
	if (!IsWritable()) {
		return false;
	}

	DecIndent();
	fprintf(m_file, "}\n");

	bool succeeded = (fclose(m_file) == 0);
	m_file = nullptr;

	return succeeded;
}

void ResourceWriter::BeginArray(const char *key)
{
	if (!IsWritable()) {
		return;
	}

	WriteIndent();
	fprintf(m_file, "%s: [\n", key);

	IncIndent();
}

void ResourceWriter::EndArray(void)
{
	if (!IsWritable()) {
		return;
	}

	DecIndent();
	WriteIndent();

	fprintf(m_file, "],\n");
}

void ResourceWriter::BeginObject(const char *key)
{
	if (!IsWritable()) {
		return;
	}

	WriteIndent();

	if (!string_is_null_or_empty(key)) {
		fprintf(m_file, "%s: {\n", key);
	}
	else {
		fprintf(m_file, "{\n");
	}

	IncIndent();
}

void ResourceWriter::EndObject(void)
{
	if (!IsWritable()) {
		return;
	}

	DecIndent();
	WriteIndent();

	fprintf(m_file, "},\n");
}

void ResourceWriter::Write(const char *key, const char *value) const
{
	if (!IsWritable()) {
		return;
	}

	WriteIndent();
	fprintf(m_file, "%s: \"%s\",\n", key, value);
}

void ResourceWriter::Write(const char *key, float value) const
{
	if (!IsWritable()) {
		return;
	}

	WriteIndent();
	fprintf(m_file, "%s: %.3f,\n", key, value);
}

void ResourceWriter::Write(const char *key, int value) const
{
	if (!IsWritable()) {
		return;
	}

	WriteIndent();
	fprintf(m_file, "%s: %d,\n", key, value);
}

void ResourceWriter::Write(const char *key, bool value) const
{
	if (!IsWritable()) {
		return;
	}
	
	WriteIndent();
	fprintf(m_file, "%s: %s,\n", key, value ? "true" : "false");
}

void ResourceWriter::Write(const char *key, const vec3_t &vector) const
{
	if (!IsWritable()) {
		return;
	}
	
	WriteIndent();
	fprintf(m_file, "%s: [ %.3f, %.3f, %.3f ],\n", key, vector.x, vector.y, vector.z);
}

void ResourceWriter::Write(const char *key, const colour_t &colour) const
{
	if (!IsWritable()) {
		return;
	}
	
	WriteIndent();
	fprintf(m_file, "%s: [ %u, %u, %u, %u ],\n", key, colour.r, colour.g, colour.b, colour.a);
}

void ResourceWriter::WriteIndent(void) const
{
	switch (m_indent) {
		case 1: fprintf(m_file, "\t"); break;
		case 2: fprintf(m_file, "\t\t"); break;
		case 3: fprintf(m_file, "\t\t\t"); break;
		case 4: fprintf(m_file, "\t\t\t\t"); break;
		case 5: fprintf(m_file, "\t\t\t\t\t"); break;
		case 6: fprintf(m_file, "\t\t\t\t\t\t"); break;
		case 7: fprintf(m_file, "\t\t\t\t\t\t\t"); break;
		case 8: fprintf(m_file, "\t\t\t\t\t\t\t\t"); break;
		default: break;
	}
}
