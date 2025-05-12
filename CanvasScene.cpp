#include "CanvasScene.h"
#include <QRunnable>

CanvasScene::CanvasScene() : m_t(0), m_renderer(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &CanvasScene::handleWindowChanged);

}

void CanvasScene::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &CanvasScene::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &CanvasScene::cleanup, Qt::DirectConnection);
        win->setColor(Qt::black);
    }
}

void CanvasScene::sync()
{
    if (!m_renderer) {
        m_renderer = new CanvasSceneRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &CanvasSceneRenderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer, &CanvasSceneRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}

void CanvasScene::cleanup()
{
    delete m_renderer;
    m_renderer = nullptr;
}

class CleanupJob : public QRunnable
{
public:
    CleanupJob(CanvasSceneRenderer *renderer) : m_renderer(renderer) { }
    void run() override { delete m_renderer; }
private:
    CanvasSceneRenderer *m_renderer;
};

void CanvasScene::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}

CanvasSceneRenderer::~CanvasSceneRenderer()
{
    delete m_program;
}

void CanvasScene::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window()) {
        // update() on the window, instead of the item itself
        window()->update();
    }

}
