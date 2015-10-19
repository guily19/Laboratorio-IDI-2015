#define GLM_FORCE_RADIANS
#include <QGLWidget>
#include <QGLShader>
#include <QGLShaderProgram>
#include <QKeyEvent>
#include <cmath>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Model/model.h"

class MyGLWidget : public QGLWidget {
  Q_OBJECT

  public:
    MyGLWidget (QGLFormat &f, QWidget *parent=0);

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

  private:
    void createBuffers ();
    void carregaShaders ();
    void modelTransform ();
    void modelTransformPatricio ();
    void projectTransform(float angle,float ra,float znear,float zfar);
    void viewTransform();
    void calculaCapsaModel(Model model);

    Model patricio;

    // attribute locations
    GLuint vertexLoc, colorLoc;

    // uniform locations
    GLuint transLoc;
    //projection
    GLuint projLoc;
    //posicio de la camera
    GLuint viewLoc;
    // VAO i VBO names
    GLuint VAO_Casa, VBO_CasaPos, VBO_CasaCol;
    GLuint VAO_Terra, VBO_TerraPos, VBO_TerraCol;
    GLuint VAO_Patricio, VBO_PatricioColor, VBO_PatricioPosicio;

    glm::vec3 centreModel;

    // Program
    QGLShaderProgram *program;
    // Internal vars
    float scale, angle, escalaModel;
    glm::vec3 pos;
};

