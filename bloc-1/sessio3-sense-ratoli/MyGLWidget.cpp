#include </usr/include/GL/glew.h>
#include "MyGLWidget.h"
#include <iostream>

using namespace glm;
//using namespace std;

MyGLWidget::MyGLWidget (QGLFormat &f, QWidget* parent) : QGLWidget(f, parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
}

void MyGLWidget::loadShaders ()
{
    QGLShader fs(QGLShader::Fragment, this);
    fs.compileSourceFile("./fragment_shader.cc");
    QGLShader vs(QGLShader::Vertex, this);
    vs.compileSourceFile("./vertex_shader.cc");

    program = new QGLShaderProgram(this);
    program -> addShader(&fs);
    program -> addShader(&vs);
    program -> link();

    program -> bind();
}

void MyGLWidget::initializeGL ()
{
  // glew és necessari per cridar funcions de les darreres versions d'OpenGL
  glewExperimental = GL_TRUE;
  glewInit(); 
  glGetError();  // Reinicia la variable d'error d'OpenGL

  loadShaders();
  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  createBuffers();
}

void MyGLWidget::paintGL ()
{
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  // Activem l'Array a pintar 
  glBindVertexArray(VAO);
 
  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, 3);

  modelTransform();
  
  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::modelTransform () {
  glm::mat4 TG; // Matriu de transformació
  TG = glm::scale(glm::mat4(1.0),glm::vec3(scl, scl, scl));
  TG = glm::translate(TG , glm::vec3(movX, movY, movZ));

  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::keyPressEvent(QKeyEvent *e){
  
  switch(e->key()){
    case Qt::Key_Escape:
      exit(0);
    case Qt::Key_S:
      if(scl < 1.0){
        scl += 0.1;
        glUniform1f(varLoc,scl);
        updateGL();
      }
      break;
    case Qt::Key_D:
      if(scl >= 0.2){
        scl -= 0.1;
        glUniform1f(varLoc,scl);
        updateGL();
      }
      break;
    case Qt::Key_Left:
      movX -= 0.1;
      updateGL();
      break;
    case Qt::Key_Right:
        movX += 0.1;
        //glUniform1f(varLoc,scl);
        updateGL();
      break;
    case Qt::Key_Up:
        movY += 0.1;
        //glUniform1f(varLoc,scl);
        updateGL();
      break;
    case Qt::Key_Down:
        movY -= 0.1;
        //glUniform1f(varLoc,scl);
        updateGL();
      break;
    default: e->ignore();//propagar al pare
  }
}

void MyGLWidget::resizeGL (int w, int h)
{
  glViewport (0, 0, w, h);
}

void MyGLWidget::createBuffers ()
{
  GLuint pos, col;
  

  glm::vec3 Vertices[3];  // Tres vèrtexs amb X, Y i Z
  Vertices[0] = glm::vec3(-1.0, -1.0, 0.0);
  Vertices[1] = glm::vec3(1.0, -1.0, 0.0);
  Vertices[2] = glm::vec3(0.0, 1.0, 0.0);

  glm::vec4 color[3];  // Tres vèrtexs amb X, Y i Z
  color[0] = glm::vec4(1.0, 0.0, 0.0,1.0);
  color[1] = glm::vec4(0.0, 1.0, 0.0,1.0);
  color[2] = glm::vec4(0.0, 0.0, 1.0,1.0);

  
  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex (només el 0 en aquest cas)	

  pos = glGetAttribLocation(program->programId(),"vertex");
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(pos);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBOcolor);
  glBindBuffer(GL_ARRAY_BUFFER, VBOcolor);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex (només el 0 en aquest cas)

  col = glGetAttribLocation(program->programId(),"color"); //lultim color es el nom de la variable en el  shader
  glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(col);

  varLoc = glGetUniformLocation(program->programId(),"val");
  transLoc = glGetUniformLocation(program->programId(),"TG");
  
  movX = movY = movZ = 0;

  scl = 0.5;  
  glUniform1f(varLoc, scl);

  // Desactivem el VAO
  glBindVertexArray(0);
}

