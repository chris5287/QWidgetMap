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

#pragma once

// STL includes.
#include <vector>

// Local includes.
#include "../qwidgetmap_global.h"
#include "Point.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Utilities namespace.
    namespace util
    {

        /// Algorithms namespace.
        namespace algorithms
        {

            /**
             * Calculates the mean points provided.
             * @param points_coord The points to use.
             * @return the mean point.
             */
            QWIDGETMAP_EXPORT PointWorldCoord mean(const std::vector<PointWorldCoord>& points_coord);

            /**
             * Calculates the Great Circle distance.
             * @param start_point The start point to use.
             * @param end_point The end point to use.
             * @return the distance between the points in meters.
             */
            QWIDGETMAP_EXPORT double distance(const PointWorldCoord& start_point, const PointWorldCoord& end_point);

            /**
             * Calculates the Great Circle destination point.
             * @param start_point The start point to use.
             * @param distance_m The distance in meters to use.
             * @param bearing_deg The bearing in degrees to use.
             * @return the destination point.
             */
            QWIDGETMAP_EXPORT PointWorldCoord destinationPoint(const PointWorldCoord& start_point, const double& distance_m, const double& bearing_deg);

        }

    }

}
