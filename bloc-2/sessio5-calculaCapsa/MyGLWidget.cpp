#include <glew.h>
#include "MyGLWidget.h"
#include <iostream>


using namespace std;

Model homerProves;

MyGLWidget::MyGLWidget (QGLFormat &f, QWidget* parent) : QGLWidget(f, parent)
{
  setFocusPolicy(Qt::ClickFocus); // per rebre events de teclat
  scale = 1.0f;
  angle = 0.0f;
}

void MyGLWidget::initializeGL () 
{
  std::cout << "initializeGL()" << endl;
  // glew és necessari per cridar funcions de les darreres versions d'OpenGL
  glewExperimental = GL_TRUE;
  glewInit(); 
  glGetError();  // Reinicia la variable d'error d'OpenGL
  glEnable (GL_DEPTH_TEST);
  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  createBuffers();
  modelTransform ();
  modelTransformPatricio();
  projectTransform(M_PI/2, 1, 1., 3.);
  viewTransform();
  
}

void MyGLWidget::paintGL () 
{
  std::cout << "paintGL()" << endl;
  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Activem Homer Prives
  modelTransform();

  glBindVertexArray(VAO_Terra);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  modelTransformPatricio();

  glBindVertexArray(VAO_Patricio);

  glDrawArrays(GL_TRIANGLES, 0, patricio.faces().size()*3);

  glBindVertexArray (0);
}

void MyGLWidget::modelTransform () 
{
  std::cout << "modelTransform()" << std::endl;

  // Matriu de transformació de model
  glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformPatricio ()
{
  std::cout << "modelTransformPatricio()" << std::endl;
  glm::mat4 TG;  // Matriu de transformació
  TG = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
  TG = glm::rotate(TG,angle,glm::vec3(0.,1.,0.));
  TG = glm::scale(TG, glm::vec3(escalaModel, escalaModel, escalaModel));
  TG = glm::translate(TG, -centreModel);
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::projectTransform(float angle, float ra, float znear, float zfar){
  std::cout << "projectTransform()" << endl;
  //glm::mat4 projection = glm::

  glm::mat4 Proj = glm::perspective(angle, ra, znear, zfar);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);

  // glm::mat4 Proj = glm::perspective(M_PI/2.0, 1.0, 1.0, 3.0);
  // glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform(){
  std::cout << "viewTransform()" << endl;
  glm::mat4 View = glm::lookAt (glm::vec3(0,1,2), glm::vec3(0,0,0), glm::vec3(0,1,0));
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::resizeGL (int w, int h) 
{
  float rav = (float) w/h;
  // double raw = (double)width()/(double)height();
  if(rav <= 1) {
    float newAngle = 2 * atan(tan(M_PI/4.)/rav);
    projectTransform(newAngle,rav,1.,3.);
  } else {
    projectTransform(M_PI/2.0,rav,1.,3.);
  }
  glViewport(0, 0, w, h);

}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  switch (event->key()) {
    case Qt::Key_Escape:
      exit(0);
    case Qt::Key_R: {
      std::cout << "R pushed -> angle =" << angle << endl;
      angle += M_PI/4;
      updateGL();
      break;
    }
    case Qt::Key_S: { // escalar a més gran
      std::cout << "S pushed" << endl;
      if(scale < 5){
        scale += 0.05;
      }
      updateGL();
      break;
    }
    case Qt::Key_D: { // escalar a més petit
      std::cout << "D pushed" << endl;
      if(scale > 0.05){
        scale -= 0.05;
      }
      updateGL();
      break;
    }
    default: event->ignore(); break;
  }
}

void MyGLWidget::calculaCapsaModel (Model model)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = model.vertices()[0];
  miny = maxy = model.vertices()[1];
  minz = maxz = model.vertices()[2];
  for (unsigned int i = 3; i < model.vertices().size(); i+=3)
  {
    if (model.vertices()[i+0] < minx)
      minx = model.vertices()[i+0];
    if (model.vertices()[i+0] > maxx)
      maxx = model.vertices()[i+0];
    if (model.vertices()[i+1] < miny)
      miny = model.vertices()[i+1];
    if (model.vertices()[i+1] > maxy)
      maxy = model.vertices()[i+1];
    if (model.vertices()[i+2] < minz)
      minz = model.vertices()[i+2];
    if (model.vertices()[i+2] > maxz)
      maxz = model.vertices()[i+2];
  }
  escalaModel = 2.0/(maxy-miny);
  centreModel[0] = (minx+maxx)/2.0; 
  centreModel[1] = (miny+maxy)/2.0; 
  centreModel[2] = (minz+maxz)/2.0;
}

void MyGLWidget::createBuffers () 
{
  std::cout << "createBuffers()" << endl;

  // Dades del terra
  // Dos VBOs, un amb posició i l'altre amb color
  glm::vec3 posterra[4] = {
  glm::vec3(-1.0, -1.0, -1.0),
  glm::vec3(-1.0, -1.0, 1.0),
  glm::vec3(1.0, -1.0, -1.0),
  glm::vec3(1.0, -1.0, 1.0)
  }; 
  glm::vec3 colterra[4] = {
  glm::vec3(1,0,1),
  glm::vec3(1,0,1),
  glm::vec3(1,0,1),
  glm::vec3(1,0,1)
  };

  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(1, &VBO_TerraPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_TerraCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colterra), colterra, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);



  patricio.load("./models/Patricio.obj");

  glGenVertexArrays(1,&VAO_Patricio);
  glBindVertexArray(VAO_Patricio);

  glGenBuffers(1,&VBO_PatricioPosicio);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatricioPosicio);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* patricio.faces().size()*3*3,patricio.VBO_vertices(), GL_STATIC_DRAW);

  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1,&VBO_PatricioColor);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatricioColor);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * patricio.faces().size()*3*3,patricio.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  calculaCapsaModel(patricio);

  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders() 
{
  std::cout << "carregaShaders()" << endl;

  // Creem els shaders per al fragment shader i el vertex shader
  QGLShader fs (QGLShader::Fragment, this);
  QGLShader vs (QGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag");
  vs.compileSourceFile("shaders/vertshad.vert");
  // Creem el program
  program = new QGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  // Uniform pojection
  projLoc = glGetUniformLocation (program->programId(), "proj");
  // posicio de la camera
  viewLoc = glGetUniformLocation (program->programId(), "view");
}

