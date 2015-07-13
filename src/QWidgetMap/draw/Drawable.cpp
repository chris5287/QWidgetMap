/**
 * @copyright 2015 Chris Stylianou
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

#include "Drawable.h"

using namespace qwm;
using namespace qwm::draw;

Drawable::Drawable(const DrawableType& drawable_type, QObject* parent)
    : QObject(parent),
      m_drawable_type(drawable_type)
{

}

const DrawableType& Drawable::drawableType() const
{
    // Return the drawable type.
    return m_drawable_type;
}

QVariant Drawable::metadata(const std::string& key) const
{
    // Default return value.
    QVariant return_value;

    // Find the key.
    const auto itr_find(m_metadata.find(key));
    if(itr_find != m_metadata.end())
    {
        // Fetch the value.
        return_value = itr_find->second;
    }

    // Return the value.
    return return_value;
}

void Drawable::setMetadata(const std::string& key, const QVariant& value)
{
    // Set the meta-data.
    m_metadata[key] = value;
}

bool Drawable::visible() const
{
    // Return the visibility.
    return m_visible;
}

bool Drawable::isVisible(const Viewport& viewport) const
{
    // Default visibility.
    bool visibility(m_visible);

    // Check whether the zoom level is within range.
    if(m_zoom_minimum > viewport.zoom() || m_zoom_maximum < viewport.zoom())
    {
        // Outside of supported zoom levels.
        visibility = false;
    }

    // Return the visibility.
    return visibility;
}

void Drawable::setVisible(const bool& enabled)
{
    // Only update visibility if it has changed.
    if(m_visible != enabled)
    {
        // Set the visibility.
        m_visible = enabled;

        // Emit that we need to redraw to display this change.
        emit requestRedraw();
    }
}

void Drawable::setZoomMinimum(const int& zoom_minimum)
{
    // Only update zoom minimum if it has changed.
    if(m_zoom_minimum != zoom_minimum)
    {
        // Set the zoom minimum.
        m_zoom_minimum = zoom_minimum;

        // Emit that we need to redraw to display this change.
        emit requestRedraw();
    }
}

void Drawable::setZoomMaximum(const int& zoom_maximum)
{
    // Only update zoom maximum if it has changed.
    if(m_zoom_maximum != zoom_maximum)
    {
        // Set the zoom maximum.
        m_zoom_maximum = zoom_maximum;

        // Emit that we need to redraw to display this change.
        emit requestRedraw();
    }
}
