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

#pragma once

// Qt includes.
#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>

// STL includes.
#include <mutex>

// Local includes.
#include "qwidgetmap_global.h"
#include "Layer.h"
#include "draw/geometry/Geometry.h"

/// QWidgetMap namespace.
namespace qwm
{

    /**
     * Manages the layers that are displayed.
     */
    class QWIDGETMAP_EXPORT LayerManager : public QObject
    {
        Q_OBJECT

    public:

        /**
         * Constructor of Layer Manager.
         * @param parent QObject parent ownership.
         */
        explicit LayerManager(QObject* parent = nullptr);

        /// Disable copy constructor.
        LayerManager(const LayerManager&) = delete;

        /// Disable copy assignment.
        LayerManager& operator=(const LayerManager&) = delete;

        /// Destructor.
        ~LayerManager() = default;

    public:

        /**
         * Fetch the layers (Use this instead of the member variable for thread-safety).
         * @return a list of all layers.
         */
        const std::vector<std::shared_ptr<Layer>> layers() const;

        /**
         * Fetch the layer with the given name.
         * @param name The name of the wanted layer.
         * @return the layer with the given name.
         */
        std::shared_ptr<Layer> layer(const std::string& name) const;

        /**
         * Adds a layer.
         * If multiple layers are added, they are painted in the added order (the last added wil be the top layer).
         * @param layer The layer which should be added.
         * @param index The index position the layer should added (-1 means the layer is added to the top).
         * @param disable_redraw Whether to disable the redraw call after the layer has been added (allows bulk adding without redrawing each time).
         * @note 'index 0' is the base layer, 'index 1 layer' will be painted on top of 'index 0 layer', and so on...
         */
        void add(const std::shared_ptr<Layer>& layer, const int& index = -1, const bool& disable_redraw = false);

        /**
         * Removes a layer.
         * @param name The name of the layer which should be removed.
         * @param disable_redraw Whether to disable the redraw call after the layer has been removed (allows bulk removal without redrawing each time).
         */
        void remove(const std::string& name, const bool& disable_redraw = false);

    signals:

        /**
         * Signal emitted when a drawable item is clicked.
         * @param drawable The drawable item clicked.
         */
        void drawableClicked(const std::shared_ptr<draw::Drawable>& drawable) const;

        /**
         * Signal emitted when a layer is added.
         * @param layer The layer that has been added.
         */
        void layerAdded(std::shared_ptr<Layer> layer);

        /**
         * Signal emitted when a layer changes.
         */
        void layerChanged();

        /**
         * Signal emitted when a layer is removed.
         * @param layer The layer that has been removed.
         */
        void layerRemoved(std::shared_ptr<Layer> layer);

    private:

        /// Mutex to protect layers.
        mutable QReadWriteLock m_layers_mutex;

        /// List of layers (use layers() to access this for thread-safe read-only functionality).
        std::vector<std::shared_ptr<Layer>> m_layers;

    };

}
