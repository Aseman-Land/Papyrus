/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    Papyrus is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Papyrus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ASEMANDEVELOPER_MACROS_H
#define ASEMANDEVELOPER_MACROS_H

#include <QMetaMethod>

#define PRINT_EMITTED_SIGNAL \
    if( sender() ) \
    { \
        QObject *obj   = sender(); \
        int signal_idx = senderSignalIndex(); \
        QMetaMethod method = obj->metaObject()->method(signal_idx); \
        qDebug() << method.name(); \
    }

#endif // ASEMANDEVELOPER_MACROS_H
