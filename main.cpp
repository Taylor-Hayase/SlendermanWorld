/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <algorithm>
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


#define SHADOW_DIM 4096

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
		pos = glm::vec3(0, 0.5, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 5*ftime;
		}
		else if (s == 1)
		{
			speed = -5*ftime;
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
	std::shared_ptr<Program> prog, prog2, prog3, psky, heightshader, prog_framebuffer;
    std::shared_ptr<Program> progS, prog2S, prog3S, heightshaderS;

	// Contains vertex information for OpenGL
	GLuint VAO1, VAO2, VertexArrayID;

	// Data necessary to give our box to OpenGL
    GLuint FBOtex, FrameBufferObj, depth_rb;
    GLuint FBOtex_shadowMapDepth, fb_shadowMap, fb_shadowMapDepth;
    GLuint VertexArrayIDRect, VertexBufferIDRect, VertexBufferTexRect;
    
    GLuint VBO1pos, VBO1text, VBO1norm;
    GLuint VBO2pos, VBO2text, VBO2norm;
    
    GLuint MeshPosID, MeshTexID, IndexBufferIDBox;
	//mat4 V_shadowmap;
	//texture data
    GLuint LeavesTex, TrunkTex, SlendyTex, SkyTex, GrassTex;
    
    bool show_slendy = true;
    bool jumpscare = false;
    int slendyZN, slendyZP, slendyXN, slendyXP;
    
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
        if (key == GLFW_KEY_P && action == GLFW_RELEASE)
        {
            show_slendy = !show_slendy;
        }
        if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
        {
            jumpscare = !jumpscare;
        }
        if (key == GLFW_KEY_I && action == GLFW_PRESS)
        {
            slendyZN = 1;
        }
        if (key == GLFW_KEY_I && action == GLFW_RELEASE)
        {
            slendyZN = 0;
        }
        if (key == GLFW_KEY_K && action == GLFW_PRESS)
        {
            slendyZP = 1;
        }
        if (key == GLFW_KEY_K && action == GLFW_RELEASE)
        {
            slendyZP = 0;
        }
        if (key == GLFW_KEY_J && action == GLFW_PRESS)
        {
            slendyXN = 1;
        }
        if (key == GLFW_KEY_J && action == GLFW_RELEASE)
        {
            slendyXN = 0;
        }
        if (key == GLFW_KEY_L && action == GLFW_PRESS)
        {
            slendyXP = 1;
        }
        if (key == GLFW_KEY_L && action == GLFW_RELEASE)
        {
            slendyXP = 0;
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
        glGenVertexArrays(1, &VertexArrayIDRect);
        glBindVertexArray(VertexArrayIDRect);

        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &VertexBufferIDRect);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDRect);

        GLfloat *ver = new GLfloat[18];
        // front
        int verc = 0;

        ver[verc++] = -1.0, ver[verc++] = -1.0, ver[verc++] = 0.0;
        ver[verc++] = 1.0, ver[verc++] = -1.0, ver[verc++] = 0.0;
        ver[verc++] = -1.0, ver[verc++] = 1.0, ver[verc++] = 0.0;
        ver[verc++] = 1.0, ver[verc++] = -1.0, ver[verc++] = 0.0;
        ver[verc++] = 1.0, ver[verc++] = 1.0, ver[verc++] = 0.0;
        ver[verc++] = -1.0, ver[verc++] = 1.0, ver[verc++] = 0.0;


        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), ver, GL_STATIC_DRAW);
        //we need to set up the vertex array
        glEnableVertexAttribArray(0);
        //key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &VertexBufferTexRect);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferTexRect);
       // float t = 1. / 100.;
        GLfloat *cube_tex = new GLfloat[12];
        int texc = 0;

        cube_tex[texc++] = 0, cube_tex[texc++] = 0;
        cube_tex[texc++] = 1, cube_tex[texc++] = 0;
        cube_tex[texc++] = 0, cube_tex[texc++] = 1;
        cube_tex[texc++] = 1, cube_tex[texc++] = 0;
        cube_tex[texc++] = 1, cube_tex[texc++] = 1;
        cube_tex[texc++] = 0, cube_tex[texc++] = 1;

        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), cube_tex, GL_STATIC_DRAW);
        //we need to set up the vertex array
        glEnableVertexAttribArray(1);
        //key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        

        
        init_mesh();
        
		string resourceDirectory = "../../resources" ;
		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/MapleTreeLeaves.obj");
		shape->resize();
		shape->init();
        
        shape2 = make_shared<Shape>();
        shape2->loadMesh(resourceDirectory + "/MapleTreeStem.obj");
        shape2->resize();
        shape2->init();
        
        shape3 = make_shared<Shape>();
        shape3->loadMesh(resourceDirectory + "/slender.obj");
        shape3->resize();
        shape3->init();
        
        sphere = make_shared<Shape>();
        sphere->loadMesh(resourceDirectory + "/sphere.obj");
        sphere->resize();
        sphere->init();
        
        {
            int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        //attach shadow map depth texture to shadow map fbo
        glGenFramebuffers(1, &fb_shadowMap);
        glBindFramebuffer(GL_FRAMEBUFFER, fb_shadowMap);

        glGenTextures(1, &FBOtex_shadowMapDepth);
        glBindTexture(GL_TEXTURE_2D, FBOtex_shadowMapDepth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_DIM, SHADOW_DIM, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(vec3(1.0)));
            

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, FBOtex_shadowMapDepth, 0);
            
        GLenum status;
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (status)
        {
        case GL_FRAMEBUFFER_COMPLETE:
            cout << "status shadow framebuffer: good" << std::endl;
            break;
        default:
            cout << "status shadow framebuffer: bad!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        //*************textures************************

		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/maple_leaf.png";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &LeavesTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, LeavesTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
        //texture 2
        str = resourceDirectory + "/maple_bark.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &TrunkTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TrunkTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //texture 3
        str = resourceDirectory + "/slender_clothing.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &SlendyTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SlendyTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //texture 4
        str = resourceDirectory + "/nsky.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &SkyTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SkyTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //texture 5
        str = resourceDirectory + "/grass.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &GrassTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GrassTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

		//set the textures to the correct samplers in the fragment shader:
		GLuint TexLeavesLocation = glGetUniformLocation(prog->pid, "texLeaves");
		//GLuint TexShadLocation = glGetUniformLocation(prog->pid, "shadowMapTex");
        GLuint TexTrunkLocation = glGetUniformLocation(prog2->pid, "texTrunk");
        
        GLuint TexSlendyLocation = glGetUniformLocation(prog3->pid, "texSlendy");
        
        GLuint TexSkyLocation = glGetUniformLocation(psky->pid, "texSky");
        
        GLuint TexGrassLocation = glGetUniformLocation(heightshader->pid, "texGrass");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog->pid);
		glUniform1i(TexLeavesLocation, 0);
        //glUniform1i(ShadowTexLocation, 1);
        
        glUseProgram(prog2->pid);
        glUniform1i(TexTrunkLocation, 0);
        
        glUseProgram(prog3->pid);
        glUniform1i(TexSlendyLocation, 0);
        
        glUseProgram(psky->pid);
        glUniform1i(TexSkyLocation, 0);
        
        glUseProgram(heightshader->pid);
        glUniform1i(TexGrassLocation, 0);
        
        GLuint TexShadLocation = glGetUniformLocation(heightshader->pid, "shadowMapTex");
        glUniform1i(TexShadLocation, 1);

        GLuint TexLocation = glGetUniformLocation(prog_framebuffer->pid, "tex"); //tex sampler in the fragment shader
        GLuint ShadowTexLocation = glGetUniformLocation(prog_framebuffer->pid, "shadowMapTex");
        glUseProgram(prog_framebuffer->pid);
        glUniform1i(TexLocation, 0);
        glUniform1i(ShadowTexLocation, 1);
    
        
        //Frame Buffer Object
        //init screen texture fbo
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        //RGBA8 2D texture, 24 bit depth texture, 256x256
        glGenTextures(1, &FBOtex);
        glBindTexture(GL_TEXTURE_2D, FBOtex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //NULL means reserve texture memory, but texels are undefined
        //**** Tell OpenGL to reserve level 0
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
        
        //You must reserve memory for other mipmaps levels as well either by making a series of calls to
        //glTexImage2D or use glGenerateMipmapEXT(GL_TEXTURE_2D).
        //Here, we'll use :
        glGenerateMipmap(GL_TEXTURE_2D);
        //-------------------------
        glGenFramebuffers(1, &FrameBufferObj);
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObj);
        //Attach 2D texture to this FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOtex, 0);
        //-------------------------
        glGenRenderbuffers(1, &depth_rb);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        //-------------------------
        //Attach depth buffer to FBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
        //-------------------------
        //Does the GPU support current FBO configuration?
        GLenum status;
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (status)
            {
            case GL_FRAMEBUFFER_COMPLETE:
                    cout << "status framebuffer: good" <<endl;
                break;
            default:
                    cout << "status framebuffer: bad!!!!!!!!!!!!!!!!!!!!!!!!!" <<endl;
            }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        
        GLuint Tex1Location = glGetUniformLocation(prog_framebuffer->pid, "tex"); //
        glUseProgram(prog_framebuffer->pid);//
        glUniform1i(Tex1Location, 0);//

	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
        
        //culling
      //  glEnable(GL_CULL_FACE);
       // glFrontFace(GL_CCW);
      
        //transparancy
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Initialize the GLSL program.
        //leaves
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/leaves_vertex.glsl", resourceDirectory + "/leaves_fragment.glsl");
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
        prog->addUniform("PVl");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");
        
        //leaves shadow map
        progS = std::make_shared<Program>();
        progS->setVerbose(true);
        progS->setShaderNames(resourceDirectory + "/tree_vertexS.glsl", resourceDirectory + "/S_frag.glsl");
        if (!progS->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        progS->addUniform("P");
        progS->addUniform("V");
        progS->addUniform("M");
        progS->addUniform("camoff");
        progS->addUniform("treeoff");
        progS->addAttribute("vertPos");
        progS->addAttribute("vertNor");
        progS->addAttribute("vertTex");
        
        //trunk
        prog2 = std::make_shared<Program>();
        prog2->setVerbose(true);
        prog2->setShaderNames(resourceDirectory + "/trunk_vertex.glsl", resourceDirectory + "/trunk_fragment.glsl");
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
        prog2->addUniform("PVl");
        prog2->addAttribute("vertPos");
        prog2->addAttribute("vertNor");
        prog2->addAttribute("vertTex");
        
        //trunk shadow
        prog2S = std::make_shared<Program>();
        prog2S->setVerbose(true);
        prog2S->setShaderNames(resourceDirectory + "/tree_vertexS.glsl", resourceDirectory + "/S_frag.glsl");
        if (!prog2S->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog2S->addUniform("P");
        prog2S->addUniform("V");
        prog2S->addUniform("M");
        prog2S->addUniform("camoff");
        prog2S->addUniform("treeoff");
        prog2S->addAttribute("vertPos");
        prog2S->addAttribute("vertNor");
        prog2S->addAttribute("vertTex");
        
        //slenderman
        prog3 = std::make_shared<Program>();
        prog3->setVerbose(true);
        prog3->setShaderNames(resourceDirectory + "/slender_vertex.glsl", resourceDirectory + "/slender_fragment.glsl");
        if (!prog3->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog3->addUniform("P");
        prog3->addUniform("V");
        prog3->addUniform("M");
        prog3->addUniform("PVl");
        prog3->addUniform("campos");
        prog3->addAttribute("vertPos");
        prog3->addAttribute("vertNor");
        prog3->addAttribute("vertTex");
        
        //slender shadow
        prog3S = std::make_shared<Program>();
        prog3S->setVerbose(true);
        prog3S->setShaderNames(resourceDirectory + "/slender_vertexS.glsl", resourceDirectory + "/S_frag.glsl");
        if (!prog3S->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog3S->addUniform("P");
        prog3S->addUniform("V");
        prog3S->addUniform("M");
        prog3S->addAttribute("vertPos");
        prog3S->addAttribute("vertNor");
        prog3S->addAttribute("vertTex");
        
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
        heightshader->addUniform("PVl");
        heightshader->addUniform("camoff");
        heightshader->addUniform("campos");
        heightshader->addAttribute("vertPos");
        heightshader->addAttribute("vertTex");
        
        //ground shadow
        heightshaderS = std::make_shared<Program>();
        heightshaderS->setVerbose(true);
        heightshaderS->setShaderNames(resourceDirectory + "/height_vertexS.glsl", resourceDirectory + "/S_frag.glsl");
        if (!heightshaderS->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        heightshaderS->addUniform("P");
        heightshaderS->addUniform("V");
        heightshaderS->addUniform("M");
        heightshaderS->addUniform("camoff");
        heightshaderS->addAttribute("vertPos");
        heightshaderS->addAttribute("vertTex");
        
        //framebuffer
        prog_framebuffer = make_shared<Program>();
        prog_framebuffer->setVerbose(true);
        prog_framebuffer->setShaderNames(resourceDirectory + "/vertFB.glsl", resourceDirectory + "/fragFB.glsl");
        if (!prog_framebuffer->init())
            {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
            }
        prog_framebuffer->init();
        prog_framebuffer->addAttribute("vertPos");
        prog_framebuffer->addAttribute("vertTex");
        
        
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
    void render() //to screen
    {
        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        glViewport(0, 0, width, height);

        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prog_framebuffer->bind();
        
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D,  FBOtex_shadowMapDepth)
        glBindTexture(GL_TEXTURE_2D, FBOtex);
        
        glBindVertexArray(VertexArrayIDRect);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        prog_framebuffer->unbind();

    }
    
    void get_light_proj_matrix(glm::mat4& lightP)
        {
        // If your scene goes outside these "bounds" (e.g. shadows stop working near boundary),
        // feel free to increase these numbers (or decrease if scene shrinks/light gets closer to
        // scene objects).
        const float left = -15.0f;
        const float right = 15.0f;
        const float bottom = -15.0f;
        const float top = 15.0f;
        const float zNear = 0.5f;
        const float zFar = 50.0f;

        lightP = glm::ortho(left, right, bottom, top, zNear, zFar);
        lightP = glm::perspective((float)(3.14159 / 4.), (float)((float)SHADOW_DIM / (float)SHADOW_DIM), 1.0f, 30.0f); //so much type casting... GLM metods are quite funny ones

        }
    void get_light_view_matrix(glm::mat4& lightV,mat4 V)
        {
        // Change earth_pos (or primaryLight.direction) to change where the light is pointing at.
        lightV = V * translate(mat4(1),vec3(-0.2,0.2,0));
        }
//-----------------------------------------------------------------------------------
    float moveZ = -25;
    float moveX = 0;
    int timer = 0;
    
	void render_to_framebuffer(float frametime, mat4 V)
	{
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObj);

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		//float aspect = width/(float)height;
		glViewport(0, 0, width, height);

        glClearColor(0.3, 0.3, 0.3, 1);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 M, P; //View, Model and Perspective matrix
		
       
        mat4 Pl, Vl = V;
        get_light_view_matrix(Vl, V);
        get_light_proj_matrix(Pl);
        mat4 PVl = Pl * Vl;
        
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
	//	static float w = -3.1415926/2.0;

        float sangle = 3.1415926 / 2.;
        glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::vec3 camp = -mycam.pos;
        glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
        glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

        M = TransSky * RotateXSky * SSky;

        // Draw skybox
        psky->bind();

        //send the matrices to the shaders
        glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniform3fv(psky->getUniform("campos"), 1, &mycam.pos[0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SkyTex);
        
        glDisable(GL_DEPTH_TEST);
        sphere->draw(psky, false);
        glEnable(GL_DEPTH_TEST);
        
        psky->unbind();
		
		// Draw trees
        
        vec3 offset = mycam.pos;
        offset.y = 0;
        offset.x = (int)offset.x;
        offset.z = (int)offset.z;
        
        int z = -(int)(mycam.pos.z/10);
        int x = -(int)(mycam.pos.x/10);
        
		for (int i = -5 + x; i < 5 + x; i++)
			{
			for (int j = -5 + z; j < 5 + z; j++)
				{
                //draw tree leaves
                prog->bind();
                mat4 Tl = translate(mat4(1.0f), vec3(0, 3.3, -0.5));
                mat4 Sl = scale(mat4(1.0f), vec3(5, 4.9, 5));
        
                vec3 posT = vec3(1 + (12 * i), 1.0, 1 + (12 * j));
                
                mat4 T = translate(mat4(1.0f), posT);
        
                //send the matrices to the shaders
                mat4 Ml = T;
                M = Tl * Ml * Sl;
           
                glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
                glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
                glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
                glUniformMatrix4fv(prog->getUniform("PVl"), 1, GL_FALSE, &PVl[0][0]);
                
                float height = noise(posT, 11, 0.3, 0.6);
                float baseheight = noise(posT, 4, 0.004, 0.3);
                baseheight = pow(baseheight, 5) * 3;
                height*=20;
                
                //use height as an angle, trajectory
                glUniform1f(prog->getUniform("treeoff"), height);
                glUniform3fv(prog->getUniform("camoff"), 1, &offset[0]);
                glUniform3fv(prog->getUniform("campos"), 1, &mycam.pos[0]);
                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, LeavesTex);

                shape->draw(prog,false);
                prog->unbind();
                
                //draw tree trunk
                prog2->bind();
                glm::mat4 St = glm::scale(glm::mat4(1.0f), glm::vec3(5, 5.1, 5));
                mat4 Tt = translate(mat4(1.0f),vec3(0., 1.1, 0));
                //send the matrices to the shaders
                M =  Ml * Tt * St;
          
                glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, &P[0][0]);
                glUniformMatrix4fv(prog2->getUniform("V"), 1, GL_FALSE, &V[0][0]);
                glUniformMatrix4fv(prog2->getUniform("PVl"), 1, GL_FALSE, &PVl[0][0]);
                glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE, &M[0][0]);
     
                //pass in same height to trunk
                glUniform1f(prog2->getUniform("treeoff"), height);
                glUniform3fv(prog2->getUniform("camoff"), 1, &offset[0]);
                glUniform3fv(prog2->getUniform("campos"), 1, &mycam.pos[0]);
         
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, TrunkTex);
  
                shape2->draw(prog2,false);
                prog2->unbind();
            }
        }
        
        //slenderman movements
        if (slendyZN == 1) {
            moveZ -= 0.5;
        }
        if (slendyZP == 1) {
            moveZ += 0.5;
        }
        if (slendyXN == 1) {
            moveX -= 0.5;
        }
        if (slendyXP == 1) {
            moveX += 0.5;
        }
        
        //slenderman jumpscare routine
        if (jumpscare) {
            if (timer == 0) {
                show_slendy = false;
            }
            moveX = -mycam.pos.x;
            timer += 1;
            if (timer == 5) {
                moveZ = (-mycam.pos.z) - 25;
                show_slendy = true;
            }
            else if (timer == 20) {
                show_slendy = false;
            }
            else if (timer == 30) {
                moveZ = (-mycam.pos.z) - 10;
                show_slendy = true;
            }
            else if (timer == 45) {
                show_slendy = false;
            }
            else if (timer == 65) {
                moveZ = (-mycam.pos.z) - 1.3;
                show_slendy = true;
            }
            else if (timer > 65) {
                jumpscare = !jumpscare;
                timer = 0;
            }
        }
        
        //draw slenderman
        mat4 Ts = translate(mat4(1.0f), vec3(moveX, -1.5, moveZ));
        mat4 Ss = scale(mat4(1.0f), vec3(1.3, 1.5, 1.3));
        prog3->bind();
        //send the matrices to the shaders
        M =  Ts * Ss;
        glUniformMatrix4fv(prog3->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog3->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog3->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        
        glUniformMatrix4fv(prog3->getUniform("PVl"), 1, GL_FALSE, &PVl[0][0]);
        glUniform3fv(prog3->getUniform("campos"), 1, &mycam.pos[0]);
         
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SlendyTex);

        if (show_slendy){
            shape3->draw(prog3,false);
        }
        prog3->unbind();
        
        //draw ground
        heightshader->bind();
        glm::mat4 TransY = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -3.0f, -50));
        M = TransY;
        glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniformMatrix4fv(heightshader->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(heightshader->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(heightshader->getUniform("PVl"), 1, GL_FALSE, &PVl[0][0]);
        
        offset = mycam.pos;
        offset.y = 0;
        offset.x = (int)offset.x;
        offset.z = (int)offset.z;
        glUniform3fv(heightshader->getUniform("camoff"), 1, &offset[0]);
        glUniform3fv(heightshader->getUniform("campos"), 1, &mycam.pos[0]);
        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
        
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GrassTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, FBOtex_shadowMapDepth);
        
        glDrawElements(GL_TRIANGLES, MESHSIZE*MESHSIZE*6, GL_UNSIGNED_SHORT, (void*)0);

        heightshader->unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, FBOtex);
        glGenerateMipmap(GL_TEXTURE_2D);
	}
    
//###################################################################################
    void render_to_shadowmap(double frametime, mat4 V)
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        
        glBindFramebuffer(GL_FRAMEBUFFER, fb_shadowMap);
        glClearColor(0.0, 0.0, 0.0, 1.0);
           glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SHADOW_DIM, SHADOW_DIM);

        glDisable(GL_BLEND);
    
     
        glm::mat4 M, P; //View, Model and Perspective matrix
        
        M = glm::mat4(1);
        
        get_light_proj_matrix(P);
        get_light_view_matrix(V, V);

        // Draw trees
        vec3 offset = mycam.pos;
        offset.y = 0;
        offset.x = (int)offset.x;
        offset.z = (int)offset.z;
        
        int z = -(int)(mycam.pos.z/10);
        int x = -(int)(mycam.pos.x/10);

        for (int i = -5 + x; i < 5 + x; i++)
        {
            for (int j = -5 + z; j < 5 + z; j++)
            {
                //leaves
                progS->bind();
                mat4 Tl = translate(mat4(1.0f), vec3(0, 3.3, -1));
                mat4 Sl = scale(mat4(1.0f), vec3(5, 4.9, 5));
             
                vec3 posT = vec3(1 + (12 * i), 1.0, 1 + (12 * j));
                     
                mat4 T = translate(mat4(1.0f), posT);
             
                //send the matrices to the shaders
                mat4 Ml =  T;
                M = Tl * Ml * Sl;
                glUniformMatrix4fv(progS->getUniform("P"), 1, GL_FALSE, &P[0][0]);
                glUniformMatrix4fv(progS->getUniform("V"), 1, GL_FALSE, &V[0][0]);
                glUniformMatrix4fv(progS->getUniform("M"), 1, GL_FALSE, &M[0][0]);
                     
                float height = noise(posT, 11, 0.3, 0.6);
                float baseheight = noise(posT, 4, 0.004, 0.3);
                baseheight = pow(baseheight, 5) * 3;
                height*=20;
                     
                //use height as an angle, trajectory
                glUniform1f(progS->getUniform("treeoff"), height);
                glUniform3fv(progS->getUniform("camoff"), 1, &offset[0]);
        
                shape->draw(progS,false);
                progS->unbind();
                     
                //trunk
                prog2S->bind();
                glm::mat4 St = glm::scale(glm::mat4(1.0f), glm::vec3(5, 5.1, 5));
                mat4 Tt = translate(mat4(1.0f),vec3(0., 1.1, 0));
                //send the matrices to the shaders
                M =  Ml * Tt * St;
                glUniformMatrix4fv(prog2S->getUniform("P"), 1, GL_FALSE, &P[0][0]);
                glUniformMatrix4fv(prog2S->getUniform("V"), 1, GL_FALSE, &V[0][0]);
                glUniformMatrix4fv(prog2S->getUniform("M"), 1, GL_FALSE, &M[0][0]);
          
                //pass in same height to trunk
                glUniform1f(prog2S->getUniform("treeoff"), height);
                glUniform3fv(prog2S->getUniform("camoff"), 1, &offset[0]);
              
                shape2->draw(prog2S,false);
                prog2S->unbind();
            }
        }
        
        //slenderman
        mat4 Ts = translate(mat4(1.0f), vec3(moveX, -1.5, moveZ));
        mat4 Ss = scale(mat4(1.0f), vec3(1.3, 1.5, 1.3));
        prog3S->bind();
        //send the matrices to the shaders
        M =  Ts * Ss;
        glUniformMatrix4fv(prog3S->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog3S->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog3S->getUniform("M"), 1, GL_FALSE, &M[0][0]);
              
        if (show_slendy)
            shape3->draw(prog3S,false);
        prog3S->unbind();
             
        //ground
        heightshaderS->bind();
        glm::mat4 TransY = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -3.0f, -50));
        M = TransY;
        glUniformMatrix4fv(heightshaderS->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniformMatrix4fv(heightshaderS->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(heightshaderS->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        
        offset = mycam.pos;
        offset.y = 0;
        offset.x = (int)offset.x;
        offset.z = (int)offset.z;
        glUniform3fv(heightshaderS->getUniform("camoff"), 1, &offset[0]);
        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
                
        glDrawElements(GL_TRIANGLES, MESHSIZE*MESHSIZE*6, GL_UNSIGNED_SHORT, (void*)0);

        heightshaderS->unbind();

        glEnable(GL_BLEND);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, FBOtex_shadowMapDepth);
        glGenerateMipmap(GL_TEXTURE_2D);
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
		double frametime = get_last_elapsed_time();
		mat4 V = mycam.process(frametime);
		// Render scene.
        application->render_to_shadowmap(frametime, V);
        application->render_to_framebuffer(frametime,V);
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
