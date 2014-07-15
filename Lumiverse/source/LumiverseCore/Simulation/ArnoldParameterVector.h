/*! \file ArnoldParameterVector.h
* \brief Stores a floating posize_t value in Lumiverse
*/
#ifndef _ArnoldParameterVECTOR_H_
#define _ArnoldParameterVECTOR_H_
#pragma once

#include <string>
#include <deque>
#include <sstream>

namespace Lumiverse {
// TODO: better big number
#define BIG_NUMBER 1e30
    
	  /*!
  * \brief Defines a float in Lumiverse
  *
  * This class allows limits to be set on the minimum and maximum values for
  * the variable in question.
  * Overloads for comparison ops and arithmetic ops are located in Lumiverse namespace.
  * \sa Lumiverse
  */
  template<size_t D, typename T>
  class ArnoldParameterVector
  {
  public:
    typedef T Element;
    /*!
    * \brief Constructs a float, default value is 0.
    * 
    * \param val Initial value
    * \param def Default value. When reset() is called, `val` will be set to `def`.
    * \param max Maximum allowed value
    * \param min Minimum allowed value
    * \sa reset()
    */
    ArnoldParameterVector(T *val = NULL);

    /*!
    * \brief Constructs a float with the contents of a different float
    * \param other The other object to copy from
    */
    ArnoldParameterVector(ArnoldParameterVector* other);

    /*!
    * \brief Destroys the float.
    */
	~ArnoldParameterVector() { };

    /*!
    * \brief Says that this object is a float.
    * \return String with contents: `"float"`
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

	Element& operator[]( size_t i ) {
        // assumes all members are in a contiguous block
        // todo assert
        return m_elements[i];
    }

	size_t getDimension() { return D; }

	void resize(size_t dim) { m_elements.reserve(dim); }

	/*! \brief Gets the value of the float 
    * \return Value of the object
    */
	Element getElement(size_t i) { return m_elements[i]; }

	Element* getElements() { return &m_elements[0]; }

    /*!
    * \brief Sets the value of the float
    * \param val New value
    */
	void setElement(size_t i, Element val) { m_elements[i] = val; }
      
  private:

	Element m_elements[D];

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
    ArnoldParameterVector<D, T> val = Element(lhs);
    val += rhs;
    return val;
  }

  template<size_t D, typename T>
  inline ArnoldParameterVector<D, T> operator-(ArnoldParameterVector<D, T>& lhs, ArnoldParameterVector<D, T>& rhs) {
    ArnoldParameterVector<D, T> val = Element(lhs);
    val -= rhs;
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
		val_s = (val) ? val[i] : 0.0f;

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
		m_elements.push_back(Element());
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