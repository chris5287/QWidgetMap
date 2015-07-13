/**
 * @copyright 2015 Chris Stylianou
 * @copyright 2007 - 2008 Kai Winter
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "LayerManager.h"

using namespace qwm;

LayerManager::LayerManager(QObject* parent)
    : QObject(parent)
{
    // Nothing else to do.
}

const std::vector<std::shared_ptr<Layer>> LayerManager::layers() const
{
    // Gain a read lock to protect the layers container.
    QReadLocker locker(&m_layers_mutex);

    // Return the layers.
    return m_layers;
}

std::shared_ptr<Layer> LayerManager::layer(const std::string& name) const
{
    // Default return value.
    std::shared_ptr<Layer> return_layer(nullptr);

    // Gain a read lock to protect the layers container.
    QReadLocker locker(&m_layers_mutex);

    // Try to find the layer in question.
    const auto itr_find(std::find_if(m_layers.begin(), m_layers.end(), [&](const std::shared_ptr<Layer>& layer) { return layer->name() == name; }));
    if(itr_find != m_layers.end())
    {
        // Return the layer found.
        return_layer = (*itr_find);
    }

    // Return the layer.
    return return_layer;
}

void LayerManager::add(const std::shared_ptr<Layer>& layer, const int& index)
{
    // Check we have a valid layer.
    if(layer != nullptr)
    {
        // Remove the layer if it already exists.
        remove(layer->name());

        // Connect signals/slots to emit layer changed when redraw requests received.
        QObject::connect(layer.get(), &Layer::requestRedraw, this, &LayerManager::layerChanged);

        // Connect signal/slot to pass on drawable clicked events.
        QObject::connect(layer.get(), &Layer::drawableClicked, this, &LayerManager::drawableClicked);

        // Scope the locker to ensure the mutex is release as soon as possible.
        {
            // Gain a write lock to protect the layers container.
            QWriteLocker locker(&m_layers_mutex);

            // Is the index == -1 or greater than current vector size.
            if(index == -1 || index >= int(m_layers.size()))
            {
                // Add the layer to the end.
                m_layers.push_back(layer);
            }
            else
            {
                // Insert the layer at the required index.
                m_layers.insert(m_layers.begin() + index, layer);
            }
        }

        // Emit that the layer has been added.
        emit layerAdded(layer);
    }
}

void LayerManager::remove(const std::string& name)
{
    // Fetch the layer by name.
    const auto layer_to_remove(layer(name));

    // Is the layer valid.
    if(layer_to_remove != nullptr)
    {
        // Scope the locker to ensure the mutex is release as soon as possible.
        {
            // Gain a write lock to protect the layers container.
            QWriteLocker locker(&m_layers_mutex);

            // Try to find the layer in question.
            const auto itr_find(std::find(m_layers.begin(), m_layers.end(), layer_to_remove));
            if(itr_find != m_layers.end())
            {
                // Disconnect all signals associated with the layer.
                QObject::disconnect(itr_find->get(), 0, this, 0);

                // Remove the layer.
                m_layers.erase(itr_find);
            }
        }

        // Emit that the layer has been removed.
        emit layerRemoved(layer_to_remove);
    }
}
