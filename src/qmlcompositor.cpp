/****************************************************************************
**
** Copyright (C) 2019 elros34
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlcompositor.h"
#include <QWaylandSurfaceInterface>
#include <QQuickView>

QmlCompositor::QmlCompositor(QQuickView *quickView, const char *socketName,
                             const QString &screenOrientationOption)
    : QObject(quickView)
    , QWaylandQuickCompositor(quickView, socketName, DefaultExtensions | SubSurfaceExtension)
    , m_fullscreenSurface(nullptr)
{
    QSize size = window()->size();
    if (screenOrientationOption == "landscape")
        setSize(size.height(), size.width());
    else
        setSize(size.width(), size.height());
    addDefaultShell();
}

QWaylandQuickSurface *QmlCompositor::fullscreenSurface() const
{
    return m_fullscreenSurface;
}

void QmlCompositor::setSize(int width, int height)
{
    setOutputGeometry(QRect(0,0, width, height));
}

QWaylandSurfaceItem *QmlCompositor::item(QWaylandSurface *surf)
{
    return static_cast<QWaylandSurfaceItem *>(surf->views().first());
}

void QmlCompositor::destroyWindow(QVariant window)
{
    qvariant_cast<QObject *>(window)->deleteLater();
}

void QmlCompositor::setFullscreenSurface(QWaylandQuickSurface *surface)
{
    if (surface == m_fullscreenSurface)
        return;
    m_fullscreenSurface = surface;
    emit fullscreenSurfaceChanged();
}

void QmlCompositor::surfaceMapped()
{
    QWaylandQuickSurface *surface = qobject_cast<QWaylandQuickSurface *>(sender());
    emit windowAdded(QVariant::fromValue(surface));
}
void QmlCompositor::surfaceUnmapped()
{
    QWaylandQuickSurface *surface = qobject_cast<QWaylandQuickSurface *>(sender());
    if (surface == m_fullscreenSurface)
        m_fullscreenSurface = nullptr;
    emit windowDestroyed(QVariant::fromValue(surface));
}

void QmlCompositor::surfaceDestroyed(QObject *object)
{
    QWaylandQuickSurface *surface = static_cast<QWaylandQuickSurface *>(object);
    if (surface == m_fullscreenSurface)
        m_fullscreenSurface = nullptr;
    emit windowDestroyed(QVariant::fromValue(surface));
}

void QmlCompositor::sendCallbacks()
{
    if (m_fullscreenSurface)
        sendFrameCallbacks(QList<QWaylandSurface *>() << m_fullscreenSurface);
    else
        sendFrameCallbacks(surfaces());
}

void QmlCompositor::surfaceCreated(QWaylandSurface *surface)
{
    connect(surface, SIGNAL(destroyed(QObject *)), this, SLOT(surfaceDestroyed(QObject *)));
    connect(surface, SIGNAL(mapped()), this, SLOT(surfaceMapped()));
    connect(surface, SIGNAL(unmapped()), this,SLOT(surfaceUnmapped()));
}

