#include "Grid.h"

Grid::Grid(float size, int divisions)
{
    Init(size, divisions);

    m_Vertices = m_OgVerts;

    m_VAO = VAO();
    m_VAO.Bind();

    m_VBO = new VBO(m_Vertices.data(), m_Vertices.size() * sizeof(GLfloat), GL_DYNAMIC_DRAW);

    m_VAO.LinkAttrib(*m_VBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    m_VAO.Unbind();
    m_VBO->Unbind();
}

void Grid::Init(float size, int divisions)
{
    float step = size / divisions;
    float halfSize = size / 2.0f;

    // x axis
    for (int yStep = 3; yStep <= 3; ++yStep) {
        float y = -halfSize * 0.3f + yStep * step;
        for (int zStep = 0; zStep <= divisions; ++zStep) {
            float z = -halfSize + zStep * step;
            for (int xStep = 0; xStep < divisions; ++xStep) {
                float xStart = -halfSize + xStep * step;
                float xEnd = xStart + step;
                m_OgVerts.push_back(xStart); m_OgVerts.push_back(y); m_OgVerts.push_back(z);
                m_OgVerts.push_back(xEnd);   m_OgVerts.push_back(y); m_OgVerts.push_back(z);
            }
        }
    }
    // zaxis
    for (int xStep = 0; xStep <= divisions; ++xStep) {
        float x = -halfSize + xStep * step;
        for (int yStep = 3; yStep <= 3; ++yStep) {
            float y = -halfSize * 0.3f + yStep * step;
            for (int zStep = 0; zStep < divisions; ++zStep) {
                float zStart = -halfSize + zStep * step;
                float zEnd = zStart + step;
                m_OgVerts.push_back(x); m_OgVerts.push_back(y); m_OgVerts.push_back(zStart);
                m_OgVerts.push_back(x); m_OgVerts.push_back(y); m_OgVerts.push_back(zEnd);
            }
        }
    }
}

void Grid::Update(float size, int divisions)
{
    m_OgVerts.clear();
    m_Vertices.clear();
    Init(size, divisions);
    m_Vertices = m_OgVerts;
    m_VBO->Bind();
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(GLfloat), m_Vertices.data(), GL_DYNAMIC_DRAW);
}

void Grid::Update(const std::vector<Body*> bodies, glm::vec3 camPos)
{
    m_Vertices = m_OgVerts;
    glm::vec3 cPos = camPos * glm::vec3(1, 0, 1); // Remove y-axis

    for (int i = 0; i < m_Vertices.size(); i += 3) {
        m_Vertices[i]       += cPos.x;
        m_Vertices[i + 2]   += cPos.z;
        glm::vec3 vertexPos(m_Vertices[i], m_Vertices[i + 1], m_Vertices[i + 2]);
        float totalDisplacement = 0.0f;
        for (Body* body : bodies)
        {
            glm::vec3 toObject = body->Position - vertexPos;
            float distance = glm::length(toObject);
            float distance_m = distance * 1000.0f;
            float rs = (2 * 6.67430e-11 * body->Mass) / pow(299792, 2);
            
            float dz = 2 * sqrt(rs * (distance_m - rs));
            totalDisplacement += dz;
        }

        m_Vertices[i + 1]   = totalDisplacement;
    }
    float highest = 0;
    for (int i = 1; i < m_Vertices.size(); i += 3) {
        if (m_Vertices[i] > highest)
            highest = m_Vertices[i];
    }
    for (int i = 1; i < m_Vertices.size(); i += 3) {
        m_Vertices[i] -= highest;
    }

    m_VBO->Update(m_Vertices.data(), GLsizeiptr(m_Vertices.size() * sizeof(GLfloat)));
}

void Grid::Render(Shader& shader, Camera& camera)
{
    shader.Activate();
    camera.Update(shader);
    m_VAO.Bind();
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, m_Vertices.size() / 3);
}
