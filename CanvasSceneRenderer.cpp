#include "CanvasSceneRenderer.h"
#include <fstream>
#include <filesystem>
#include <vector>

void CanvasSceneRenderer::init()
{
    if (!m_program) {
        QSGRendererInterface *rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL);

        initializeOpenGLFunctions();

        const float values[] = { -1, -1, 1, -1, -1, 1, 1, 1 };

        m_vbo.create();
        m_vbo.bind();
        m_vbo.allocate(values, sizeof(values));

        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
        // attribute 0, two components per vertex, each component is float
        // not normalized
        // stride of two floats, 0 offset


        m_program = new QOpenGLShaderProgram();

        // should not need absolute paths...
        std::filesystem::path vert_path = "/Users/fgao/dev/vec/shader.vert";
        std::filesystem::path frag_path = "/Users/fgao/dev/vec/shader.frag";

        uintmax_t vert_size = std::filesystem::file_size(vert_path.string());
        uintmax_t frag_size = std::filesystem::file_size(frag_path.string());
        std::vector<char> vert(vert_size + 1);
        std::vector<char> frag(frag_size + 1);


        std::ifstream vertfile(vert_path.string());
        std::ifstream fragfile(frag_path.string());
        vertfile.read(vert.data(), vert_size);
        fragfile.read(frag.data(), frag_size);
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
                                                    "attribute highp vec4 vertices;"
                                                    "varying highp vec2 coords;"
                                                    "void main() {"
                                                    "    gl_Position = vertices;"
                                                    "    coords = vertices.xy;"
                                                    "}");
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
                                                    "uniform lowp float t;"
                                                    "varying highp vec2 coords;"
                                                    "void main() {"
                                                    "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
                                                    "    i = smoothstep(t - 0.8, t + 0.8, i);"
                                                    "    i = floor(i * 20.) / 20.;"
                                                    "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
                                                    "}");

        m_program->bindAttributeLocation("vertices", 0);
        // vertices attribute is bounded to attribute 0

        m_program->link();

    }
}

void CanvasSceneRenderer::paint()
{
    // Play nice with the RHI. Not strictly needed when the scenegraph uses
    // OpenGL directly.
    m_window->beginExternalCommands();

    m_vbo.bind();
    m_program->bind();
    m_program->setUniformValue("t", (float)m_t);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    m_program->release();

    m_window->endExternalCommands();
}
