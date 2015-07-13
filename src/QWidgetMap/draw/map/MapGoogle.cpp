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

#include "MapGoogle.h"

using namespace qwm::draw::map;

MapGoogle::MapGoogle(const MapGoogle::GoogleLayerType& layer_type, QObject* parent)
    : MapTile("http://mt1.google.com/vt/v=ap.106&hl=en&x=%x&y=%y&zoom=%zoom&lyrs=" + layerTypeToString(layer_type), std::set<projection::EPSG> { projection::EPSG::SphericalMercator }, 0, true, parent)
{

}

QString MapGoogle::layerTypeToString(const MapGoogle::GoogleLayerType& layer_type)
{
    // Convert the enum to a 1-character representation.
    switch(layer_type)
    {
        case GoogleLayerType::SATELLITE:
        {
            return "s";
        }
        case GoogleLayerType::TERRAIN:
        {
            return "t";
        }
        case GoogleLayerType::HYBRID:
        {
            return "h";
        }
        case GoogleLayerType::RASTER:
        {
            return "r";
        }
        case GoogleLayerType::MAPS:
        {
            return "m";
        }
    }

    // Default to maps.
    return "m";
}
