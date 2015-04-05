#ifndef GLCM_H
#define GLCM_H

#include <vector>

template <typename ValueType = unsigned int>
class GLCM: private std::vector<ValueType>
{
public:
	GLCM(const unsigned int size);
	unsigned int size() const;

	using std::vector<ValueType>::begin;
	using std::vector<ValueType>::end;

	void reset();
	void inc(const unsigned int v1, const unsigned int v2);

	void print();

private:
	unsigned int m_Size;

	unsigned int offset(const unsigned int v1, const unsigned int v2) const;
};

#ifndef ITK_MANUAL_INSTANTIATION
#include "GLCM.hxx"
#endif

#endif /* GLCM_H */
