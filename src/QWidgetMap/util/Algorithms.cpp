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

// STL includes.
#include <cmath>

// Local includes.
#include "Algorithms.h"

using namespace qwm::util;

PointWorldCoord algorithms::mean(const std::vector<PointWorldCoord>& points_coord)
{
    // Sum totals.
    double sum_longitudes(0.0);
    double sum_latitudes(0.0);

    // Loop through the coordiantes to sum the longitude and latitude's.
    for(const auto& coordinate : points_coord)
    {
        sum_longitudes += coordinate.longitude();
        sum_latitudes += coordinate.latitude();
    }

    // Return the mean of longitudes and latitudes.
    return PointWorldCoord(sum_longitudes / points_coord.size(), sum_latitudes / points_coord.size());
}

double algorithms::distance(const PointWorldCoord& start_point, const PointWorldCoord& end_point)
{
    // Distance of earth radius in meters (ie: 1 radian).
    const double earths_radius_m(6371000);

    // Convert the points and differences into radians.
    const double start_latitude_rad(start_point.latitude() * (M_PI / 180.0));
    const double end_latitude_rad(end_point.latitude() * (M_PI / 180.0));
    const double diff_latitude_rad((end_point.latitude() - start_point.latitude())* (M_PI / 180.0));
    const double diff_longitude_rad((end_point.longitude() - start_point.longitude())* (M_PI / 180.0));

    // Calculate the distance between the points using the Great Circle algorithms.
    const double distance(std::pow(std::sin(diff_latitude_rad / 2.0) , 2) + std::cos(start_latitude_rad) * std::cos(end_latitude_rad) * std::pow(std::sin(diff_longitude_rad / 2.0), 2));

    // Return the distance in meters.
    return (earths_radius_m * 2.0 * std::atan2(std::sqrt(distance), std::sqrt(1.0 - distance)));
}

PointWorldCoord algorithms::destinationPoint(const PointWorldCoord& start_point, const double& distance_m, const double& bearing_deg)
{
    // Distance of earth radius in meters (ie: 1 radian).
    const double earths_radius_m(6371000);

    // Convert the start point, distance and bearing into radians.
    const double start_latitude_rad(start_point.latitude() * (M_PI / 180.0));
    const double start_longitude_rad(start_point.longitude() * (M_PI / 180.0));
    const double distance_rad(distance_m/earths_radius_m);
    const double bearing_rad(bearing_deg * (M_PI / 180.0));

    // Calculate the destination point using the Great Circle algorithms.
    const auto destination_latitude_rad = std::asin(std::sin(start_latitude_rad) * std::cos(distance_rad) + std::cos(start_latitude_rad) * std::sin(distance_rad) * std::cos(bearing_rad));
    const auto destination_longitude_rad = start_longitude_rad + std::atan2(std::sin(bearing_rad) * std::sin(distance_rad) * std::cos(start_latitude_rad), std::cos(distance_rad) - std::sin(start_latitude_rad) * std::sin(destination_latitude_rad));

    // Return the destination point.
    return PointWorldCoord(destination_longitude_rad * (180.0 / M_PI), destination_latitude_rad * (180.0 / M_PI));
}
