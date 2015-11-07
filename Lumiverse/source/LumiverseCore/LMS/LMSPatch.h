/*! \file LMSPatch.h
* \brief Implementation of a patch for Lightman Simulation.
*/
#ifndef _LMSPATCH_H_
#define _LMSPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include "../Patch.h"
#include "../lib/libjson/libjson.h"

#include <iostream>
#include <thread>

#include "LightmanCore.h"
#include "LightmanSimulation.h"

namespace Lumiverse {
/*!
* \brief The LMS Patch object manages the communication between the a Lightman
* simulation and the Lumiverse devices.
*/
class LMSPatch : public Patch {
public:

    /*!
    * \brief Constructs a LMSPatch object.
    */
    LMSPatch();

    /*!
    * \brief Construct LMSPatch from JSON data.
    *
    * \param data JSONNode containing the LMSPatch object data.
    */
    LMSPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~LMSPatch();

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "LMSPatch"
    */
    virtual string getType() { return "LMSPatch"; }

    /*!
    * \brief Initializes connections and other network settings for the patch.
    *
    * Call this AFTER all interfaces have been assigned. May need to call again
    * if interfaces change.
    */
    virtual void init();

    /*!
    * \brief Updates the values sent to the DMX network given the list of devices
    * in the rig.
    *
    * The list of devices should be maintained outside of this class.
    */
    virtual void update(set<Device *> devices);

    /*!
    * \brief Closes connections to the interfaces.
    */
    virtual void close();

    /*!
    * \brief Exports a JSONNode with the data in this patch
    *
    * \return JSONNode containing the LMSPatch object
    */
    virtual JSONNode toJSON();

    /*!
     * \brief Pathes a single device by device pointer
     * \param device Device id of the device to patch
     * \param layer Layer name of the layer to map the device to
     */
    void patchDevice(string device_id, string layer_name);

    /*!
    * \brief Unpatch a deleted device.
    * \param device_id Device id of the device to unpatch
    */
    virtual void deleteDevice(string device_id);

private:

    /*!
    * \brief Loads data from a parsed JSON object
    * \param data JSON data to load
    */
    void loadJSON(const JSONNode data);

    /*!
    * \brief Loads a Lightman workspace from a JSON node
    * \param data JSON node containing the workspace data
    */
    void loadInputFile(const JSONNode data);

    /*!
    * \brief Save path to workspace input as JSON object.
    */
    JSONNode saveInputFile();

    /*!
    * \brief Loads the device map from a JSON node
    * \param data JSON node containing the Device Map data
    */
    void loadDeviceMap(const JSONNode data);

    /*!
    * \brief Converts the device map into a JSON object.
    */
    JSONNode saveDeviceMap();

    /*!
    * \brief Clears out the patch, deletes all device mappings.
    */
    void clearDeviceMap();

    /*!
    * \brief Path to the input Lightman workspace to be used.
    */
    string input_file;

    /*!
    * \brief Maps devices to lightman layers in the simulation. 
    *
    * This is a unique mapping. Devices are stored by ids and all devices
    * in the map should be valid in the rig. Layers are stored by layer name
    * and layers should be valid layers in the simulation.
    * \sa LightmanCore::Layer
    */
    map<string, string> devicemap;

    /*!
     * \brief The Lightman workspace.
     */
    LightmanCore::Workspace *workspace;

    /*!
     * \brief The Lightman compositor.
     */
    LightmanCore::Compositor *compositor;

    /*!
     * \brief The Lightman tonemapper.
     */
    LightmanCore::ToneMapper *tonemapper;

    /*!
     * \brief The Lightman simulation instance.
     */
    LightmanSimulation::Simulation *simulation;
};

}; // namespace Lumiverse

#endif // _LMSPATCH_H_