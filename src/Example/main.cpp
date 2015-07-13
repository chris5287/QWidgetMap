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

// Qt includes.
#include <QtWidgets/QApplication>

// Local includes.
#include "Example.h"

int main(int argc, char *argv[])
{
    // Create a QApplication.
    QApplication app(argc, argv);

    // Create a Example widget.
    Example example_widget;
    example_widget.resize(640, 640);
    example_widget.setWindowTitle("QWidgetMap Example");

    // Show the widget.
    example_widget.show();

    // Execute the application.
    return app.exec();
}
