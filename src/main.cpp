/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape, shape2, shape3, sphere;


double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		rot = glm::vec3(0, 0, 0);
		pos = glm::vec3(0, 0.5, -2);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 2*ftime;
		}
		else if (s == 1)
		{
			speed = -2*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -1*ftime;
		else if(d==1)
			yangle = 1*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, prog2, prog3, psky, heightshader, lightshader, lampshader;

	// Contains vertex information for OpenGL
	GLuint VAO1, VAO2, VertexArrayID;

	// Data necessary to give our box to OpenGL
    GLuint VBO1pos, VBO1text, VBO1norm;
    GLuint VBO2pos, VBO2text, VBO2norm;
    
    GLuint MeshPosID, MeshTexID, IndexBufferIDBox;

	//texture data
	GLuint Texture, Texture2, Texture3, Texture4, Texture5, Texture6, Texture7;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VAO1);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}
    
    #define MESHSIZE 100
    void init_mesh()
    {
        //generate the VAO
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &MeshPosID);
        glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
        vec3 vertices[MESHSIZE * MESHSIZE * 4];
        for(int x=0;x<MESHSIZE;x++)
            for (int z = 0; z < MESHSIZE; z++)
                {
                vertices[x * 4 + z*MESHSIZE * 4 + 0] = vec3(0.0, 0.0, 0.0) + vec3(x, 0, z);
                vertices[x * 4 + z*MESHSIZE * 4 + 1] = vec3(1.0, 0.0, 0.0) + vec3(x, 0, z);
                vertices[x * 4 + z*MESHSIZE * 4 + 2] = vec3(1.0, 0.0, 1.0) + vec3(x, 0, z);
                vertices[x * 4 + z*MESHSIZE * 4 + 3] = vec3(0.0, 0.0, 1.0) + vec3(x, 0, z);
                }
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * MESHSIZE * MESHSIZE * 4, vertices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        //tex coords
        float t = 1. / 100;
        vec2 tex[MESHSIZE * MESHSIZE * 4];
        for (int x = 0; x<MESHSIZE; x++)
            for (int y = 0; y < MESHSIZE; y++)
            {
                tex[x * 4 + y*MESHSIZE * 4 + 0] = vec2(0.0, 0.0)+ vec2(x, y)*t;
                tex[x * 4 + y*MESHSIZE * 4 + 1] = vec2(t, 0.0)+ vec2(x, y)*t;
                tex[x * 4 + y*MESHSIZE * 4 + 2] = vec2(t, t)+ vec2(x, y)*t;
                tex[x * 4 + y*MESHSIZE * 4 + 3] = vec2(0.0, t)+ vec2(x, y)*t;
            }
        glGenBuffers(1, &MeshTexID);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, MeshTexID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * MESHSIZE * MESHSIZE * 4, tex, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glGenBuffers(1, &IndexBufferIDBox);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
        GLushort elements[MESHSIZE * MESHSIZE * 6];
        int ind = 0;
        for (int i = 0; i<MESHSIZE * MESHSIZE * 6; i+=6, ind+=4)
            {
            elements[i + 0] = ind + 0;
            elements[i + 1] = ind + 1;
            elements[i + 2] = ind + 2;
            elements[i + 3] = ind + 0;
            elements[i + 4] = ind + 2;
            elements[i + 5] = ind + 3;
            }
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*MESHSIZE * MESHSIZE * 6, elements, GL_STATIC_DRAW);
        glBindVertexArray(0);
    }
    int numVert;
    
	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
        init_mesh();
        
		string resourceDirectory = "../../resources" ;
		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/MapleTreeLeaves.obj");
        //shape->loadMesh(resourceDirectory + "/alduin.obj");
        //shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();
        
        shape2 = make_shared<Shape>();
        shape2->loadMesh(resourceDirectory + "/MapleTreeStem.obj");
        //shape->loadMesh(resourceDirectory + "/alduin.obj");
        //shape->loadMesh(resourceDirectory + "/sphere.obj");
        shape2->resize();
        shape2->init();
        
        shape3 = make_shared<Shape>();
        shape3->loadMesh(resourceDirectory + "/slender.obj");
        //shape->loadMesh(resourceDirectory + "/alduin.obj");
        //shape->loadMesh(resourceDirectory + "/sphere.obj");
        shape3->resize();
        shape3->init();
        
        sphere = make_shared<Shape>();
        sphere->loadMesh(resourceDirectory + "/sphere.obj");
        sphere->resize();
        sphere->init();
        
        //*************textures************************

		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/maple_leaf.png";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture 2
		str = resourceDirectory + "/maple_leaf_Mask.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
        //texture 3
        str = resourceDirectory + "/maple_bark.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture3);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Texture3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //texture 4
        str = resourceDirectory + "/slender_clothing.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture4);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, Texture4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //texture 5
        str = resourceDirectory + "/slender_skin.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture5);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, Texture5);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //texture 6
        str = resourceDirectory + "/nsky.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture6);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, Texture6);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //texture 7
        str = resourceDirectory + "/grass.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture7);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, Texture7);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");
		GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
        
        GLuint Tex3Location = glGetUniformLocation(prog2->pid, "tex3");
        
        GLuint Tex4Location = glGetUniformLocation(prog3->pid, "tex4");
        GLuint Tex5Location = glGetUniformLocation(prog3->pid, "tex5");
        
        GLuint Tex6Location = glGetUniformLocation(psky->pid, "tex6");
        GLuint Tex7Location = glGetUniformLocation(heightshader->pid, "tex7");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);
        
        glUseProgram(prog2->pid);
        glUniform1i(Tex3Location, 2);
        
        glUseProgram(prog3->pid);
        glUniform1i(Tex4Location, 3);
        glUniform1i(Tex5Location, 4);
        
        glUseProgram(psky->pid);
        glUniform1i(Tex6Location, 5);
        
        glUseProgram(heightshader->pid);
        glUniform1i(Tex7Location, 6);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
        //leaves
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("campos");
        prog->addUniform("camoff");
        prog->addUniform("treeoff");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");
        
        //trunk
        prog2 = std::make_shared<Program>();
        prog2->setVerbose(true);
        prog2->setShaderNames(resourceDirectory + "/shader_vertex2.glsl", resourceDirectory + "/shader_fragment2.glsl");
        if (!prog2->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog2->addUniform("P");
        prog2->addUniform("V");
        prog2->addUniform("M");
        prog2->addUniform("campos");
        prog2->addUniform("camoff");
        prog2->addUniform("treeoff");
        prog2->addAttribute("vertPos");
        prog2->addAttribute("vertNor");
        prog2->addAttribute("vertTex");
        
        //slenderman
        prog3 = std::make_shared<Program>();
        prog3->setVerbose(true);
        prog3->setShaderNames(resourceDirectory + "/shader_vertex3.glsl", resourceDirectory + "/shader_fragment3.glsl");
        if (!prog3->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog3->addUniform("P");
        prog3->addUniform("V");
        prog3->addUniform("M");
        prog3->addUniform("campos");
        prog3->addAttribute("vertPos");
        prog3->addAttribute("vertNor");
        prog3->addAttribute("vertTex");
        
        //skybox
        psky = std::make_shared<Program>();
        psky->setVerbose(true);
        psky->setShaderNames(resourceDirectory + "/sky_vertex.glsl", resourceDirectory + "/sky_fragment.glsl");
        if (!psky->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        psky->addUniform("P");
        psky->addUniform("V");
        psky->addUniform("M");
        psky->addUniform("campos");
        psky->addAttribute("vertPos");
        psky->addAttribute("vertNor");
        psky->addAttribute("vertTex");
        
        //terrain
        heightshader = std::make_shared<Program>();
        heightshader->setVerbose(true);
        heightshader->setShaderNames(resourceDirectory + "/height_vertex.glsl", resourceDirectory + "/height_frag.glsl");
        if (!heightshader->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        heightshader->addUniform("P");
        heightshader->addUniform("V");
        heightshader->addUniform("M");
        heightshader->addUniform("camoff");
        heightshader->addUniform("campos");
        heightshader->addAttribute("vertPos");
        heightshader->addAttribute("vertTex");
	}

    float hash(float n) { return fract(sin(n) * 753.5453123); }
    
    float snoise(vec3 x)
    {
        vec3 p = floor(x);
        vec3 f = fract(x);
        f = f * f * (3.0f - 2.0f * f);

        float n = p.x + p.y * 157.0 + 113.0 * p.z;
        return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
        mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
        mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
            mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
    }
//Changing octaves, frequency and presistance results in a total different landscape.
    float noise(vec3 position, int octaves, float frequency, float persistence) {
        float total = 0.0;
        float maxAmplitude = 0.0;
        float amplitude = 1.0;
        for (int i = 0; i < octaves; i++) {
            total += snoise(position * frequency) * amplitude;
            frequency *= 2.0;
            maxAmplitude += amplitude;
            amplitude *= persistence;
        }
        return total / maxAmplitude;
    }

	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		double frametime = get_last_elapsed_time();

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		//float aspect = width/(float)height;
		glViewport(0, 0, width, height);

        glClearColor(0.3, 0.3, 0.3, 1);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
		static float w = -3.1415926/2.0;
		//w += 1.0 * frametime;//rotation angle
		glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
		float angle = 0;
		glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
        
        
        float sangle = 3.1415926 / 2.;
            glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
            glm::vec3 camp = -mycam.pos;
            glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
            glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

            M = TransSky * RotateXSky * SSky;

            // Draw the box using GLSL.
            psky->bind();

            
            //send the matrices to the shaders
            glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
            glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
            glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            glUniform3fv(psky->getUniform("campos"), 1, &mycam.pos[0]);

            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, Texture6);
            glDisable(GL_DEPTH_TEST);
            sphere->draw(psky, false);
            glEnable(GL_DEPTH_TEST);
        
            psky->unbind();
		
		// Draw the box using GLSL.
        
        vec3 offset = mycam.pos;
        offset.y = 0;
        offset.x = abs((int)offset.x/5);
        offset.z = abs((int)offset.z/5);
        
        for (int i = -4 - offset.x; i < 4 + offset.x; i++)
        {
            for (int j = -4 - offset.z; j < 4 + offset.z; j++) {
            //leaves
            prog->bind();
            mat4 Tl = translate(mat4(1.0f), vec3(0, 3.3, -1));
            mat4 Sl = scale(mat4(1.0f), vec3(5, 4.9, 5));
        
            vec3 posT = vec3(1 + (12 * i), 1.0, 1 + (12 * j));
                
            mat4 T = translate(mat4(1.0f), posT);
        
            //send the matrices to the shaders
            mat4 Ml =  T * RotateY * RotateX;
            M = Tl * Ml * Sl;
            glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
            glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
                
            float height = noise(posT, 11, 0.3, 0.6);
            float baseheight = noise(posT, 4, 0.004, 0.3);
            baseheight = pow(baseheight, 5) * 3;
            height*=30;
                
            //use height as an angle, trajectory
            glUniform1f(prog->getUniform("treeoff"), height);
            glUniform3fv(prog->getUniform("camoff"), 1, &offset[0]);
            glUniform3fv(prog->getUniform("campos"), 1, &mycam.pos[0]);
   
            shape->draw(prog,false);
            prog->unbind();
                
                
            //trunk
            prog2->bind();
            glm::mat4 St = glm::scale(glm::mat4(1.0f), glm::vec3(5, 5.1, 5));
            //St = (height / 5) * St;
            mat4 Tt = translate(mat4(1.0f),vec3(-1, 1.1, 0));
            //send the matrices to the shaders
            M =  Ml * Tt * St;
            glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, &P[0][0]);
            glUniformMatrix4fv(prog2->getUniform("V"), 1, GL_FALSE, &V[0][0]);
            glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE, &M[0][0]);
     
            //pass in same height to trunk
            glUniform1f(prog2->getUniform("treeoff"), height);
            glUniform3fv(prog2->getUniform("camoff"), 1, &offset[0]);
            glUniform3fv(prog2->getUniform("campos"), 1, &mycam.pos[0]);
         
            shape2->draw(prog2,false);
            prog2->unbind();
            }
        }
        
        //slenderman
        mat4 Ts = translate(mat4(1.0f), vec3(2, -1, -2));
        mat4 Ss = scale(mat4(1.0f), vec3(1, 1, 1));
        prog3->bind();
         //send the matrices to the shaders
        M =  Ts * Ss;
        glUniformMatrix4fv(prog3->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog3->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog3->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniform3fv(prog3->getUniform("campos"), 1, &mycam.pos[0]);
         
        shape3->draw(prog3,false);
        prog3->unbind();
        
        //ground
        heightshader->bind();
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 TransY = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -3.0f, -50));
        M = TransY;
        glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniformMatrix4fv(heightshader->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(heightshader->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        
        
        offset = mycam.pos;
        offset.y = 0;
        offset.x = (int)offset.x;
        offset.z = (int)offset.z;
        glUniform3fv(heightshader->getUniform("camoff"), 1, &offset[0]);
        glUniform3fv(heightshader->getUniform("campos"), 1, &mycam.pos[0]);
        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
        
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, Texture7);
        
        glDrawElements(GL_TRIANGLES, MESHSIZE*MESHSIZE*6, GL_UNSIGNED_SHORT, (void*)0);

        heightshader->unbind();

	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
