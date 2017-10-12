#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

OpenGLWidget::~ OpenGLWidget()
{
    destroyShaders();
    destroyVBOs();
}

void OpenGLWidget::initializeGL(){
    initializeOpenGLFunctions();

    qDebug("OpenGL version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    createShaders();
    createVBOs();
}

void OpenGLWidget::resizeGL(int width, int height){
    glViewport(0, 0, width, height);
}

void OpenGLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0,0,0,1);

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);
}

void OpenGLWidget::toggleBackgroundColor(bool changeBColor){
    makeCurrent();
    if (changeBColor)
        glClearColor(0,0,0,1);
    else
        glClearColor(1,1,1,1);
    update();
}

void OpenGLWidget::createShaders(){
    makeCurrent();
    destroyShaders();
    QString vertexShaderFile(":/shaders/vshader1.glsl");
    QString fragmentShaderFile(":/shaders/fshader1.glsl");

    QFile vs(vertexShaderFile);
    QFile fs(fragmentShaderFile);

    vs.open(QFile::ReadOnly | QFile::Text);
    fs.open(QFile::ReadOnly | QFile::Text);

    QTextStream streamVs(&vs), streamFs(&fs);

    QString qtStringVs = streamVs.readAll();
    QString qtStringFs = streamFs.readAll();

    std::string stdStringVs = qtStringVs.toStdString();
    std::string stdStringFs = qtStringFs.toStdString();


    GLuint vertexShader = 0;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *source = stdStringVs.c_str();
    glShaderSource(vertexShader, 1, &source, 0);

    glCompileShader(vertexShader);
    int isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE){
        int maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infolog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infolog[0]);
        qDebug("%s", &infolog[0]);

        glDeleteShader(vertexShader);
        return;
    }


    GLuint fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    source = stdStringFs.c_str();
    glShaderSource(fragmentShader, 1, &source, 0);

    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE){
        int maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infolog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infolog[0]);
        qDebug("%s", &infolog[0]);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    GLint isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);

    if(isLinked == GL_FALSE){
        int maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infolog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infolog[0]);
        qDebug("%s", &infolog[0]);

        glDeleteProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vs.close();
    fs.close();
}

void OpenGLWidget::destroyShaders(){
    makeCurrent();
    glDeleteProgram(shaderProgram);
}

void OpenGLWidget::createVBOs(){
    makeCurrent();
    destroyVBOs();

    vertices = std::make_unique<QVector4D []>(4);
    colors = std::make_unique<QVector4D []>(4);
    indices = std::make_unique<unsigned int []>(2 * 3);

    vertices[0] = QVector4D(-0.5, -0.5, 0, 1);
    vertices[1] = QVector4D(0.5, -0.5, 0, 1);
    vertices[2] = QVector4D(0.5, 0.5, 0, 1);
    vertices[3] = QVector4D(-0.5, 0.5, 0, 1);

    colors[0] = QVector4D(1, 0, 0, 1);
    colors[1] = QVector4D(0, 1, 0, 1);
    colors[2] = QVector4D(0, 0, 1, 1);
    colors[3] = QVector4D(1, 1, 0, 1);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QVector4D), vertices.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboColor);
    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QVector4D), colors.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QVector4D), indices.get(), GL_STATIC_DRAW);
}

void OpenGLWidget::destroyVBOs(){
    makeCurrent();

    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboColor);
    glDeleteBuffers(1, &vboIndices);
    glDeleteVertexArrays(1, &vao);

    vboVertices = 0;
    vboColor = 0;
    vboIndices = 0;
    vao = 0;
}
