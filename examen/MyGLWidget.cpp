#include <GL/glew.h>
#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QGLFormat &f, QWidget* parent) : QGLWidget(f, parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  angleY = M_PI/5.;
  angleX = M_PI/8.;
  movX = 0.;
  movZ = 0.;
  esGeneral = true;
  DoingInteractive = NONE;
  llumBlanca = true;
  colorLlum[0] = 1.;
  colorLlum[1] = 1.;
  colorLlum[2] = 1.;
}

void MyGLWidget::initializeGL ()
{
  // glew és necessari per cridar funcions de les darreres versions d'OpenGL
  glewExperimental = GL_TRUE;
  glewInit(); 
  glGetError();  // Reinicia la variable d'error d'OpenGL

  glClearColor(0.7, 0.7, 1.0, 1.0);  // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  loadShaders();
  createBuffers();
  projectTransform(M_PI/3.0, 2.0, 1.0, 60.0);
  viewTransform();
}

void MyGLWidget::paintGL ()
{
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Activem el VAO per a pintar el terra 
  glBindVertexArray (VAO_Terra);

  modelTransformTerra ();

  // pintem
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Activem el VAO per a pintar el Patricio
  glBindVertexArray (VAO_Patr);

  modelTransformPatricio1 ();

  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);

  modelTransformPatricio2 ();

  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);

  modelTransformPatricio3 ();

  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);

  modelTransformPatricio4 ();

  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);
  
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
  float rav = (float) w/h;
  // double raw = (double)width()/(double)height();
  if(rav <= 1) {
    float newAngle = 2 * atan(tan(M_PI/4.)/rav);
    projectTransform(newAngle,rav,1.,60.);
  } else {
    projectTransform(M_PI/2.0,rav,1.,60.);
  }
  glViewport(0, 0, w, h);
  //glViewport (0, 0, w, h);
}

void MyGLWidget::createBuffers ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  patr.load("./models/Patricio.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel ();
  
  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Patr);
  glBindVertexArray(VAO_Patr);

  // Creació dels buffers del model patr
  // Buffer de posicions
  glGenBuffers(1, &VBO_PatrPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glGenBuffers(1, &VBO_PatrNorm);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrNorm);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glGenBuffers(1, &VBO_PatrMatamb);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrMatamb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glGenBuffers(1, &VBO_PatrMatdiff);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrMatdiff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glGenBuffers(1, &VBO_PatrMatspec);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrMatspec);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glGenBuffers(1, &VBO_PatrMatshin);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatrMatshin);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3, patr.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  // Dades del terra
  // VBO amb la posició dels vèrtexs
  glm::vec3 posterra[4] = {
	glm::vec3( 10.0, 0.0, -10.0),
	glm::vec3( 10.0, 0.0,  10.0),
	glm::vec3(-10.0, 0.0, -10.0),
	glm::vec3(-10.0, 0.0,  10.0)
  }; 

  // VBO amb la normal de cada vèrtex
  glm::vec3 norm (0,1,0);
  glm::vec3 normterra[4] = {
	norm, norm, norm, norm
  };

  // Definim el material del terra
  glm::vec3 amb(0.,0.,0.1);
  glm::vec3 diff(0.6,0.6,0.0);
  //glm::vec3 diff(1.0,1.0,1.0);
  glm::vec3 spec(0.,0.,0.);
  float shin = 100;

  // Fem que aquest material afecti a tots els vèrtexs per igual
  glm::vec3 matambterra[4] = {
	amb, amb, amb, amb
  };
  glm::vec3 matdiffterra[4] = {
	diff, diff, diff, diff
  };
  glm::vec3 matspecterra[4] = {
	spec, spec, spec, spec
  };
  float matshinterra[4] = {
	shin, shin, shin, shin
  };

// Creació del Vertex Array Object del terra
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(1, &VBO_TerraPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_TerraNorm);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraNorm);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normterra), normterra, GL_STATIC_DRAW);

  // Activem l'atribut normalLoc
  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glGenBuffers(1, &VBO_TerraMatamb);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatamb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matambterra), matambterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glGenBuffers(1, &VBO_TerraMatdiff);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatdiff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matdiffterra), matdiffterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glGenBuffers(1, &VBO_TerraMatspec);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatspec);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matspecterra), matspecterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glGenBuffers(1, &VBO_TerraMatshin);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatshin);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matshinterra), matshinterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);

  glUniform3fv(colorLlumLoc,1,&colorLlum[0]);
}

void MyGLWidget::loadShaders ()
{
  // Creem els shaders per al fragment i vertex shader
  QGLShader fs(QGLShader::Fragment, this);
  QGLShader vs(QGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem 
  fs.compileSourceFile("./shaders/fragshad.frag");
  vs.compileSourceFile("./shaders/vertshad.vert");

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
  // Obtenim identificador per a l'atribut “normal” del vertex shader
  normalLoc = glGetAttribLocation (program->programId(), "normal");
  // Obtenim identificador per a l'atribut “matamb” del vertex shader
  matambLoc = glGetAttribLocation (program->programId(), "matamb");
  // Obtenim identificador per a l'atribut “matdiff” del vertex shader
  matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
  // Obtenim identificador per a l'atribut “matspec” del vertex shader
  matspecLoc = glGetAttribLocation (program->programId(), "matspec");
  // Obtenim identificador per a l'atribut “matshin” del vertex shader
  matshinLoc = glGetAttribLocation (program->programId(), "matshin");

  colorLlumLoc = glGetUniformLocation(program->programId(), "colorLlum");

  // Demanem identificadors per als uniforms del vertex shader
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");
}

void MyGLWidget::modelTransformPatricio1 ()
{
  glm::mat4 TG;  // Matriu de transformació
  TG = glm::translate(glm::mat4(1.0), glm::vec3(8.0, 0.0, 8.0));
  TG = glm::translate(TG, glm::vec3(movX, 0.0, movZ));
  TG = glm::rotate(TG, float(M_PI+(M_PI/4)), glm::vec3(0.0, 1.0, 0.0));  
  TG = glm::scale(TG, glm::vec3(escala));  
  TG = glm::translate(TG, -centreBasePatr);  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPatricio2 ()
{
  glm::mat4 TG;  // Matriu de transformació
  TG = glm::translate(glm::mat4(1.0), glm::vec3(8.0, 0.0, -8.0));
  TG = glm::translate(TG, glm::vec3(movX, 0.0, -movZ));
  TG = glm::rotate(TG, float(2*M_PI-(M_PI/4)), glm::vec3(0.0, 1.0, 0.0));  
  TG = glm::scale(TG, glm::vec3(escala));  
  TG = glm::translate(TG, -centreBasePatr);  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPatricio3 ()
{
  glm::mat4 TG;  // Matriu de transformació
  TG = glm::translate(glm::mat4(1.0), glm::vec3(-8.0, 0.0, -8.0));
  TG = glm::translate(TG, glm::vec3(-movX, 0.0, -movZ));
  TG = glm::rotate(TG, float(M_PI/4), glm::vec3(0.0, 1.0, 0.0));  
  TG = glm::scale(TG, glm::vec3(escala));  
  TG = glm::translate(TG, -centreBasePatr);  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPatricio4 ()
{
  glm::mat4 TG;  // Matriu de transformació
  TG = glm::translate(glm::mat4(1.0), glm::vec3(-8.0, 0.0, 8.0));
  TG = glm::translate(TG, glm::vec3(-movX, 0.0, movZ));
  TG = glm::rotate(TG, float(M_PI-(M_PI/4)), glm::vec3(0.0, 1.0, 0.0));  
  TG = glm::scale(TG, glm::vec3(escala));  
  TG = glm::translate(TG, -centreBasePatr);  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformTerra ()
{
  glm::mat4 TG;  // Matriu de transformació
  TG = glm::mat4(1.f);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform (float angle,float rav,float znear,float zfar)
{
  glm::mat4 Proj;  // Matriu de projecció
  Proj = glm::perspective(angle, rav, znear , zfar);
  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform ()
{
  if(esGeneral){
    glm::mat4 View;  // Matriu de posició i orientació
    View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -30));
    View = glm::rotate(View, angleX, glm::vec3(1, 0, 0));
    View = glm::rotate(View, -angleY, glm::vec3(0, 1, 0));
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
  } else {
    
    glm::mat4 View = glm::lookAt (glm::vec3(8.0+movX,2.,8.0+movZ), glm::vec3(0,0,0), glm::vec3(0,1,0));
    //View = glm::translate(View, glm::vec3(movX, 0.0, movZ));
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
  }
  
}

void MyGLWidget::calculaCapsaModel ()
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = patr.vertices()[0];
  miny = maxy = patr.vertices()[1];
  minz = maxz = patr.vertices()[2];
  for (unsigned int i = 3; i < patr.vertices().size(); i+=3)
  {
    if (patr.vertices()[i+0] < minx)
      minx = patr.vertices()[i+0];
    if (patr.vertices()[i+0] > maxx)
      maxx = patr.vertices()[i+0];
    if (patr.vertices()[i+1] < miny)
      miny = patr.vertices()[i+1];
    if (patr.vertices()[i+1] > maxy)
      maxy = patr.vertices()[i+1];
    if (patr.vertices()[i+2] < minz)
      minz = patr.vertices()[i+2];
    if (patr.vertices()[i+2] > maxz)
      maxz = patr.vertices()[i+2];
  }
  escala = 2.0/(maxy-miny);
  centreBasePatr[0] = (minx+maxx)/2.0; centreBasePatr[1] = miny; centreBasePatr[2] = (minz+maxz)/2.0;
}

void MyGLWidget::keyPressEvent (QKeyEvent *e)
{
  switch (e->key())
  {
    case Qt::Key_Escape:
        exit(0);
    case Qt::Key_W:
        if(movX >= -7.){
          movX = movX -0.1;
          movZ = movZ -0.1;
          viewTransform();
         /* modelTransformPatricio1();
          modelTransformPatricio2();
          modelTransformPatricio3();
          modelTransformPatricio4();
          viewTransform()*/
          updateGL();
        }
        break;
    case Qt::Key_S:
        movX = movX +0.1;
        movZ = movZ +0.1;
        viewTransform();
        break;
    case Qt::Key_C:
        esGeneral = !esGeneral;
        viewTransform();
        break;
    case Qt::Key_L:
        if(llumBlanca == false){
          llumBlanca = true;
          colorLlum[0] = 1.;
          colorLlum[1] = 1.;
          colorLlum[2] = 1.;
        }else{
          llumBlanca = false;
          colorLlum[0] = 0.0;
          colorLlum[1] = 0.0;
          colorLlum[2] = 1.0;
        }
        glUniform3fv(colorLlumLoc,1,&colorLlum[0]);
        updateGL();
        break;
    default: e->ignore(); break;
  }
  updateGL();
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  // Aqui cal que es calculi i s'apliqui la rotacio o el zoom com s'escaigui...
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleY += (e->x() - xClick) * M_PI / 180.0;
    angleX += (yClick - e->y()) * M_PI / 180.0;
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  updateGL ();
}


