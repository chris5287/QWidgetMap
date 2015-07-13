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

#include "PluginQProgressIndicator.h"

// Qt includes.
#include <QtCore/QtPlugin>

// QWidgetMap includes.
#include <QWidgetMap/util/QProgressIndicator.h>

PluginQProgressIndicator::PluginQProgressIndicator(QObject* parent)
    : QObject(parent)
{
    // Nothing else to do.
}

void PluginQProgressIndicator::initialize(QDesignerFormEditorInterface* /*core*/)
{
    // Set the initialised to true.
    m_initialised = true;
}

bool PluginQProgressIndicator::isInitialized() const
{
    // Return the initialised state.
    return m_initialised;
}

QWidget* PluginQProgressIndicator::createWidget(QWidget* parent)
{
    // Helper function to create the required widget.
    return new qwm::util::QProgressIndicator(parent);
}

QString PluginQProgressIndicator::name() const
{
    // Return the name of the widget.
    return "QProgressIndicator";
}

QString PluginQProgressIndicator::group() const
{
    // Return the group the widget belongs to.
    return "Custom Widgets";
}

QIcon PluginQProgressIndicator::icon() const
{
    // Return an empty icon.
    return QIcon();
}

QString PluginQProgressIndicator::toolTip() const
{
    // Return a tool tip note.
    return "QWidget that displays progress";
}

QString PluginQProgressIndicator::whatsThis() const
{
    // Return a what's this note.
    return "";
}

bool PluginQProgressIndicator::isContainer() const
{
    // Return whether the user can insert other widgets inside this widget.
    return true;
}

QString PluginQProgressIndicator::domXml() const
{
    // Return the default style to be applied to the widget.
    return "<ui language=\"c++\">\n"
           " <widget class=\"QProgressIndicator\" name=\"progress_indicator\" />\n"
           "</ui>\n";
}

QString PluginQProgressIndicator::includeFile() const
{
    // Return the header file required.
    return QLatin1String("<QWidgetMap/util/QProgressIndicator.h>");
}
