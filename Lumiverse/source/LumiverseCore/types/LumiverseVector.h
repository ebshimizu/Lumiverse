/*! \file LumiverseFloat.h
* \brief Stores a floating posize_t value in Lumiverse
*/
#ifndef _LumiverseVECTOR_H_
#define _LumiverseVECTOR_H_
#pragma once

#include "../LumiverseType.h"
#include "LumiverseFloat.h"
#include <string>
#include <vector>
#include <sstream>

namespace Lumiverse {
// TODO: better big number
#define BIG_NUMBER 1e30
    
class LumiverseType;
	  /*!
  * \brief Defines a float in Lumiverse
  *
  * This class allows limits to be set on the minimum and maximum values for
  * the variable in question.
  * Overloads for comparison ops and arithmetic ops are located in Lumiverse namespace.
  * \sa Lumiverse
  */
  template<size_t D>
  class LumiverseVector : LumiverseType
  {
  public:
    /*!
    * \brief Constructs a float, default value is 0.
    * 
    * \param val Initial value
    * \param def Default value. When reset() is called, `val` will be set to `def`.
    * \param max Maximum allowed value
    * \param min Minimum allowed value
    * \sa reset()
    */
    LumiverseVector(float *val = NULL, float *def = NULL, float *max = NULL, float *min = NULL);

    /*!
    * \brief Constructs a float with the contents of a different float
    * \param other The other object to copy from
    */
    LumiverseVector(LumiverseVector* other);

    /*!
    * \brief Constructs a float by copying from a generic LumiverseType
    *
    * If the other object isn't actually a float, this function will initialize with default values.
    * \param other The other object to copy from.
    */
    LumiverseVector(LumiverseType* other);

    /*!
    * \brief Destroys the float.
    */
	~LumiverseVector() { };

    /*!
    * \brief Says that this object is a float.
    * \return String with contents: `"float"`
    */
    virtual string getTypeName() {
		std::string result;
		std::stringstream sstm;
		sstm << "vector" << D;
		result = sstm.str();
		return result;
	}

    // Override for =
    void operator=(LumiverseVector& val);

    // Arithmetic overrides
    LumiverseVector& operator+=(LumiverseVector& val);

    LumiverseVector& operator-=(LumiverseVector& val);

    LumiverseVector& operator*=(float val);
    LumiverseVector& operator*=(LumiverseVector& val);

    LumiverseVector& operator/=(float val);
    LumiverseVector& operator/=(LumiverseVector& val);

	LumiverseFloat& operator[]( size_t i ) {
        // assumes all members are in a contiguous block
        // todo assert
        return m_floats[i];
    }

	size_t getDimension() { return D; }

	void resize(size_t dim) { m_floats.reserve(dim); }

	/*! \brief Gets the value of the float 
    * \return Value of the object
    */
	LumiverseFloat getFloat(size_t i) { return m_floats[i]; }

	LumiverseFloat* getFloats() { return &m_floats[0]; }

    /*!
    * \brief Sets the value of the float
    * \param val New value
    */
	void setFloat(size_t i, LumiverseFloat val) { m_floats[i] = val; }

    /*! \brief Gets the value of the float 
    * \return Value of the object
    */
	float getVal(size_t i) { return m_floats[i].getVal(); }

    /*!
    * \brief Sets the value of the float
    * \param val New value
    */
	void setVal(size_t i, float val) { m_floats[i].setVal(val); }

    /*!
    * \brief Set maximum value
    * \param val New maximum value
    */
	void setMax(size_t i, float val) { m_floats[i].setMax(val); }
    
    /*!
    * \brief Get the maximum value
    * \return Maximum value for the float
    */
	float getMax(size_t i) { return m_floats[i].getMax(); }

    /*!
    * \brief Set miniumum value
    * \param val New minimum value
    */
	void setMin(size_t i, float val) { m_floats[i].setMin(val); }
    
    /*!
    * \brief Get the minimum value
    * \return Minimum value for the float
    */
	float getMin(size_t i) { return m_floats[i].getMin(); }

    /*!
    * \brief Set the default value for the float
    * \param val New default value
    */
	void setDefault(size_t i, float val) { m_floats[i].setDefault(val); }
    
    /*!
    * \brief Gets the default value for the float
    * \return Default value
    */
	float getDefault(size_t i) { return m_floats[i].getDefault(); }

    /*!
    * \brief Resets the value to the default value
    */
	virtual void reset() { for (auto& floats : m_floats) floats.reset(); }

	// Converts a float to a JSON object with specified name.
    virtual JSONNode toJSON(string name);
    
    virtual std::string asString();
    
  private:
    /*!
    * \brief Ensures that the value of this float is between min and max.
    */
    inline void clamp();

	std::vector<LumiverseFloat> m_floats;

  };

  // Ops ops ops all overloaded woo

  // Compares two LumiverseVectors. Uses normal float comparison
  template<size_t D>
  inline bool operator==(LumiverseVector<D>& a, LumiverseVector<D>& b) {
	for (size_t i = 0; i < D; i++) {
		if (a.getVal(i) != b.getVal(i))
			return false;
	}

    return true;
  }

  template<size_t D>
  inline bool operator!=(LumiverseVector<D>& a, LumiverseVector<D>& b) {
    return !(a == b);
  }

  // LumiverseFloat uses the normal < op for floats.
  template<size_t D>
  inline bool operator<(LumiverseVector<D>& a, LumiverseVector<D>& b) {
	  for (size_t i = 0; i < D; i++) {
		if (a.getVal(i) >= b.getVal(i))
			return false;
	}

    return true;
  }

  template<size_t D>
  inline bool operator>(LumiverseVector<D>& a, LumiverseVector<D>& b) {
    return b < a;
  }

  template<size_t D>
  inline bool operator<=(LumiverseVector<D>& a, LumiverseVector<D>& b) {
    return !(a > b);
  }

  template<size_t D>
  inline bool operator>=(LumiverseVector<D>& a, LumiverseVector<D>& b) {
    return !(a < b);
  }

  // Arithmetic overrides
  template<size_t D>
  inline LumiverseVector<D> operator+(LumiverseVector<D>& lhs, LumiverseVector<D>& rhs) {
    LumiverseVector<D> val = LumiverseFloat(lhs);
    val += rhs;
    return val;
  }

  template<size_t D>
  inline LumiverseVector<D> operator-(LumiverseVector<D>& lhs, LumiverseVector<D>& rhs) {
    LumiverseVector<D> val = LumiverseFloat(lhs);
    val -= rhs;
    return val;
  }

  template<size_t D>
  inline LumiverseFloat operator*(LumiverseVector<D>& lhs, float rhs) {
    LumiverseVector<D> val = LumiverseVector<D>(lhs);
    val *= rhs;
    return val;
  }

  template<size_t D>
  inline LumiverseVector<D> operator*(LumiverseVector<D>& lhs, LumiverseVector<D>& rhs) {
    LumiverseVector<D> val = LumiverseVector<D>(lhs);
    val *= rhs;
    return val;
  }

  template<size_t D>
  inline LumiverseFloat operator/(LumiverseVector<D>& lhs, float rhs) {
    LumiverseVector<D> val = LumiverseVector<D>(lhs);
    val /= rhs;
    return val;
  }

  template<size_t D>
  inline LumiverseVector<D> operator/(LumiverseVector<D>& lhs, LumiverseVector<D>& rhs) {
    LumiverseVector<D> val = LumiverseVector<D>(lhs);
    val /= rhs;
    return val;
  }

  template<size_t D>
  LumiverseVector<D>::LumiverseVector(float *val, float *def, float *max, float *min) {
	m_floats.clear();
	m_floats.reserve(D);

	float val_s;
	float def_s;
	float max_s;
	float min_s;

	for (size_t i = 0; i < D; i++) {
		val_s = (val) ? val[i] : 0.0f;
		def_s = (def) ? def[i] : 0.0f;
		max_s = (max) ? max[i] : (float)BIG_NUMBER;
		min_s = (min) ? min[i] : -(float)BIG_NUMBER;

		m_floats.push_back(LumiverseFloat(val_s, def_s, max_s, min_s));
	}
}

template<size_t D>
LumiverseVector<D>::LumiverseVector(LumiverseVector* other) {
	size_t i = 0;
	m_floats.clear();
	m_floats.reserve(D);

	for (; i < other->getDimension() && i < D; i++) {
		m_floats.push_back(other->getFloat(i));
	}

	for (; i < D; i++) {
		m_floats.push_back(LumiverseFloat());
	}
}

template<size_t D>
LumiverseVector<D>::LumiverseVector(LumiverseType* other) {
	m_floats.clear();
	m_floats.reserve(D);

	std::string name = other->getTypeName();
	std::string type = name.substr(0, 6);
   if (type != "vector") {
    // If this isn't actually a float, use defaults.
    for (size_t i = 0; i < D; i++) {
		m_floats.push_back(LumiverseFloat());
	}
  }
  else {
	size_t i = 0;
	LumiverseVector<D> *other_ptr = (LumiverseVector<D> *)other;

	for (; i < other_ptr->getDimension() && i < D; i++) {
		m_floats.push_back(other_ptr->getFloat(i));
	}

	for (; i < D; i++) {
		m_floats.push_back(LumiverseFloat());
	}
  }
}

template<size_t D>
void LumiverseVector<D>::clamp() {
  for (size_t i = 0; i < D; i++) {
	  m_floats[i].clamp();
  }
}

// Override for =
template<size_t D>
void LumiverseVector<D>::operator=(LumiverseVector& val) {
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_floats[i] = val.getFloat(i);
	}

	clamp(); 
}

template<size_t D>
JSONNode LumiverseVector<D>::toJSON(string name) {
  JSONNode node;
  node.set_name(name);
  
  node.push_back(JSONNode("type", getTypeName()));
  for (size_t i = 0; i < D; i++) {
    std::stringstream sstm;
    sstm << "v_" << i;
    node.push_back(m_floats[i].toJSON(sstm.str()));
  }
  
  return node;
}

template<size_t D>
std::string LumiverseVector<D>::asString() {
    std::stringstream sstm;
    
    for (size_t i = 0; i < D; i++) {
        if (i > 0)
            sstm << ", ";
        sstm << m_floats[i].asString();
    }
    
    return sstm.str();
}
  
// Arithmetic overrides
template<size_t D>
LumiverseVector<D>& LumiverseVector<D>::operator+=(LumiverseVector& val) {
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_floats[i] += val.getFloat(i);
	}

	return *this; 
}

template<size_t D>
LumiverseVector<D>& LumiverseVector<D>::operator-=(LumiverseVector& val) { 
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_floats[i] -= val.getFloat(i);
	}

	return *this; 
}

template<size_t D>
LumiverseVector<D>& LumiverseVector<D>::operator*=(float val) {
	for (size_t i = 0; i < D; i++) {
		m_floats[i] *= val;
	}
	clamp();
	return *this;
}

template<size_t D>
LumiverseVector<D>& LumiverseVector<D>::operator*=(LumiverseVector& val) { 
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_floats[i] *= val.getFloat(i);
	}

	return *this; 
}

template<size_t D>
LumiverseVector<D>& LumiverseVector<D>::operator/=(float val) {
	for (size_t i = 0; i < D; i++) {
		m_floats[i] /= val;
	}

	clamp(); 
	return *this; 
}

template<size_t D>
LumiverseVector<D>& LumiverseVector<D>::operator/=(LumiverseVector& val) { 
	for (size_t i = 0; i < val.getDimension() && i < D; i++) {
		m_floats[i] /= val.getFloat(i);
	}

	return *this; 
}

}

#endif