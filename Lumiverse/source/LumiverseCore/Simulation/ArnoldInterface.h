/*! \file ArnoldInterface.h
* \brief Implementation of a interface between ArnoldPatch and arnold.
*/
#ifndef _ArnoldINTERFACE_H_
#define _ArnoldINTERFACE_H_

#pragma once

#include "LumiverseCoreConfig.h"

#ifdef USE_ARNOLD

#include <algorithm>
#include <cstdio>
#include "../lib/arnold/include/ai.h"
#include "../Patch.h"
#include "../lib/libjson/libjson.h"
#include "ArnoldParameterVector.h"

#include <iostream>

namespace Lumiverse {

  /*! \brief Wrapper struct for a arnold parameter. */
  struct ArnoldParam {
	  ArnoldParam()
		  : dimension(1), arnoldTypeName("unknown") { }
      
      size_t dimension;
      std::string arnoldTypeName;
  };

  /*! \brief Wrapper union used as a argument to parameter setter functions. */
  template<typename T>
  union AiNodeSetter {
    void (*AiNodeSetter1D)(AtNode *, const char *, T);
    void (*AiNodeSetter2D)(AtNode *, const char *, T, T);
    void (*AiNodeSetter3D)(AtNode *, const char *, T, T, T);
    void (*AiNodeSetter4D)(AtNode *, const char *, T, T, T, T);
  };
    
  struct BucketPositionInfo {
	  int bucket_xo;
	  int bucket_yo;
	  int bucket_size_x;
	  int bucket_size_y;

	  BucketPositionInfo() :
		  bucket_xo(-1), bucket_yo(-1), bucket_size_x(-1), bucket_size_y(-1) { }
  };

  struct ProgressInfo {
	  int bucket_sum;
	  int bucket_cur;

	  ProgressInfo() : bucket_sum(-1), bucket_cur(-1) { }
  };

  /*!
  * \brief Interface between ArnoldPatch and arnold. Almost all arnold APIs are called from this class.
  *
  * ArnoldInterface is mainly responsible to configure light node of arnold. It keeps a list of mappings
  * from metadata ids to Arnold types. ArnoldInterface also creates and closes Arnold session.
  * \sa ArnoldPatch
  */
  class ArnoldInterface
  {
  public:
    /*!
    * \brief Constructs a ArnoldInterface object.
    *
    * The frame buffer point is set to NULL. The buffer will be initialized after getting the size of output.
    * The default gamma is 2.2.
    */
	ArnoldInterface() : m_buffer(NULL), m_gamma(2.2), m_samples(-3), 
		  m_bucket_pos(NULL), m_bucket_num(0) { }
      
    /*!
    * \brief Destroys the object.
    */
	virtual ~ArnoldInterface() { delete[] m_buffer; delete[] m_bucket_pos; m_bucket_num = 0; }
      
    /*!
    * \brief Initializes the Arnold renderer.
    *
    * Opens a new Arnold session. Loads ass file and the plugin (buffer_driver).
    */
    void init();

    /*!
    * \brief Closes the Arnold session.
    */
    void close();

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "ArnoldInterface"
    */
    virtual string getInterfaceType() { return "ArnoldInterface"; }
      
    /*!
    * \brief Gets the width of result.
    *
    * \return The width of result
    */
    int getWidth() { return m_width; }
      
    /*!
    * \brief Gets the height of result.
    *
    * \return The height of result
    */
    int getHeight() { return m_height; }
    
    /*!
    * \brief Gets the pointer to the frame buffer.
    *
    * \return The pointer to the frame buffer.
    */
    float *getBufferPointer() { return m_buffer; }

    /*!
    * \brief Sets a parameter light node according to a key-value pair of metadata.
    *
    * The value string should follow the format "v1, v2, ..., vn" for a n-dimensional value
    * or "v1, v2, ..., vn; v1', v2', ..., vn'; ..." for an array or matrix.
    * \param light_ptr The pointer to a light node.
    * \param paramName The name of the parameter, which is also the name of the metadata.
    * \param value The value (in string) of the parameter, which is also the value of the metadata.
    * \sa setArrayParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value)
    */
    void setParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value);
      
    /*!
    * \brief Sets a parameter with AtArray type of light node according to a key-value pair of metadata.
    *
    * The value string should follow the format "v1, v2, ..., vn; v1', v2', ..., vn'; ..." for an array.
    * \param light_ptr The pointer to a light node.
    * \param paramName The name of the parameter, which is also the name of the metadata.
    * \param value The value (in string) of the parameter, which is also the value of the metadata.
    * \sa setParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value)
    */
    void setArrayParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value);
      
    /*!
    * \brief Loads a arnold parameter mapping into the list.
    *
    * \param data The json node containing the mapping.
    */
    void loadArnoldParam(const JSONNode data);
      
    /*!
    * \brief Sets the path to ass file.
    *
    * \param fileName The path to ass file
    */
    void setAssFile(std::string fileName) { m_ass_file = fileName; }
      
    /*!
    * \brief Gets the path to ass file.
    *
    * \return The path to ass file
    */
    std::string getAssFile() { return m_ass_file; }
      
    /*!
    * \brief Sets the directory containing the plugin (buffer_driver).
    *
    * \param dir The directory containing the plugin
    */
    void setPluginDirectory(std::string dir) { m_plugin_dir = dir; }
      
    /*!
    * \brief Gets the directory containing the plugin (buffer_driver).
    *
    * \return The directory containing the plugin
    */
    std::string getPluginDirectory() { return m_plugin_dir; }
      
    /*!
    * \brief Sets the gamma.
    *
    * \param gamma The gamma.
    */
    void setGamma(float gamma) { m_gamma = gamma; }
      
    /*!
    * \brief Gets the gamma.
    *
    * \return The gamma.
    */
    float getGamma() { return m_gamma; }
    
	/*!
	* \brief Sets the camera sampling rate used for current rendering.
	*
	* Although the system may have multiple camera sampling rates (e.g. for interactive mode and for real rendering),
	* this rate will be used for the current rendering.
	* \param samples The sampling rate.
	*/
    void setSamples(int samples) { m_samples = samples; }
      
	/*!
	* \brief Gets the sampling rate.
	*
	* \return The sampling rate.
	*/
    int getSamples() { return m_samples; }
      
    /*!
    * \brief Starts rendering with Arnold.
    * Returns the error code of AiRender, so the caller can know if the renderer was interrupted.
    * \return Error code of arnold.
    */
    int render();
      
    /*!
    * \brief Interrupts current rendering.
    */
    void interrupt();

	/*!
	* \brief Parses the arnold parameter map to a JSON node.
	* \return The Json node.
	*/
	JSONNode arnoldParameterToJSON();

	float getPercentage() const { 
		if (m_progress.bucket_sum < 0)
			return 0.f;
		return (0.f + m_progress.bucket_cur) / m_progress.bucket_sum * 100.f; 
	}

	BucketPositionInfo *getBucketPositionInfo() const { return m_bucket_pos; }

	size_t getBucketNumber() const { return m_bucket_num; }
      
  private:
    /*!
    * \brief Helper function to sets a arnold non-array parameter.
    *
    * The function uses a union contains the function pointer to the exact API (may has variant signature).
    * \param node The pointer to a light node.
    * \param paramName The name of the parameter, which is also the name of the metadata.
    * \param value The value (in string) of the parameter, which is also the value of the metadata.
    * \param aiNodeSetter The union contains the function pointer to the exact API (may has variant signature).
    * \sa setArrayParameter(AtNode *node, const std::string &paramName, const std::string &value, bool (*AiArraySet) (AtArray*, AtUInt32, C, const char*, int), const int AiType)
    */
    template<size_t D, typename T>
    void setSingleParameter(AtNode *node, const std::string &paramName, const std::string &value,
                            union AiNodeSetter<T> aiNodeSetter) const;
    
    /*!
    * \brief Helper function to sets a arnold array parameter.
    *
    * The function uses a function pointer to the exact API and a int value to specify the element type of the array.
    * \param node The pointer to a light node.
    * \param paramName The name of the parameter, which is also the name of the metadata.
    * \param value The value (in string) of the parameter, which is also the value of the metadata.
    * \param AiArraySet The function pointer to the exact API.
    * \param AiType The int value indicating the element type of the array.
    * \sa setSingleParameter(AtNode *node, const std::string &paramName, const std::string &value, union AiNodeSetter<T> aiNodeSetter)
    */
    template<size_t D, typename T, class C>
    void setArrayParameter(AtNode *node, const std::string &paramName, const std::string &value,
                           bool (*AiArraySet) (AtArray*, AtUInt32, C, const char*, int), const int AiType) const;
            
    /*!
    * \brief Appends the new output command to the outputs attribute of options.
    *
    * \param buffer_output The output command (typically using a driver_buffer node).
    */
    void appendToOutputs(const std::string buffer_output);

	/*!
	* \brief Appends the new output command to the outputs attribute of options.
	*/
	void setSamplesOption();
      
    /*!
    * \brief The list containing the mappings between metadata to Arnold parameter.
    * \sa ArnoldParam
    */
	map<string, ArnoldParam> m_arnold_params;
      
    /*!
    * \brief The path to ass file.
    */
    std::string m_ass_file;
      
    /*!
    * \brief The directory containing the plugin (buffer_driver).
    */
    std::string m_plugin_dir;
      
    /*!
    * \brief The pointer to the frame buffer.
    */
    float *m_buffer;
    
    /*!
    * \brief The width of the result.
    */
    int m_width;
      
    /*!
    * \brief The height of the result.
    */
    int m_height;
      
    /*!
    * \brief The gamma for gamma correction.
    */
    float m_gamma;
      
    /*!
    * \brief Arnold AA samples
    */
    int m_samples;

	BucketPositionInfo *m_bucket_pos;
	size_t m_bucket_num;
	ProgressInfo m_progress;
  };

  /*!
  * \brief Parses a formatted string into a ArnoldParameterVector instance.
  *
  * \param value A formatted string.
  * \param vector The returned vector.
  */
  template<size_t D, typename T>
  static void parseArnoldParameter(const std::string &value, ArnoldParameterVector<D, T> &vector) {
	  T element;
	  std::string value_spaceless = value;

	  // Removes spaces when the input type is not string
	  if (typeid(std::string) != typeid(T))
		  std::remove_if(value_spaceless.begin(), value_spaceless.end(),
		  [](char x){return std::isspace(x); });

	  // Format: "v1, v2, ..."
	  size_t offset = 0;
	  for (size_t i = 0; i < D; i++) {
		  std::istringstream iss(value_spaceless.substr(offset));
		  iss >> element;
		  vector[i] = element;

		  offset = value_spaceless.find(",", offset);

		  if (offset == std::string::npos)
			  break;
		  offset++;
	  }
  }
}

#endif

#endif
