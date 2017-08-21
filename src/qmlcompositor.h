#ifndef QMLCOMPOSITOR_H
#define QMLCOMPOSITOR_H

#include <QObject>
#include "qwaylandquickcompositor.h"
#include "qwaylandquicksurface.h"

#include <QtCompositor/qwaylandsurfaceitem.h>
#include <QtCompositor>

#include <QQmlContext>

#include <QQuickItem>
#include <QQuickView>

#include <QtCompositor/QWaylandSurfaceResizeOp>

class QmlCompositor : public QObject, public QWaylandQuickCompositor
{
    Q_OBJECT
    Q_PROPERTY(QWaylandQuickSurface* fullscreenSurface READ fullscreenSurface WRITE setFullscreenSurface NOTIFY fullscreenSurfaceChanged)

public:
    QmlCompositor(QQuickView *window);

    QWaylandQuickSurface *fullscreenSurface() const;

    Q_INVOKABLE QWaylandSurfaceItem *item(QWaylandSurface *surf);
    Q_INVOKABLE void setSize(int width, int height);

signals:
    void windowAdded(QVariant window);
    void windowDestroyed(QVariant window);
    void windowResized(QVariant window);
    void fullscreenSurfaceChanged();

public slots:
    void destroyWindow(QVariant window);

    void setFullscreenSurface(QWaylandQuickSurface *surface);
    void sendCallbacks();

private slots:
    void surfaceMapped();
    void surfaceUnmapped();
    void surfaceDestroyed(QObject *object);

protected:
    void surfaceCreated(QWaylandSurface *surface);

private:
    QWaylandQuickSurface *m_fullscreenSurface;
};

#endif // QMLCOMPOSITOR_H
