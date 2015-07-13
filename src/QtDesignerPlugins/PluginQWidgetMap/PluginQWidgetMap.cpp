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

#include "PluginQWidgetMap.h"

// Qt includes.
#include <QtCore/QtPlugin>

// QWidgetMap includes.
#include <QWidgetMap/QWidgetMap.h>

PluginQWidgetMap::PluginQWidgetMap(QObject* parent)
    : QObject(parent)
{
    // Nothing else to do.
}

void PluginQWidgetMap::initialize(QDesignerFormEditorInterface* /*core*/)
{
    // Set the initialised to true.
    m_initialised = true;
}

bool PluginQWidgetMap::isInitialized() const
{
    // Return the initialised state.
    return m_initialised;
}

QWidget* PluginQWidgetMap::createWidget(QWidget* parent)
{
    // Helper function to create the required widget.
    return new qwm::QWidgetMap(parent);
}

QString PluginQWidgetMap::name() const
{
    // Return the name of the widget.
    return "QWidgetMap";
}

QString PluginQWidgetMap::group() const
{
    // Return the group the widget belongs to.
    return "Custom Widgets";
}

QIcon PluginQWidgetMap::icon() const
{
    // Return an empty icon.
    return QIcon();
}

QString PluginQWidgetMap::toolTip() const
{
    // Return a tool tip note.
    return "QWidget that can display maps";
}

QString PluginQWidgetMap::whatsThis() const
{
    // Return a what's this note.
    return "";
}

bool PluginQWidgetMap::isContainer() const
{
    // Return whether the user can insert other widgets inside this widget.
    return true;
}

QString PluginQWidgetMap::domXml() const
{
    // Return the default style to be applied to the widget.
    return "<ui language=\"c++\">\n"
           " <widget class=\"QWidgetMap\" name=\"qwidget_map\" />\n"
           "</ui>\n";
}

QString PluginQWidgetMap::includeFile() const
{
    // Return the header file required.
    return QLatin1String("<QWidgetMap/QWidgetMap.h>");
}
