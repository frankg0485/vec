#pragma once

#include <QObject>
#include <QQuickItem>
#include "CanvasSceneRenderer.h"

class CanvasScene : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    QML_ELEMENT

public:
    CanvasScene();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void releaseResources() override;

    qreal m_t;
    CanvasSceneRenderer *m_renderer;
};
