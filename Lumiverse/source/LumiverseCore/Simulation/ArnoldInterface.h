/*! \file DMXPatch.h
* \brief Implementation of a patch for a DMX system.
*/
#ifndef _ArnoldINTERFACE_H_
#define _ArnoldINTERFACE_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include <algorithm>
#include <cstdio>
#include <ai.h>
#include "../Patch.h"
#include "../lib/libjson/libjson.h"
#include "ArnoldParameterVector.h"

#include <iostream>

namespace Lumiverse {

  struct ArnoldParam {
	  ArnoldParam()
		  : dimension(1), arnoldTypeName("unknow") { }
      
      size_t dimension;
      std::string arnoldTypeName;
  };

  template<typename T>
  union AiNodeSet {
    void (*AiNodeSet1D)(AtNode *, const char *, T);
    void (*AiNodeSet2D)(AtNode *, const char *, T, T);
    void (*AiNodeSet3D)(AtNode *, const char *, T, T, T);
    void (*AiNodeSet4D)(AtNode *, const char *, T, T, T, T);
  };
    
  template<typename T>
    union AiArraySet {
        bool (*AiArraySet1D) (AtArray*, AtUInt32, T, const char*, int);
        void (*AiNodeSet1D)(AtNode *, const char *, T);
        void (*AiNodeSet2D)(AtNode *, const char *, T, T);
        void (*AiNodeSet3D)(AtNode *, const char *, T, T, T);
        void (*AiNodeSet4D)(AtNode *, const char *, T, T, T, T);
    };
    
  /*!
  * \brief  
  *
  *
  * \sa  
  */
  class ArnoldInterface
  {
  public:
    /*!
    * \brief Constructs a DMXPatch object.
    */
    ArnoldInterface() : m_buffer(NULL), m_gamma(1 / 2.2) { }
      
    /*!
    * \brief Destroys the object.
    */
    ~ArnoldInterface() { delete[] m_buffer; }
      
    /*!
    * \brief Initializes connections and other network settings for the patch.
    *
    * Call this AFTER all interfaces have been assigned. May need to call again
    * if interfaces change.
    */
    void init();

    /*!
    * \brief Closes connections to the interfaces.
    */
    void close();

    /*!
    * \brief Exports a JSONNode with the data in this patch
    *
    * \return JSONNode containing the DMXPatch object
    */
    virtual JSONNode toJSON();

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "DMXPatch"
    */
    virtual string getInterfaceType() { return "ArnoldInterface"; }
      
    int getWidth() { return m_width; }
    int getHeight() { return m_height; }
      
    float *getBufferPointer() { return m_buffer; }

    void setArrayParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value);
      
    void setParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value);
      
    template<size_t D, typename T>
    void setSingleParameter(AtNode *node, const std::string &paramName, const std::string &value,
                            union AiNodeSet<T> aiNodeSet) const;
      
    template<size_t D, typename T, class C>
    void setArrayParameter(AtNode *node, const std::string &paramName, const std::string &value,
                           bool (*AiArraySet) (AtArray*, AtUInt32, C, const char*, int), const int AiType) const;
      
    bool updateLight(set<Device *> devices);
      
    void loadJSON(const JSONNode data);
      
    template<size_t D, typename T>
    void parseArnoldParameter(const std::string &value, ArnoldParameterVector<D, T> &vector) const;
      
    void loadArnoldParam(const JSONNode data);
      
    void setAssFile(std::string fileName) { m_ass_file = fileName; }
    std::string getAssFile() { return m_ass_file; }
      
    void setPluginDirectory(std::string dir) { m_plugin_dir = dir; }
    std::string getPluginDirectory() { return m_plugin_dir; }
      
    void setGamma(float gamma) { m_gamma = gamma; }
    float getGamma() { return m_gamma; }
      
  private:

	map<string, ArnoldParam> m_arnold_params;
    std::string m_ass_file;
    std::string m_plugin_dir;
      
    float *m_buffer;
    int m_width, m_height;
    float m_gamma;
  };
}

#endif
