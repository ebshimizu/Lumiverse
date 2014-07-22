/*! \file ArnoldParameterVector.h
* \brief Stores a vector used by ArnoldPatch
*/
#ifndef _ArnoldParameterVECTOR_H_
#define _ArnoldParameterVECTOR_H_
#pragma once

#include <string>
#include <deque>
#include <sstream>

namespace Lumiverse {
  /*!
  * \brief Defines a vector type for Arnold parameters, like color, vector etc.
  *
  * This class is used by ArnoldInterface to temporarily contain value and the corresponding
  * arnold parameter name.
  * \tparam D Dimension
  * \tparam T Value type
  * \sa ArnoldInterface
  */
  template<size_t D, typename T>
  class ArnoldParameterVector
  {
  public:
    /*!
    * \brief Constructs a vector with the values passed as an array.
    * 
    * If the pointer is NULL, uses 0 to fill the D-dimensional vector.
    * \param val A pointer to an array
    * \sa reset()
    */
    ArnoldParameterVector(T *val = NULL);

    /*!
    * \brief Constructs a vector with the contents of a different vector
    *
    * If the other vector has a dimension less than D, then only part of this
    * vector would get rewritten.
    * \param other The other object to copy from
    */
    ArnoldParameterVector(ArnoldParameterVector* other);

    /*!
    * \brief Destroys the vector.
    */
    ~ArnoldParameterVector() { };

    /*!
    * \brief Says that this object is a vector for a arnold parameter.
    * \return String with contents: `"arnold type name" + "dimension"`
    */
    string getTypeName() {
		std::string result;
		std::stringstream sstm;
		sstm << m_arnoldTypeName << D;
		result = sstm.str();
		return result;
	}

    // Override for =
    void operator=(ArnoldParameterVector& val);

    // Arithmetic overrides
    ArnoldParameterVector& operator+=(ArnoldParameterVector& val);

    ArnoldParameterVector& operator-=(ArnoldParameterVector& val);

    ArnoldParameterVector& operator*=(float val);
    ArnoldParameterVector& operator*=(ArnoldParameterVector& val);

    ArnoldParameterVector& operator/=(float val);
    ArnoldParameterVector& operator/=(ArnoldParameterVector& val);

    T& operator[]( size_t i ) {
        // assumes all members are in a contiguous block
        // todo assert
        return m_elements[i];
    }

    /*!
    * \brief Returns the dimension of this vector.
    * \return The dimension
    */
    size_t getDimension() { return D; }

    /*!
    * \brief Resizes the vector to a specific dimension.
    * \param dim The dimension
    */
    void resize(size_t dim) { m_elements.reserve(dim); }

    /*! 
     * \brief Gets the nth element
     * \param i The n
     * \return Value of the element
     */
    T getElement(size_t i) { return m_elements[i]; }

    /*! 
     * \brief Gets the pointer to corresponding array.
     * \return The pointer to the value array.
     */
    T* getElements() { return &m_elements[0]; }

    /*!
    * \brief Sets the value of a element
    * \param i A specific dimension
    * \param val New value
    */
    void setElement(size_t i, T val) { m_elements[i] = val; }
      
  private:
    /*!
    * \brief The actual array contains the elements.
    */
    T m_elements[D];

    /*!
    * \brief The type name of arnold parameter.
    */
    std::string m_arnoldTypeName;
  };

  // Ops ops ops all overloaded woo

  // Compares two ArnoldParameterVectors. Uses normal float comparison
  template<size_t D, typename T>
  inline bool operator==(ArnoldParameterVector<D, T>& a, ArnoldParameterVector<D, T>& b) {
	for (size_t i = 0; i < D; i++) {
		if (a.getElement(i) != b.getElement(i))
			return false;
	}

    return true;
  }

  template<size_t D, typename T>
  inline bool operator!=(ArnoldParameterVector<D, T>& a, ArnoldParameterVector<D, T>& b) {
    return !(a == b);
  }

  // Element uses the normal < op for floats.
  template<size_t D, typename T>
  inline bool operator<(ArnoldParameterVector<D, T>& a, ArnoldParameterVector<D, T>& b) {
	  for (size_t i = 0; i < D; i++) {
		if (a.getElement(i) >= b.getElement(i))
			return false;
	}

    return true;
  }

  template<size_t D, typename T>
  inline bool operator>(ArnoldParameterVector<D, T>& a, ArnoldParameterVector<D, T>& b) {
    return b < a;
  }

  template<size_t D, typename T>
  inline bool operator<=(ArnoldParameterVector<D, T>& a, ArnoldParameterVector<D, T>& b) {
    return !(a > b);
  }

  template<size_t D, typename T>
  inline bool operator>=(ArnoldParameterVector<D, T>& a, ArnoldParameterVector<D, T>& b) {
    return !(a < b);
  }

  // Arithmetic overrides
  template<size_t D, typename T>
  inline ArnoldParameterVector<D, T> operator+(ArnoldParameterVector<D, T>& lhs, ArnoldParameterVector<D, T>& rhs) {
    ArnoldParameterVector<D, T> val = ArnoldParameterVector<D, T>(lhs);

    for (size_t i = 0; i < D; i++) {
	val.getElement(i) += rhs.getElement(i);
    }

    return val;
  }

  template<size_t D, typename T>
  inline ArnoldParameterVector<D, T> operator-(ArnoldParameterVector<D, T>& lhs, ArnoldParameterVector<D, T>& rhs) {
    ArnoldParameterVector<D, T> val = ArnoldParameterVector<D, T>(lhs);

    for (size_t i = 0; i < D; i++) {
	val.getElement(i) -= rhs.getElement(i);
    }

    return val;
  }

  template<size_t D, typename T>
  inline T operator*(ArnoldParameterVector<D, T>& lhs, float rhs) {
    ArnoldParameterVector<D, T> val = ArnoldParameterVector<D, T>(lhs);
    val *= rhs;
    return val;
  }

  template<size_t D, typename T>
  inline ArnoldParameterVector<D, T> operator*(ArnoldParameterVector<D, T>& lhs, ArnoldParameterVector<D, T>& rhs) {
    ArnoldParameterVector<D, T> val = ArnoldParameterVector<D, T>(lhs);
    val *= rhs;
    return val;
  }

  template<size_t D, typename T>
  inline T operator/(ArnoldParameterVector<D, T>& lhs, float rhs) {
    ArnoldParameterVector<D, T> val = ArnoldParameterVector<D, T>(lhs);
    val /= rhs;
    return val;
  }

  template<size_t D, typename T>
  inline ArnoldParameterVector<D, T> operator/(ArnoldParameterVector<D, T>& lhs, ArnoldParameterVector<D, T>& rhs) {
    ArnoldParameterVector<D, T> val = ArnoldParameterVector<D, T>(lhs);
    val /= rhs;
    return val;
  }

  template<size_t D, typename T>
  ArnoldParameterVector<D, T>::ArnoldParameterVector(T *val) {
	T val_s;

	for (size_t i = 0; i < D; i++) {
		val_s = (val) ? val[i] : 0;

		m_elements[i] = val_s;
	}
}

template<size_t D, typename T>
ArnoldParameterVector<D, T>::ArnoldParameterVector(ArnoldParameterVector* other) {
	size_t i = 0;
	m_elements.clear();
	m_elements.reserve(D);

	for (; i < other->getDimension() && i < D; i++) {
		m_elements.push_back(other->getElement(i));
	}

	for (; i < D; i++) {
		m_elements.push_back(T());
	}
}

// Override for =
template<size_t D, typename T>
void ArnoldParameterVector<D, T>::operator=(ArnoldParameterVector& val) {
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_elements[i] = val.getElement(i);
	}
}
  
// Arithmetic overrides
template<size_t D, typename T>
ArnoldParameterVector<D, T>& ArnoldParameterVector<D, T>::operator+=(ArnoldParameterVector& val) {
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_elements[i] += val.getElement(i);
	}

	return *this; 
}

template<size_t D, typename T>
ArnoldParameterVector<D, T>& ArnoldParameterVector<D, T>::operator-=(ArnoldParameterVector& val) { 
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_elements[i] -= val.getElement(i);
	}

	return *this; 
}

template<size_t D, typename T>
ArnoldParameterVector<D, T>& ArnoldParameterVector<D, T>::operator*=(float val) {
	for (size_t i = 0; i < D; i++) {
		m_elements[i] *= val;
	}
    
	return *this;
}

template<size_t D, typename T>
ArnoldParameterVector<D, T>& ArnoldParameterVector<D, T>::operator*=(ArnoldParameterVector& val) { 
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_elements[i] *= val.getElement(i);
	}

	return *this; 
}

template<size_t D, typename T>
ArnoldParameterVector<D, T>& ArnoldParameterVector<D, T>::operator/=(float val) {
	for (size_t i = 0; i < D; i++) {
		m_elements[i] /= val;
	}

	return *this; 
}

template<size_t D, typename T>
ArnoldParameterVector<D, T>& ArnoldParameterVector<D, T>::operator/=(ArnoldParameterVector& val) { 
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_elements[i] /= val.getElement(i);
	}

	return *this; 
}

}

#endif
