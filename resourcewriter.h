#pragma once

#include <mylly/math/vector.h>
#include <mylly/renderer/colour.h>

// -------------------------------------------------------------------------------------------------

class ResourceWriter
{
public:
	bool BeginWrite(const char *path);
	bool FinishWrite(void);

	void BeginArray(const char *key);
	void EndArray(void);

	void BeginObject(const char *key = nullptr);
	void EndObject(void);

	void Write(const char *key, const char *value) const;
	void Write(const char *key, float value) const;
	void Write(const char *key, int value) const;
	void Write(const char *key, bool value) const;
	void Write(const char *key, const vec3_t &vector) const;
	void Write(const char *key, const colour_t &colour) const;

	void IncIndent(void) { m_indent++; }
	void DecIndent(void) { if (m_indent > 0) m_indent--; }

private:
	bool IsWritable(void) const { return (m_file != nullptr); }

	void WriteIndent(void) const;

private:
	FILE *m_file = nullptr;
	int m_indent = 0;
};
