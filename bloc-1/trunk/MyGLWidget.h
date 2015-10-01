#include <QGLWidget>
#include "glm/glm.hpp"
#include <QGLShader>
#include <QGLShaderProgram>
#include <QKeyEvent>

#define GLM_FORCE_RADIANS

class MyGLWidget : public QGLWidget 
{
  Q_OBJECT

  public:
    MyGLWidget (QGLFormat &f, QWidget *parent=0);
  
  protected:
    // initializeGL() - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ();

    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ();
 
    // resizeGL() - Es cridat quan canvi la mida del widget
    virtual void resizeGL (int width, int height);
/*
    virtual void mousePressEvent ( QMouseEvent * e ); // apretar el ratoli
    virtual void mouseReleaseEvent ( QMouseEvent * e ); // desapretar el ratoli
    virtual void mouseMoveEvent ( QMouseEvent * e ); //apretar i moure el ratoli
*/    
    virtual void keyPressEvent ( QKeyEvent * e ); // apretar una tecla del teclat

  private:
    void createBuffers ();

    void loadShaders();

    QGLShaderProgram *program;

    GLuint VAO, VBO, VBOcolor, pos, col;

    GLint varLoc;

    float scl;

};
