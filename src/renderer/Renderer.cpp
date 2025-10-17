#include "Renderer.h"
#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

Renderer::Renderer()
    : m_viewMatrix(1.0f), m_projectionMatrix(1.0f), m_VAO(0), m_VBO(0) {}

Renderer::~Renderer() { Shutdown(); }

bool Renderer::Initialize() {
  spdlog::info("Initializing Renderer...");

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  m_basicShader = std::make_unique<Shader>();
  if (!m_basicShader->LoadFromFilesAsync("src/assets/shaders/basic.vert",
                                         "src/assets/shaders/basic.frag")) {
    spdlog::error("Failed to create basic shader");
    return false;
  } // Create test triangle
  CreateTestTriangle();

  // Set up projection matrix (perspective)
  m_projectionMatrix =
      glm::perspective(glm::radians(75.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

  spdlog::info("Renderer initialization complete");
  return true;
}

void Renderer::Update(float deltaTime) {
  // Update renderer state here
}

void Renderer::BeginFrame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame() {
  // Render test triangle
  m_basicShader->Use();

  // Set matrices
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));

  m_basicShader->SetMat4("model", model.value_ptr());
  m_basicShader->SetMat4("view", m_viewMatrix.value_ptr());
  m_basicShader->SetMat4("projection",
                         m_projectionMatrix.value_ptr()); // Draw triangle
  glBindVertexArray(m_VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}

void Renderer::Shutdown() {
  CleanupTestTriangle();
  m_basicShader.reset();
}

void Renderer::SetViewMatrix(const glm::mat4 &view) { m_viewMatrix = view; }

void Renderer::SetProjectionMatrix(const glm::mat4 &projection) {
  m_projectionMatrix = projection;
}

void Renderer::CreateTestTriangle() {
  // Triangle vertices with colors
  float vertices[] = {
      // positions         // colors
      0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // top (red)
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left (green)
      0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // bottom right (blue)
  };

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);

  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Renderer::CleanupTestTriangle() {
  if (m_VAO) {
    glDeleteVertexArrays(1, &m_VAO);
    m_VAO = 0;
  }
  if (m_VBO) {
    glDeleteBuffers(1, &m_VBO);
    m_VBO = 0;
  }
}
