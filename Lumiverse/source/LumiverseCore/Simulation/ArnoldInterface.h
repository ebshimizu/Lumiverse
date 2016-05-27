/*! \file ArnoldInterface.h
* \brief Implementation of a interface between ArnoldPatch and arnold.
*/
#ifndef _ArnoldINTERFACE_H_
#define _ArnoldINTERFACE_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include <algorithm>
#include <cstdio>

#ifdef USE_ARNOLD
  #include "../lib/arnold/include/ai.h"
#endif

#include "../Patch.h"
#include "../lib/libjson/libjson.h"
#include "ArnoldParameterVector.h"
#include <thread>
#include <iostream>
#include <locale>
#include <set>

namespace Lumiverse {
#ifdef USE_ARNOLD
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
#endif

  /*! \brief Wrapper for unit (bucket) being rendered. */
  struct BucketPositionInfo {
	  int bucket_xo;
	  int bucket_yo;
	  int bucket_size_x;
	  int bucket_size_y;

	  BucketPositionInfo() :
		  bucket_xo(-1), bucket_yo(-1), bucket_size_x(-1), bucket_size_y(-1) { }
  };

  /*! \brief Wrapper for progress information computed based on number of buckets. */
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
     ArnoldInterface() : m_buffer(NULL), m_gamma(2.2f), m_samples(-3), m_predictive(false),
		   m_bucket_pos(NULL), m_bucket_num(0), m_open(false) { }
      
    /*!
      \brief Destroys the object.
    */
    virtual ~ArnoldInterface() { 
      delete[] m_buffer; 
      m_buffer = NULL;
      delete[] m_bucket_pos; 
      m_bucket_pos = NULL;
      m_bucket_num = 0; 
    }
      
    /*!
    * \brief Initializes the Arnold renderer.
    *
    * Opens a new Arnold session. Loads ass file and the plugin (buffer_driver).
    */
    virtual void init();

    /*!
    * \brief Initialize the ArnoldRenderer with a reference to a JSON serialized parent patch
    *
    * Initializes the ArnoldRenderer with a reference to a JSON serialized parent patch.
    * This is currently used by the distributed renderer to send a patch over the wire
    */
    virtual void init(const JSONNode jsonPatch) { this->init(); }

    /*!
    * \brief Closes the Arnold session.
    */
    virtual void close();

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
    \brief Sets the dimensions of the image.
    */
    virtual bool setDims(int w, int h);
    
    /*!
    * \brief Gets the pointer to the frame buffer.
    *
    * \return The pointer to the frame buffer.
    */
    virtual float *getBufferPointer() { return m_buffer; }

#ifdef USE_ARNOLD
    /*!
    \brief Sets an integer parameter for an arnold light
    */
    void setParameter(string lightName, string param, int val);

    /*!
    \brief Sets a float parameter for an arnold light.
    */
    void setParameter(string lightName, string param, float val);

    /*!
    \brief Sets a 4x4 transformation matrix given translation and rotation 
    */
    void setParameter(string lightName, string param, Eigen::Matrix3f rot, Eigen::Vector3f trans);

    /*!
    \brief Sets a RGB parameter
    */
    void setParameter(string lightName, string param, float x, float y, float z);

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
    \brief Sets a parameter found in the global options node in arnold
    */
    virtual void setOptionParameter(const std::string &paramName, int val);
    virtual void setOptionParameter(const std::string &paramName, float val);
      
    /*!
    \brief Gets a parameter found in the global options node in arnold
    */
    int getOptionParameter(const std::string &paramName);

    /*!
    * \brief Loads a arnold parameter mapping into the list.
    *
    * \param data The json node containing the mapping.
    */
    void loadArnoldParam(const JSONNode data);      
#endif

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
    * \brief Sets the predictive flag.
    *
    * \param predictive The predictive flag.
    */
    void setPredictive(bool predictive) { m_predictive = predictive; }

    /*!
    * \brief Gets the predictive flag.
    *
    * \return The predictive flag.
    */
    bool getPredictive() { return m_predictive; }
      
    /*!
    * \brief Sets the camera sampling rate used for current rendering.
    *
    * Although the system may have multiple camera sampling rates (e.g. for interactive mode and for real rendering),
    * this rate will be used for the current rendering.
    * \param samples The sampling rate.
    */
    virtual void setSamples(int samples);

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
    virtual int render();

    /*!
    * \brief Fire off a render request using the given set of devices
    * 
    * Fires off a render request with the given set of devices being used
    * in the rendering. This is currently used to notify a distributed
    * renderer about any changes to nodes.
    * \return Arnold error code
    * \sa render()
    */
    virtual int render(const std::set<Device*> &devices) { return this->render(); }
      
    /*!
    * \brief Interrupts current rendering.
    */
    virtual void interrupt();

    /*!
    * \brief Parses the arnold parameter map to a JSON node.
    * \return The Json node.
    */
    virtual JSONNode toJSON();

    /*!
    * \brief Gets the progress of current frame as a percentage.
    * \return The percent.
    */
    virtual float getPercentage() { 
      if (m_progress.bucket_sum < 0)
        return 0.f;
      return (0.f + m_progress.bucket_cur) / m_progress.bucket_sum * 100.f; 
    }

    /*!
    * \brief Gets the current bucket for each worker thread.
    * \return An array of current buckets.
    */
    BucketPositionInfo *getBucketPositionInfo() const { return m_bucket_pos; }

    /*!
    * \brief Gets number of buckets rendered simultanously.
    * This is usually the number of threads supported by hardware.
    * \return The number of buckets rendered simultanously.
    */
    size_t getBucketNumber() const { return m_bucket_num; }

#ifdef USE_ARNOLD
    /*!
    * \brief Modifies the surface color according to Picture Perfect RGB Rendering Using Spectral Prefiltering and Sharp Color Primaries.
    * Currently only converts sRGB to sharp RGB.
    */
    void updateSurfaceColor(Eigen::Vector3d white);
#endif

    /*!
    * \brief Sets the default path.
    *
    * \param def_path The default path.
    */
    void setDefaultPath(std::string def_path) { m_default_path = def_path; }

    /*!
    * \brief Gets the default path.
    *
    * \return The default path.
    */
    std::string getDefaultPath() { return m_default_path; }

    /*!
    * \brief Checks if this interface is currently open.
    *
    * \return If it's open.
    */
    bool isOpen() { return m_open; }

#ifdef USE_ARNOLD
    void addGobo(AtNode *light_ptr, std::string file, float deg, float rot);

    /*!
    \brief Sets the Arnold log filename and flags
    */
    void setLogFileName(string filename, int flags = AI_LOG_ALL);

    /*!
    \brief Returns a set of light names used in the .ass file
    */
    map<string, AtNode*> getLights();

    /*!
    \brief Sets the driver file path. Assumes default arnold renderers
    */
    void setDriverFileName(string base, string filename);
#endif

    /*!
    \brief Indicates if the distributed interface is currently open
    In the base class, this is the same as the local status
    */
    virtual bool isDistributedOpen();

    /*!
    \brief Check whether this patch has caching enabled
    */
    bool isUsingCaching() { return m_using_caching; }

    /*!
    \brief Set whether this interface is using caching
    */
    void setUsingCaching(bool usingCaching) { m_using_caching = usingCaching; }

    /*!
    \brief If the interface is caching it should use this function to perform any
    preload operations it may be doing.
    */
    virtual void loadIfUsingCaching(const set<Device*>& devices) { };

  protected:
#ifdef USE_ARNOLD
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
#endif

	/*!
	* \brief Checks to see if this arnold STRING parameter is a file.
	* Currently it only checks if the file contains ".ies".
	* \return If is a file.
	*/
	inline static bool isRelativeFileName(std::string str) {
		// This function is applied to STRING arnold parameters.
		// Right now we only need to handle .ies file.
		// May add more conditions to this line in the future.
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		if ((!str.empty() && str[0] == '.') ||
			((str.find(".ies") != std::string::npos || str.find(".ass") != std::string::npos
			|| str.find(".jpg") != std::string::npos) &&
			(str.find("/") == std::string::npos && str.find("\\") == std::string::npos)))
			return true;
		return false;
	}
      
	/*!
	* \brief Converts to relative path with respect to json path.
	* Currently it only checks if the file contains ".ies".
	* \return If is a file.
	*/
	inline std::string toRelativePath(std::string file);

#ifdef USE_ARNOLD
  /*!
  * \brief The list containing the mappings between metadata to Arnold parameter.
  * \sa ArnoldParam
  */
  map<string, ArnoldParam> m_arnold_params;
#endif    

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

    /*!
    * \brief If turn on (so-called) predictive rendering
    */
    bool m_predictive;

    /*!
    * \brief If the interface is currently open
    */
    bool m_open;

    // An array for worker threads.
    BucketPositionInfo *m_bucket_pos;

    // The size of buckets array.
    size_t m_bucket_num;

    // The progress info of the current frame.
    ProgressInfo m_progress;

    // The default path for all passed files (.ass, .ies...)
    std::string m_default_path;

    // Buffer driver name
    string m_bufDriverName;

	/*!
	* \brief Is this interface using caching -- currently only used for
	* notifying the distributed renderer that it should use the caching
	* driver instead of the buffer driver
	*/
	bool m_using_caching;
  };

  /*!
  * \brief Parses a formatted string into a ArnoldParameterVector instance.
  *
  * \param value A formatted string.
  * \param vector The returned vector.
  * \return If there is error.
  */
  template<size_t D, typename T>
  static bool parseArnoldParameter(const std::string &value, ArnoldParameterVector<D, T> &vector) {
	  T element;
	  std::string value_spaceless = value;
	  bool ret = true;

	  // Removes spaces when the input type is not string
	  int count = 0;
	  for (char c : value) {
      std::locale loc;
		  if (!std::isspace(c, loc)) {
			  value_spaceless[count++] = c;
		  }
	  }
	  value_spaceless = value_spaceless.substr(0, count);

	  // Format: "v1, v2, ..."
	  size_t offset = 0;
	  size_t i;
	  for (i = 0; i < D; i++) {
		  std::istringstream iss(value_spaceless.substr(offset));
		  iss >> element;
		  vector[i] = element;

		  offset = value_spaceless.find(",", offset);

		  if (offset == std::string::npos) {
			  i++;
			  break;
		  }

		  offset++;
	  }

	  if (i != D) {
		  ret = false;
		  Logger::log(WARN, "Input dimension disagrees with vector dimension. Extra elements are left with existing values.");
	  }

	  return ret;
  }
}

#endif