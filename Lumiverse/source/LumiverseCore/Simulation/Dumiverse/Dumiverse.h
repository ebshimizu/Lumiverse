/*! \file Dumiverse.h
* \brief Implementation of a distributed node that renders Arnold ass files
*/
#ifndef _ArnoldDUMIVERSE_H
#pragma once

#include <algorithm>
#include <cstdio>
#include <thread>
#include <iostream>
#include <locale>


/*!
* \brief Initializes data structures
*
* Opens a connection to this DistributedArnold node. This includes sending it
* the ass file and plugin (buffer_driver).
*/
void init(const char *jsonPatchStr, const char *filename);

/*!
* \brief Fire off an arnold render
*
* Wrapper function for firing off an arnold render
* request. This spawns a new thread in which
* render() is actually called -- we need to
* do this to unblock the node event loop.
*
* When the rendering is completed, render
* writes a local file with the return value.
*/
int renderWrapper(const char *jsonDevicesStr, const char *jsonSettingsStr);

/*!
* \brief Render a frame and pass it back to node
*/
void render();

/*!
* \brief Close the connection to node
*
* Closes the open connection with node
*/
void close();

/*!
 * \brief Get the frame buffer that was rendered by Arnold
 */
float *getFrameBuffer();

/*!
 * \brief Interrupt the current Arnold renderer
 */
void interrupt();

/*!
 *  \brief Get the width of the loaded image
 */
int getWidth();

/*!
 * \brief Get the height of the loaded image
 */
int getHeight();

/*!
 * \brief Gets the progress of current frame as a percentage.
 * \return The percent.
 */
float getPercentage();

#endif
