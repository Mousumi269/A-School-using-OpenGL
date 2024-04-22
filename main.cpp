

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "bed.h"
#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "directionalLight.h"
#include "SpotLight.h"
#include "walls.h"
#include "bench.h"
#include "swing.h"
#include "stand.h"
#include "sphere.h"
#include "cube.h"
#include "stb_image.h"
#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
void processInput(GLFWwindow* window);
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);

long long nCr(int n, int r);
void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L);
unsigned int hollowBezier(GLfloat ctrlpoints[], int L);


//void Tree_Making(unsigned int& cubeVAO, unsigned int& cVAO, Shader& lightingShader, glm::mat4 alTogether, Shader& lightingShaderWithTexture);


void drawSphere(Shader& lightingShader, glm::mat4 alTogether, float x, float y, float z, float h, float w, float b, float red, float green, float blue)
{
    Sphere sphere = Sphere(0.50f, 10, 20, glm::vec3(red, green, blue), glm::vec3(red, green, blue), glm::vec3(red, green, blue), glm::vec3(red, green, blue), 1.0f);
    glm::mat4 modelForSphere = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::mat4(1.0f);
    translate = glm::translate(modelForSphere, glm::vec3(x, y, z));
    scale = glm::scale(modelForSphere, glm::vec3(h, w, b));
    modelForSphere = alTogether * translate * scale;
    sphere.drawSphere(lightingShader, modelForSphere);

}
    

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;
float angle_ball = 0.0f;
float ball_z = 5.0f;
float ball_x = 0.0f;
//bool ball_shoot = false;
float bz = 0.0f;
float bar_x = 0.0f;

GLint viewport[4];

/////////////////////////////////////////////////////////////
float wcsClkDn[3], wcsClkUp[3];
vector <float> cntrlPoints;
vector <float> coordinates;
vector <float> normals;
vector <int> indices;
vector <float> vertices;
class point
{
public:
    point()
    {
        x = 0;
        y = 0;
    }
    int x;
    int y;
} clkpt[2];
int mouseButtonFlag = 0;
FILE* fp;
const double pi = 3.14159265389;
const int nt = 40;
const int ntheta = 20;
bool showControlPoints = true;
bool loadBezierCurvePoints = false;
bool showHollowBezier = false;
bool lineMode = false;
unsigned int bezierVAO;

void drawCurve(unsigned int& bezierVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b)
{
    lightingShader.use();
    lightingShader.setMat4("model", model);
    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("material.shininess", 32.0f);

    glBindVertexArray(bezierVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}
bool fanOn = false,doorOn=false,doorOp=false,doOn=false, doOp=false,flagOn=false,flagOp=false,gateOn=false;
float r = 0.0,t=3.0,b=1.4,d=0.0,c=10.8,e=0.0,f=26.0,g=0.0,h=0.0,a=0.0,k=5.0,l=0.0,m=0.0,n=1.4,o=-1.6,p=0.0,s=0.0,q=0.0,x=0.0;

// camera
Camera camera(glm::vec3(0.0f, 1.1f, 5.2f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);


// positions of the point lights

glm::vec3 pointLightPositions[] = {
    glm::vec3(-3.5f,  8.15f,  -6.6+0.25f),
    glm::vec3(3.5f,  4.15f,   -6.6 +0.25f),
    
};

PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
        // ambient
    0.3f, 0.3f, 0.3f,     // diffuse
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.08f,  //k_l
    0.032, //k_q
    1       // light number
);


PointLight pointlight3(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.5f, 0.5f, 0.5f,     // ambient
    0.0f, 1.0f, 1.0f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.08f,  //k_l
    0.032f, //k_q
    2       // light number
);



DirLight dirLight(
        -0.2f, -1.0f, -0.3f,//direction
         0.5f, 0.5f, 0.5f,//ambient 0.05f, 0.05f, 0.05
        0.8f, 0.8f, 0.8f, //    diffuse .5f, 0.5f, 0.5f, 
       0.1f, 0.1f, 0.1f //specular 0.5f, 0.5f, 0.5f
);

SpotLight spotLight(
    -3.50f, 4.0f, -2.0f,//position
    0.6f, -1.0f, 0.5f, //direction
    0.5f, 0.0f, 0.5f,//ambient
    1.0f, 0.0f, 1.0f,//diffuse
    1.0f, 1.0f, 1.0f,//specular
    1.0f,//k_c
    0.09f, //k_l
    0.032f, //k_q
    13.5f, //cos_theta
    15.0f
);

// light settings
bool PointToggle1 = true;
bool PointToggle3 = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;


// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

vector<float> tree = {
 
-0.5800, 1.6050, 5.1000,
-0.7500, 1.3100, 5.1000,
-0.7050, 1.0100, 5.1000,
-0.4050, 0.8300, 5.1000,

};

/*Cube tree = Cube(glm: : vec3(1, 0, 0));
tre.initiate(tree);*/

long long nCr(int n, int r)
{
    if (r > n / 2)
        r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for (i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

//polynomial interpretation for N points
void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L)
{
    double y = 0;
    double x = 0;
    t = t > 1.0 ? 1.0 : t;
    for (int i = 0; i < L + 1; i++)
    {
        long long ncr = nCr(L, i);
        double oneMinusTpow = pow(1 - t, double(L - i));
        double tPow = pow(t, double(i));
        double coef = oneMinusTpow * tPow * ncr;
        x += coef * ctrlpoints[i * 3];
        y += coef * ctrlpoints[(i * 3) + 1];

    }
    xy[0] = float(x);
    xy[1] = float(y);
}

unsigned int hollowBezier(GLfloat ctrlpoints[], int L)
{
    int i, j;
    float x, y, z, r;                //current coordinates
    float theta;
    float nx, ny, nz, lengthInv;    // vertex normal


    const float dtheta = 2 * pi / ntheta;        //angular step size

    float t = 0;
    float dt = 1.0 / nt;
    float xy[2];

    for (i = 0; i <= nt; ++i)              //step through y
    {
        BezierCurve(t, xy, ctrlpoints, L);
        r = xy[0];
        y = xy[1];
        theta = 0;
        t += dt;
        lengthInv = 1.0 / r;

        for (j = 0; j <= ntheta; ++j)
        {
            double cosa = cos(theta);
            double sina = sin(theta);
            z = r * cosa;
            x = r * sina;

            coordinates.push_back(x);
            coordinates.push_back(y);
            coordinates.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            // center point of the circle (0,y,0)
            nx = (x - 0) * lengthInv;
            ny = (y - y) * lengthInv;
            nz = (z - 0) * lengthInv;

            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            theta += dtheta;
        }
    }

    // generate index list of triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1

    int k1, k2;
    for (int i = 0; i < nt; ++i)
    {
        k1 = i * (ntheta + 1);     // beginning of current stack
        k2 = k1 + ntheta + 1;      // beginning of next stack

        for (int j = 0; j < ntheta; ++j, ++k1, ++k2)
        {
            // k1 => k2 => k1+1
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            // k1+1 => k2 => k2+1
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }

    size_t count = coordinates.size();
    for (int i = 0; i < count; i += 3)
    {
        vertices.push_back(coordinates[i]);
        vertices.push_back(coordinates[i + 1]);
        vertices.push_back(coordinates[i + 2]);

        vertices.push_back(normals[i]);
        vertices.push_back(normals[i + 1]);
        vertices.push_back(normals[i + 2]);
    }

    unsigned int bezierVAO;
    glGenVertexArrays(1, &bezierVAO);
    glBindVertexArray(bezierVAO);

    // create VBO to copy vertex data to VBO
    unsigned int bezierVBO;
    glGenBuffers(1, &bezierVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);           // for vertex data
    glBufferData(GL_ARRAY_BUFFER,                   // target
        (unsigned int)vertices.size() * sizeof(float), // data size, # of bytes
        vertices.data(),   // ptr to vertex data
        GL_STATIC_DRAW);                   // usage

    // create EBO to copy index data
    unsigned int bezierEBO;
    glGenBuffers(1, &bezierEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierEBO);   // for index data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
        (unsigned int)indices.size() * sizeof(unsigned int),             // data size, # of bytes
        indices.data(),               // ptr to index data
        GL_STATIC_DRAW);                   // usage

    // activate attrib arrays
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // set attrib arrays with stride and offset
    int stride = 24;     // should be 24 bytes
    glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));

    // unbind VAO, VBO and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return bezierVAO;
}

// glfw: whenever the mouse moves, this callback is called


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, //back side of cube, surface normal on -z 
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, //
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    bezierVAO = hollowBezier(tree.data(), ((unsigned int)tree.size() / 3) - 1);
    

        glm::mat4 modelForKing = glm::mat4(1.0f);
        glm::mat4 translate1 = glm::mat4(1.0f);
        glm::mat4 scale1 = glm::mat4(1.0f);
        glm::mat4 rotate1 = glm::mat4(1.0f);
       // translate = glm::translate(modelForKing, glm::vec3(x, y, z));
       // scale = glm::scale(modelForKing, glm::vec3(h, w, b));
       // modelForKing = alTogether * translate * scale;
        //drawCurve(bezierVAO, lightingShader, modelForKing, r, g, bl);

        /*modelForKing = glm::mat4(1.0f);
        translate1 = glm::mat4(1.0f);
        scale1 = glm::mat4(1.0f);
        rotate1 = glm::mat4(1.0f);
        modelForKing =  translate1 * scale1;
        drawSphere(lightingShader, modelForKing, 0.5, 0.505, 0.5, 1.09f, 1.03f, 1.09f, 1, 1, 1);*/

        /*modelForKing = glm::mat4(1.0f);
        translate = glm::mat4(1.0f);
        scale = glm::mat4(1.0f);
        rotate = glm::mat4(1.0f);
        modelForKing = alTogether * translate * scale;
        drawSphere(lightingShader, alTogether * modelForKing, x, 0.505, z, 0.105f, 0.045f, 0.105f, r, g, bl);

        modelForKing = glm::mat4(1.0f);
        translate = glm::mat4(1.0f);
        scale = glm::mat4(1.0f);
        rotate = glm::mat4(1.0f);
        modelForKing = alTogether * translate * scale;
        drawSphere(lightingShader, alTogether * modelForKing, x, 0.325, z, 0.15f, 0.075f, 0.15f, r, g, bl);

        modelForKing = glm::mat4(1.0f);
        translate = glm::mat4(1.0f);
        scale = glm::mat4(1.0f);
        rotate = glm::mat4(1.0f);
        translate = glm::translate(modelForKing, glm::vec3(cube_x - 0.01, cube_y + 0.375, cube_z));
        scale = glm::scale(modelForKing, glm::vec3(0.02f, 0.06f, 0.02f));
        modelForKing = alTogether * translate * scale;
        drawCube(cubeVAO, lightingShader, alTogether * modelForKing, r, g, bl);

        modelForKing = glm::mat4(1.0f);
        translate = glm::mat4(1.0f);
        scale = glm::mat4(1.0f);
        rotate = glm::mat4(1.0f);
        translate = glm::translate(modelForKing, glm::vec3(cube_x - 0.0325, cube_y + 0.395, cube_z));
        scale = glm::scale(modelForKing, glm::vec3(0.06f, 0.02f, 0.02f));
        modelForKing = alTogether * translate * scale;
        drawCube(cubeVAO, lightingShader, alTogether * modelForKing, r, g, bl);*/

    

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    string diffuseMapPath = "./container2.jpg";
    string specularMapPath = "./container2.jpg";

  

    unsigned int diffMap = loadTexture(diffuseMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap = loadTexture(specularMapPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    
    string diffuseMapPath1 = "./windw.jpg";
    string specularMapPath1 = "./windw.jpg";

    unsigned int diffMap1 = loadTexture(diffuseMapPath1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap1 = loadTexture(specularMapPath1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath2 = "./road.jpg";
    string specularMapPath2 = "./road.jpg";

    unsigned int diffMap2 = loadTexture(diffuseMapPath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap2 = loadTexture(specularMapPath2.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath3 = "./toilets.png";
    string specularMapPath3 = "./toilets.png";

    unsigned int diffMap3 = loadTexture(diffuseMapPath3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap3 = loadTexture(specularMapPath3.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath4 = "./class1.png";
    string specularMapPath4 = "./class1.png";

    unsigned int diffMap4 = loadTexture(diffuseMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap4 = loadTexture(specularMapPath4.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath5 = "./class2.png";
    string specularMapPath5 = "./class2.png";

    unsigned int diffMap5 = loadTexture(diffuseMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap5 = loadTexture(specularMapPath5.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath6 = "./class3.png";
    string specularMapPath6 = "./class3.png";

    unsigned int diffMap6 = loadTexture(diffuseMapPath6.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap6 = loadTexture(specularMapPath6.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath7 = "./class4.png";
    string specularMapPath7 = "./class4.png";

    unsigned int diffMap7 = loadTexture(diffuseMapPath7.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap7 = loadTexture(specularMapPath7.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath8 = "./troom.png";
    string specularMapPath8 = "./troom.png";

    unsigned int diffMap8 = loadTexture(diffuseMapPath8.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap8 = loadTexture(specularMapPath8.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath9 = "./whitefloor.jpg";
    string specularMapPath9 = "./whitefloor.jpg";

    unsigned int diffMap9 = loadTexture(diffuseMapPath9.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap9 = loadTexture(specularMapPath9.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath10 = "./grnd.jpg";
    string specularMapPath10 = "./grnd.jpg";

    unsigned int diffMap10 = loadTexture(diffuseMapPath10.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap10 = loadTexture(specularMapPath10.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath11 = "./treebmp.jpg";
    string specularMapPath11 = "./treebmp.jpg";

    unsigned int diffMap11 = loadTexture(diffuseMapPath11.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap11 = loadTexture(specularMapPath11.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath12 = "./school.png";
    string specularMapPath12 = "./school.png";

    unsigned int diffMap12 = loadTexture(diffuseMapPath12.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap12 = loadTexture(specularMapPath12.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    string diffuseMapPath13 = "./sky.jpg";
    string specularMapPath13 = "./sky.jpg";

    unsigned int diffMap13 = loadTexture(diffuseMapPath13.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int specMap13 = loadTexture(specularMapPath13.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);



    //std::cout << diffMap << ' ' << specMap << std::endl;

    Cube cube = Cube(diffMap, specMap, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube1 = Cube(diffMap1, specMap1, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube2 = Cube(diffMap2, specMap2, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube3 = Cube(diffMap3, specMap3, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube4 = Cube(diffMap4, specMap4, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube5 = Cube(diffMap5, specMap5, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube6 = Cube(diffMap6, specMap6, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube7 = Cube(diffMap7, specMap7, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube8 = Cube(diffMap8, specMap8, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube9 = Cube(diffMap9, specMap9, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube10 = Cube(diffMap10, specMap10, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube11 = Cube(diffMap11, specMap11, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube12 = Cube(diffMap12, specMap12, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    Cube cube13 = Cube(diffMap13, specMap13, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);


    //ourShader.use();
    //lightingShader.use();

    // render loop
    // -----------

    //Curve Leaf(leaf);
    //tre = &Leaf;
    /*bar.setRadius(0.1);
    bar.diffuse = glm::vec3(1.0, 1.0, 1.0);
    bar.ambient = glm::vec3(1.0, 1.0, 1.0);
    bar.specular = glm::vec3(1.0, 1.0, 1.0);*/
    //ourShader.use();
    //lightingShader.use();

    //Sphere2 ball = Sphere2();

    camera.Position = glm::vec3(13.0, 6.0, 3.0);
   
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // point light 1
        pointlight1.setUpPointLight(lightingShader);
        // point light 2
        //pointlight2.setUpPointLight(lightingShader);
        // point light 3
        pointlight3.setUpPointLight(lightingShader);
        // point light 4
        //pointlight4.setUpPointLight(lightingShader);
       dirLight.setUpDirLight(lightingShader);
       spotLight.setUpSpotLight(lightingShader);


        lightingShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        lightingShader.setMat4("view", view);

        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        //glBindVertexArray(cubeVAO);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
       // bed(cubeVAO, lightingShader, model); 
       //here the model matrix is the alTogether matrix


            walls(cubeVAO, lightingShader, model);
            //stand(cubeVAO, lightingShader, model);
            swing(cubeVAO, lightingShader, model);
        
        
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5, 0.0, 0.5));

        model = model *translateMatrix* rotateXMatrix;
        bench1(cubeVAO, lightingShader, model);


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, translate_Y, -1.5));

        model = model * translateMatrix * rotateXMatrix;
        bench2(cubeVAO, lightingShader, model);
        //sphere
        modelForKing = glm::mat4(1.0f);
        translate1 = glm::mat4(1.0f);
        scale1 = glm::mat4(1.0f);
        rotate1 = glm::mat4(1.0f);
        modelForKing = translate1 * scale1;
        drawSphere(lightingShader, modelForKing, 38, 25, -7, 2.750f, 2.750f, 2.750f, 1, 0.753, 0);


        //curve
        glm::mat4 modelForKing2 = glm::mat4(1.0f);
        glm::mat4 translate2 = glm::mat4(1.0f);
        glm::mat4 scale2 = glm::mat4(1.0f);
       // glm::mat4 rotate2 = glm::mat4(1.0f);
        translate2= glm::translate(modelForKing2, glm::vec3(-4.0,1.0,17.5));
        scale2 = glm::scale(modelForKing, glm::vec3(4, 4.0, 2));
        modelForKing =  translate2 * scale2;
        drawCurve(bezierVAO, lightingShader, modelForKing, 0, 1, 0);

        

        translate2 = glm::translate(modelForKing2, glm::vec3(54.8, -.4, -7.5));
        scale2 = glm::scale(modelForKing, glm::vec3(1, 1.0, 1));
        modelForKing = translate2 * scale2;
        drawCurve(bezierVAO, lightingShader, modelForKing, 0, 1, 0);

       /* translate2 = glm::translate(modelForKing2, glm::vec3(45, -.4, -6));
        scale2 = glm::scale(modelForKing, glm::vec3(1, 1.0, 1));
        modelForKing = translate2 * scale2;
        drawCurve(bezierVAO, lightingShader, modelForKing, 0, 1, 0);*/

       /* glm::mat4 modelForKing3 = glm::mat4(1.0f);
        glm::mat4 translate3 = glm::mat4(1.0f);
        glm::mat4 scale3 = glm::mat4(1.0f);
        //glm::mat4 rotate3 = glm::mat4(1.0f);
        translate3 = glm::translate(modelForKing3, glm::vec3(45.7, .3,1.30));
        scale3 = glm::scale(modelForKing, glm::vec3(1.0, 1.0, 1.0));
        modelForKing = translate3 * scale3;
        drawCurve(bezierVAO, lightingShader, modelForKing, 0, 1, 0);*/

        /*glm::mat4 modelForKing4 = glm::mat4(1.0f);
        glm::mat4 translate4 = glm::mat4(1.0f);
        glm::mat4 scale4 = glm::mat4(1.0f);
        glm::mat4 rotate4 = glm::mat4(1.0f);
        translate4 = glm::translate(modelForKing4, glm::vec3(-4.0, 1.0, 17.5));
        scale4 = glm::scale(modelForKing, glm::vec3(4.0, 4.0, 4.0));
        modelForKing = translate4 * scale4;
        drawCurve(bezierVAO, lightingShader, modelForKing, 0, 1, 0);*/

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5, translate_Y, 0.5));

        model = model * translateMatrix * rotateXMatrix;
        bench1(cubeVAO, lightingShader, model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, translate_Y, -1.5));

        model = model * translateMatrix * rotateXMatrix;
        bench2(cubeVAO, lightingShader, model);


        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0, translate_Y, 0.5));

        model = model * translateMatrix * rotateXMatrix;
        bench1(cubeVAO, lightingShader, model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, translate_Y, -1.5));

        model = model * translateMatrix * rotateXMatrix;
        bench2(cubeVAO, lightingShader, model);


        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.5, translate_Y, 0.5));

        model = model * translateMatrix * rotateXMatrix;
        bench1(cubeVAO, lightingShader, model);


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, translate_Y, -1.5));

        model = model * translateMatrix * rotateXMatrix;
        bench2(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5, 0.0, 2.0));

        model = model * translateMatrix * rotateXMatrix;
        bench1(cubeVAO, lightingShader, model);


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 0.0, -1.5));

        model = model * translateMatrix * rotateXMatrix;
        bench2(cubeVAO, lightingShader, model);


        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0, 0.0, 2.0));

        model = model * translateMatrix * rotateXMatrix;
        bench1(cubeVAO, lightingShader, model);


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 0.0, -1.5));

        model = model * translateMatrix * rotateXMatrix;
        bench2(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.5, 0.0, 2.0));

        model = model * translateMatrix * rotateXMatrix;
        bench1(cubeVAO, lightingShader, model);


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 0.0, -1.5));

        model = model * translateMatrix * rotateXMatrix;
        bench2(cubeVAO, lightingShader, model);

        


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 0.0, -1.5));

        model = model * translateMatrix * rotateXMatrix;
        bench2(cubeVAO, lightingShader, model);


        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5, 0.0, 2.0));

        model = model * translateMatrix * rotateXMatrix;
        bench1(cubeVAO, lightingShader, model);


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, 0.0, -1.5));

        model = model * translateMatrix * rotateXMatrix;
        bench2(cubeVAO, lightingShader, model);


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2, 0.0, 4.5));

        model = model * translateMatrix * rotateXMatrix;
        bench3(cubeVAO, lightingShader, model);

        
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.7, 0.0, -1.0));

        model = model * translateMatrix * rotateXMatrix;
        bench4(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(28.5, 0.0, -25.0));

        model = model * translateMatrix * rotateXMatrix;
        bench5(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);


        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(24.5, 5.0, 2.5));

        model = model * translateMatrix * rotateXMatrix;
        bench6(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(24.5, 5.0, 0.05));

        model = model * translateMatrix * rotateXMatrix;
        bench6(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(25.2, 5.0, -2.0));

        model = model * translateMatrix * rotateXMatrix;
        bench6(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(27.8, 5.0, -2.0));

        model = model * translateMatrix * rotateXMatrix;
        bench6(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(35.4, -0.1, -25.0));

        model = model * translateMatrix * rotateXMatrix;
        bench7(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(35.4, 0.8, -25.0));

        model = model * translateMatrix * rotateXMatrix;
        bench7(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(35.4, 1.7, -25.0));

        model = model * translateMatrix * rotateXMatrix;
        bench7(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(35.4, 2.6, -25.0));

        model = model * translateMatrix * rotateXMatrix;
        bench7(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(35.4, 3.6, -25.0));

        model = model * translateMatrix * rotateXMatrix;
        bench7(cubeVAO, lightingShader, model);



        

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(59.4, -0.4, 2.55));

        model = model * translateMatrix * rotateXMatrix;
        bench8(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(59.4, -0.4, 7.0));

        model = model * translateMatrix * rotateXMatrix;
        bench8(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(60.4, -0.2, 2.55));

        model = model * translateMatrix * rotateXMatrix;
        bench9(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(35.4, -.5, 2.55));

        model = model * translateMatrix * rotateXMatrix;
        bench17(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(23.4, -.5, -2.55));

        model = model * translateMatrix * rotateXMatrix;
        bench18(cubeVAO, lightingShader, model);

        //translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y,translate_Z));

        /*translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.4, -0.5, 4.55));//-1.6

        model = model *  translateMatrix * rotateXMatrix;
        bench11(cubeVAO, lightingShader, model);*/

      
        

        /*translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.4, -0.5, 4.55));//-1.6

        model = model * translateMatrix * rotateXMatrix;
        bench11(cubeVAO, lightingShader, model);*/

        /*translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.8, -0.5, 4.55));//7.8

        model = model  * translateMatrix *  rotateXMatrix;
        bench11(cubeVAO, lightingShader, model);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.8, -0.5, 4.55));//7.8

        model = model * translateMatrix * rotateXMatrix;
        bench11(cubeVAO, lightingShader, model);*/

        //flag
        /*translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.35, 5.0, 13.0));//-1.6

        model = model * translateMatrix * rotateXMatrix;
        bench13(cubeVAO, lightingShader, model);*/

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(19.0, 4.98, 12.88));//-1.6

        model = model * translateMatrix * rotateXMatrix;
        bench14(cubeVAO, lightingShader, model);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.6,-0.4, 13.55));//-1.6

        model = model * translateMatrix * rotateXMatrix;
        bench15(cubeVAO, lightingShader, model);

        /*if (flagOn)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            if (k > 3.0)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(18.35, k, 13.0));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench13(cubeVAO, lightingShader, model);
                k -= .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(18.35, 3.0, 13.0));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench13(cubeVAO, lightingShader, model);
                a= 1;
            }

        }
        else if (!flagOn)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (k < 5.01)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(18.35, k, 13.0));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench13(cubeVAO, lightingShader, model);
                k += .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(18.35, 5.01, 13.0));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench13(cubeVAO, lightingShader, model);
            }
        }*/

       /* void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
        {
            lightingShader.use();

            lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
            lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
            lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            lightingShader.setFloat("material.shininess", 32.0f);

            lightingShader.setMat4("model", model);

            glBindVertexArray(cubeVAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        c
        void Tree_Making(unsigned int& cubeVAO, unsigned int& cVAO, Shader& lightingShader, glm::mat4 alTogether, Shader& lightingShaderWithTexture)
        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 translate = glm::mat4(1.0f);
            glm::mat4 translate2 = glm::mat4(1.0f);
            glm::mat4 translate3 = glm::mat4(1.0f);
            glm::mat4 scale = glm::mat4(1.0f);
            glm::mat4 scale2 = glm::mat4(1.0f);
            glm::mat4 rotate = glm::mat4(1.0f);
            scale = glm::scale(model, glm::vec3(1, 4, 1));
            translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
            model = alTogether * scale * translate;

            tre->draw(lightingShader, model);

            rotate = glm::mat4(1.0f);


            translate2 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3, 5, -0.2));
            model = glm::mat4(1.0f);
            scale = glm::scale(model, glm::vec3(0.7, 1.5, 0.6));

            rotate = glm::rotate(rotate, glm::radians(45.0f), glm::vec3(0, 0, 1));
            model = alTogether * translate2 * rotate * scale * translate * glm::mat4(1.0f);
            tre->draw(lightingShader, model);


            rotate = glm::mat4(1.0f);


            translate2 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.2, 6, -0.2));
            model = glm::mat4(1.0f);
            scale = glm::scale(model, glm::vec3(0.6, 1.2, 0.5));

            rotate = glm::rotate(rotate, glm::radians(-45.0f), glm::vec3(0, 0, 1));
            model = alTogether * translate2 * rotate * scale * translate * glm::mat4(1.0f);
            tre->draw(lightingShader, model);

            rotate = glm::mat4(1.0f);


            translate2 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3, 7, -0.2));
            model = glm::mat4(1.0f);
            scale = glm::scale(model, glm::vec3(0.5, 1, 0.4));

            rotate = glm::rotate(rotate, glm::radians(45.0f), glm::vec3(0, 0, 1));
            model = alTogether * translate2 * rotate * scale * translate * glm::mat4(1.0f);
            tre->draw(lightingShader, model);

            rotate = glm::mat4(1.0f);


            translate2 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.2, 7.5, -0.2));
            model = glm::mat4(1.0f);
            scale = glm::scale(model, glm::vec3(0.4, 0.8, 0.3));

            rotate = glm::rotate(rotate, glm::radians(-45.0f), glm::vec3(0, 0, 1));
            model = alTogether * translate2 * rotate * scale * translate * glm::mat4(1.0f);
            tre->draw(lightingShader, model);


            rotate = glm::mat4(1.0f);


            translate2 = glm::translate(glm::mat4(1.0f), glm::vec3(3.6, -1, 1.3));
            model = glm::mat4(1.0f);
            scale = glm::scale(model, glm::vec3(8, 6, 4));

            rotate = glm::rotate(rotate, glm::radians(-45.0f), glm::vec3(0, 0, 1));
            model = alTogether * translate2 * scale * translate * glm::mat4(1.0f);
            lef->draw(lightingShader, model);


        }*/

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        if (k == 5)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(18.35, k, 12.88));
            k = 4.95;
        }
        else
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(18.35, 4.95, 12.88));
            k = 5;
        }
        model = model * translateMatrix * rotateXMatrix;
        bench13(cubeVAO, lightingShader, model);


        if (doorOn)
        {

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (b > -1.6)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(b, -0.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                b -= .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.6, -0.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                d = 1;
            }

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (b > -1.6)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(b, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                b -= .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.6, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                d = 1;
            }
            
        }
      
        

        else if(!doorOn)
        {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
                rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
                rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
                model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
                lightingShader.setMat4("model", model);

                rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

                if (b < 1.4)
                {
                    translateMatrix = glm::translate(identityMatrix, glm::vec3(b, -0.5, 4.55));//-1.6

                    model = model * translateMatrix * rotateXMatrix;
                    bench11(cubeVAO, lightingShader, model);
                    b += .005;
                }
                else
                {
                    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.4, -0.5, 4.55));//-1.6

                    model = model * translateMatrix * rotateXMatrix;
                    bench11(cubeVAO, lightingShader, model);
                }

                translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
                rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
                rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
                model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
                lightingShader.setMat4("model", model);

                rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

                if (b < 1.4)
                {
                    translateMatrix = glm::translate(identityMatrix, glm::vec3(b, 4.5, 4.55));//-1.6

                    model = model * translateMatrix * rotateXMatrix;
                    bench11(cubeVAO, lightingShader, model);
                    b += .005;
                }
                else
                {
                    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.4, 4.5, 4.55));//-1.6

                    model = model * translateMatrix * rotateXMatrix;
                    bench11(cubeVAO, lightingShader, model);
                }

              
        }
        
        if (doorOp)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (c > 7.8)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(c, -.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                c -= .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(7.8, -.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                e = 1;
            }

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (c > 7.8)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(c, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                c -= .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(7.8, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                e = 1;
            }
        }

        else if (!doorOp)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (c < 10.8)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(c, -.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                c += .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(10.8, -.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
            }

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (c < 10.8)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(c, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
                c += .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(10.8, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench11(cubeVAO, lightingShader, model);
            }
        }
        
        if (doOn)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (f > 22)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(f, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench12(cubeVAO, lightingShader, model);
                f -= .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(22, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench12(cubeVAO, lightingShader, model);
                g = -1;
            }
        }
        else if (!doOn)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (f <26.0)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(f, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench12(cubeVAO, lightingShader, model);
                f += .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(26.0, 4.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench12(cubeVAO, lightingShader, model);
            }
        }

        if (doOp)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (f > 22)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(f, -.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench12(cubeVAO, lightingShader, model);
                f -= .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(22, -.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench12(cubeVAO, lightingShader, model);
                g = -1;
            }
        }
        else if (!doOp)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (f < 26.0)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(f, -.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench12(cubeVAO, lightingShader, model);
                f += .005;
            }
            else
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(26.0, -.5, 4.55));//-1.6

                model = model * translateMatrix * rotateXMatrix;
                bench12(cubeVAO, lightingShader, model);
            }
        }
        //gate on
            
        if (gateOn)
        {
            if (p < 180.0)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
                rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
                rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
                model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
                lightingShader.setMat4("model", model);

                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(p), glm::vec3(0.0f, 1.0f, 0.0f));
                translateMatrix = glm::translate(identityMatrix, glm::vec3(12.1, -.45, 18.45));

                model = model * translateMatrix * rotateYMatrix ;
                bench16(cubeVAO, lightingShader, model);
            p++;
            }
            else //if (p == 180.0)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
                rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
                rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
                model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
                lightingShader.setMat4("model", model);

                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                translateMatrix = glm::translate(identityMatrix, glm::vec3(8.6, -.45, 18.45));

                model = model * translateMatrix *rotateYMatrix ;
                bench16(cubeVAO, lightingShader, model);
                q = 1;
            }

        }
        else if (!gateOn)
        {
            if (p > 0.0)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
                rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
                rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
                model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
                lightingShader.setMat4("model", model);

                rotateZMatrix = glm::rotate(identityMatrix, glm::radians(p), glm::vec3(0.0f, 0.0f, 1.0f));
                translateMatrix = glm::translate(identityMatrix, glm::vec3(12.1, -.45, 18.45));

                model = model * translateMatrix * rotateYMatrix ;
                bench16(cubeVAO, lightingShader, model);
                p--;
            }
            else// if (p == 0.0)
            {
                translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
                rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
                rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
                scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
                model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
                lightingShader.setMat4("model", model);

                rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                translateMatrix = glm::translate(identityMatrix, glm::vec3(12.1, -.45, 18.45));

                model = model * translateMatrix * rotateYMatrix ;
                bench16(cubeVAO, lightingShader, model);
                s = 1;
            }
        }

        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(q), glm::vec3(1.0f, .0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(39.56, 1.4, 14.3));

        model = model * translateMatrix  * rotateYMatrix;
        bench19(cubeVAO, lightingShader, model);

        /*translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(45.56, 2.99, 10.3));

        model = model * translateMatrix * rotateYMatrix;
        bench20(cubeVAO, lightingShader, model);
        x++;*/

        if (fanOn)
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(27.56, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(27.56, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r += 1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(9.56, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(9.56, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(9.56, 4.5, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(9.56, 4.5, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.54, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.54, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.54, 4.5, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.54, 4.5, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            r += 1;
        }
        else
        {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(27.56, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(27.56, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
           // r += 1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(9.56, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(9.56, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            // r += 1;
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.54, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.56, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.54, 9.4, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            // r += 1;
            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.54, 4.5, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.56, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.54, 4.5, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(9.56, 4.5, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.56, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
            //r+=1;

            translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
            model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
            lightingShader.setMat4("model", model);

            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r + 90), glm::vec3(0.0f, 1.0f, 0.0f));
            translateMatrix = glm::translate(identityMatrix, glm::vec3(9.56, 4.5, -2.3));

            model = model * translateMatrix * glm::translate(identityMatrix, glm::vec3(0.5, 0.0, 0.0)) * rotateYMatrix * glm::translate(identityMatrix, glm::vec3(-.5, 0.0, 0.0));
            bench10(cubeVAO, lightingShader, model);
        }
        

        //sphere
        /*glm::mat4 modelForBall = identityMatrix;
        translateMatrix = glm::translate(model, glm::vec3(15, 3.36f, 5.5));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.2f, 2.2f, 2.2f));
        modelForBall = modelForBall * translateMatrix * scaleMatrix;
        ball.drawSphere(lightingShader, modelForBall);*/

        /*glm::mat4 rotArrow = glm::rotate(identityMatrix, glm::radians(angle_ball), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 modelForArrow = identityMatrix;
        translateMatrix = glm::translate(model, glm::vec3(-0.02, 0.36f, 5.0));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.01f, 0.5f));
        modelForArrow = modelForArrow * translateMatrix * rotArrow * scaleMatrix;
        drawCube(cubeVAO, lightingShader, modelForArrow, 1.0, 1.0, 1.0);*/
        
        //cube(cubeVAO, lightingShader, model);
        // also draw the lamp object(s)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        
        glBindVertexArray(lightCubeVAO);
        
        for (unsigned int i = 0; i < 2; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[0]);
            model = glm::scale(model, glm::vec3(0.3f)); // Make it a smaller cube
            ourShader.setMat4("model", model);
            ourShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
            //model = model * translateMatrix;
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[1]);
            model = glm::scale(model, glm::vec3(0.3f)); // Make it a smaller cube
            ourShader.setMat4("model", model);
            ourShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
           // model = model * translateMatrix;
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

            lightingShaderWithTexture.use();
            lightingShaderWithTexture.setVec3("viewPos", camera.Position);
            lightingShaderWithTexture.setMat4("projection", projection);
            lightingShaderWithTexture.setMat4("view", view);
            
            //2nd part of cube draw
            lightingShaderWithTexture.use();
            // point light 1
            pointlight1.setUpPointLight(lightingShaderWithTexture);
            // point light 3
            pointlight3.setUpPointLight(lightingShaderWithTexture);
            // point light 4
            //pointlight4.setUpPointLight(lightingShaderWithTexture);
            
            glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.5, -1.0, -10.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(68, .5, 33));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            
            //ttoilet
            translateMatrix = glm::translate(identityMatrix, glm::vec3(29.95, 1.5, 3.54));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5, 0.4, 1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube3.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //terom
            translateMatrix = glm::translate(identityMatrix, glm::vec3(27.95, 9.5, 3.54));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5, 0.4, 1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube8.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //sky
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-30.5, -5.4, -7.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(120.5, 80.4, 1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube13.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //classroom1
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5, 9.5, 3.54));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5, .4, 1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube4.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //classroom3
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5, 4.0, 3.54));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5, .4, 1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube6.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //classroom2
            translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, 9.5, 3.54));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5, .4, 1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube5.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //classroom4
            translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, 4.0, 3.54));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.5, .4, 1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube7.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //seesaw
            translateMatrix = glm::translate(identityMatrix, glm::vec3(38.0, -0.99 ,-4.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(8, .5, 7.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube9.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //flower1
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5, -0.7, 15.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(8, .5, .6));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube9.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //flower2
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5, -0.7, 9.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.5, .5, 5.6));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube9.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //flower3
            translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0, -0.7, 9.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.5, .5, 5.6));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube9.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //ground
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.5, -0.988, 9.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(8, .5, 5.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube10.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //grass
            translateMatrix = glm::translate(identityMatrix, glm::vec3(2.8, -.99, 12.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            //grass
            translateMatrix = glm::translate(identityMatrix, glm::vec3(1.8, -.99, 12.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            //grass
            translateMatrix = glm::translate(identityMatrix, glm::vec3(.8, -.99, 12.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //grass
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.8, -.99, 12.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //grass
            translateMatrix = glm::translate(identityMatrix, glm::vec3(.5, -.99, 10.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(1.7, -.99, 10.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, -.99, 10.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5, -.99, 10.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.5, -.99, 10.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(4.0, -.99, 13.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5, -.99, 12.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.2, -.99, 13.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(2.2, -.99, 13.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(0.2, -.99, 13.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(-.4, -.99, 13.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.04, .6, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);


            //mini treer
            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5, -.99, 12.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.08, .9, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube11.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //mini treer
            translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5, -.99, 11.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.08, .9, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube11.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //mini treer
            translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, -.99, 13.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.08, .9, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube11.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //mini treer
            translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0, -.99, 13.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.08, .9, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube11.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5, -.99, 13.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.08, .9, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube11.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //mini treel
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5, -.99, 10.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.08, .9, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube11.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //tree
            translateMatrix = glm::translate(identityMatrix, glm::vec3(50.5, -.99, 5.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 5.5, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube11.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(50.5, -.99, 10.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 5.5, .1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube11.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.5, -.99, 17.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0, 5.5, .1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube11.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //scl
            translateMatrix = glm::translate(identityMatrix, glm::vec3(11.53, 3.7, 19.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(4, .5, .1));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube12.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            //road
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-8.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(0.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(5.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(10.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(15.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(20.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(25.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(30.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(35.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(40.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(45.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(50.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(55.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(59.5, -0.99, 19.0));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(-5.5, .5, 2.5));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            //road end


            
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.51, 1.0, -1.5));
            //rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            //rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            //rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.53, 2.5, 4.0));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube1.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            

            translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.51, 6.0, -1.5));
            rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(.53, 2.5, 4.0));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube1.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(5.5, 1.0, 4.468));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.5, 2.0, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube1.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(5.5, 6.5, 4.468));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.5, 2.0, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube1.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0, 1.0, 4.468));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.5, 2.0, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube1.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0, 6.5, 4.468));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.5, 2.0, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube1.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(27.5, 6.5, 4.468));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.5, 2.0, .05));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            //modelMatrixForContainer = glm::translate(identityMatrix, glm::vec3(-0.45f, -0.4f, -2.8f));
            cube1.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
           


           /* translateMatrix = glm::translate(identityMatrix, glm::vec3(-5.5, -1.0, 4.5));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(50, .5, 10));
            modelMatrixForContainer = translateMatrix * scaleMatrix;
            cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);*/

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    //printf("%f %f %f", translate_X, translate_Y, translate_Z);
    //printf("%f %f %f", rotateAngle_X, rotateAngle_Y, rotateAngle_Z);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        if (fanOn)
            fanOn = false;
        else
            fanOn = true;
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        if (gateOn)
            gateOn = false;
        else
            gateOn = true;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (doorOn)
            doorOn = false;
        else
            doorOn = true;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        if (doorOp)
            doorOp = false;
        else
            doorOp = true;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (doOn)
            doOn = false;
        else
            doOn = true;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (doOp)
            doOp = false;
        else
            doOp = true;
    }

    /*if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (flagOn)
            flagOn = false;
        else
            flagOn = true;
    }*/

    /*if (flagOn)
        flagOn = false;
    else
        flagOn = true;*/

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (rotateAxis_X) rotateAngle_X -= 0.1;
        else if (rotateAxis_Y) rotateAngle_Y -= 0.1;
        else rotateAngle_Z -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.001;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.001;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.001;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.001;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.001;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.001;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.001;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        rotateAngle_X += 0.1;
        rotateAxis_X = 1.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rotateAngle_Y += 0.1;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 1.0;
        rotateAxis_Z = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        rotateAngle_Z += 0.1;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 1.0;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        eyeX -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        eyeZ -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        pointlight1.turnOn();
        pointlight3.turnOn();
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        pointlight1.turnOff();
        pointlight3.turnOff();

    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        pointlight1.turnDiffuseOn();
       pointlight3.turnDiffuseOn();
        spotLight.turnDiffuseOn();
        dirLight.turnDiffuseOn();

    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        
        pointlight1.turnOff();
        pointlight3.turnOff();
        dirLight.turnOff();
        spotLight.turnOff();

    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        dirLight.turnOn();

    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        dirLight.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        spotLight.turnOn();

    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        spotLight.turnOff();
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        pointlight1.turnAmbientOn();
        pointlight3.turnAmbientOn();
        spotLight.turnAmbientOn();
        dirLight.turnAmbientOn();
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        pointlight1.turnSpecularOn();
        pointlight3.turnSpecularOn();
        spotLight.turnSpecularOn();
        dirLight.turnSpecularOn();
    }


 
    


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}