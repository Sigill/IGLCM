#ifndef GLCM_HXX
#define GLCM_HXX

#include "GLCM.h"
#include <algorithm>
#include <iostream>

template <typename ValueType>
GLCM<ValueType>::GLCM(const unsigned int size):
	std::vector<ValueType>(size * size),
	m_Size(size)
{}

template <typename ValueType>
unsigned int GLCM<ValueType>::size() const
{
	return this->m_Size;
}

template <typename ValueType>
unsigned int GLCM<ValueType>::offset(const unsigned int v1, const unsigned int v2) const
{
	return v2 * this->m_Size + v1;
}

template <typename ValueType>
void GLCM<ValueType>::reset()
{
    std::fill( this->begin(), this->end(), ValueType() );
}

template <typename ValueType>
void GLCM<ValueType>::inc(const unsigned int v1, const unsigned int v2)
{
	++std::vector<ValueType>::operator[](offset(v1, v2));
}

template <typename ValueType>
void GLCM<ValueType>::print()
{
	for(int i = 0; i < this->m_Size; ++i) {
		for(int j = 0; j < this->m_Size; ++j) {
			std::cout << std::vector<ValueType>::operator[](offset(j, i)) << " ";
		}
		std::cout << std::endl;
	}
}

#endif /* GLCM_HXX */
