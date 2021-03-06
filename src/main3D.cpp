/**
 * \file main.cpp
 * \brief Programme principal - Editeur de scène 3D
 * \author Amélia MANSION - Manon SGRO'
 * \version 0.1
 * \date 20 décembre 2019
 *
 * Programme d'édition de scène 3D.
 *
 */

/*! \mainpage World_Imaker Documentation
 * \version 0.1
 * \image html logo_text.png
 * \tableofcontents
 * \section intro_sec About
 *
 * World Imaker is a 3D scene editor-visualizer made with OpenGL/C++.
 *
 * \section install_sec Installation
 *
 * \subsection tools_subsec Tools required
 * - OpenGL 3.0.0 or later: <small><a href="https://www.opengl.org/">OpenGL Website</a></small>
 * - SDL 2.0: <small><a href="https://www.libsdl.org/">SDL Website</a></small>
 *
 * \subsection running Running the program
 * - Step 1: Clone the repo from <a href="https://github.com/ManonSgro/World_Imaker.git">Github</a>.
 * - Step 2: Create the build directory if doesn't exist. Then move into it.
 * - Step 3: Run CMake and build the project.
 * - Step 4: Execute the program in the <code style="font-family:monospace; font-size:0.8rem;padding:2px;background-color:#eee">bin</code> folder.
 *
 * \section authors Authors
 * - Amélia Mansion
 * - Manon Sgro'
 *
 * \section footer More infos
 * The documentation is located in <code style="font-family:monospace; font-size:0.8rem;padding:2px;background-color:#eee">[path to build]/doc/html/index.html</code>
 *
 */

// Includes
#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>
#include <glimac/Image.hpp>
#include <glimac/Cube.hpp>
#include <glimac/Texture.hpp>
#include <glimac/CubeList.hpp>
#include <glimac/Controls.hpp>
#include <glimac/objloader.hpp>
#include <glimac/text.hpp>
#include <cstddef>
#include <vector>
#include <stack>

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>
// Include GLFW
//#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

// Include imGUI
#include <include/imgui.h>
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

using namespace glm;

using namespace glimac;

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    float windowWidth = 800.0f;
    float windowHeight = 800.0f;
    float menuWidth = 200.0f;
    SDLWindowManager windowManager(windowWidth+menuWidth, windowHeight+menuWidth, "World Imaker");
    //glewExperimental = GL_TRUE;

    // Print logo
    windowManager.printSignature();

    // Initialize GLEW for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    // Print some infos about OpenGl and GLEW
    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /** INITIALIZE SCENE **/    
    // Add 3 cubes
    CubeList myCubeList;
    myCubeList.addCube(Cube());
    myCubeList.addCube(Cube());
    myCubeList.addCube(Cube());

    // Translate cubes
    myCubeList.setTrans(0, 0,0,0);
    myCubeList.setTrans(1, -1,0,0);
    myCubeList.setTrans(2, 1,0,0);

    // Link cubes with textures
    myCubeList.setTextureIndex(0, 1);
    myCubeList.setTextureIndex(1, 1);
    myCubeList.setTextureIndex(2, 1);

    // Initialize cursor (a very special cube)
    Cube cursor;
    cursor.setTrans(myCubeList.getTrans(0).x,myCubeList.getTrans(0).y,myCubeList.getTrans(0).z);
    cursor.setTextureIndex(0);


    /** INITIALIZE IMGUI */

    // Decide GL+GLSL versions
    #if __APPLE__
        // GL 3.2 Core + GLSL 150
        const char* glsl_version = "#version 150";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #endif

    // Initialize OpenGL loader
    #if defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
        bool err = glewInit() != GLEW_OK;
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
        bool err = gladLoadGL() == 0;
    #else
        bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
    #endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Platform/Renderer bindings
    SDL_GLContext gl_context = SDL_GL_CreateContext(windowManager.window);
    SDL_GL_MakeCurrent(windowManager.window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    ImGui_ImplSDL2_InitForOpenGL(windowManager.window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

     /** LOADING SHADERS **/
    FilePath applicationPath(argv[0]);
    Program program = loadProgram(
        applicationPath.dirPath() + "shaders/vertex.vs.glsl",
        applicationPath.dirPath() + "shaders/fragment.fs.glsl"
    );
    program.use();

    // Get uniform variable ID
    GLint uMVPMatrix = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    GLint uMVMatrix = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    GLint uNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
    
    //Get uniform variables for light
    GLint uKd = glGetUniformLocation(program.getGLId(), "uKd");
    GLint uKs = glGetUniformLocation(program.getGLId(), "uKs");
    GLint uShininess = glGetUniformLocation(program.getGLId(), "uShininess");
    GLint uLightPos_vs = glGetUniformLocation(program.getGLId(), "uLightPos_vs");
    GLint uLightDir_vs = glGetUniformLocation(program.getGLId(), "uLightDir_vs");
    GLint uLightIntensityP = glGetUniformLocation(program.getGLId(), "uLightIntensityP");
    GLint uLightIntensityD = glGetUniformLocation(program.getGLId(), "uLightIntensityD");
    
    /** INITIALIZE TEXTURES **/
    uint nbOfTextures = 10;
    std::vector<Texture> textures(nbOfTextures);

    for(int i=0; i<textures.size(); i++){
        char const *pchar = "uTexture" + i;
        textures[i].setUniformLocation(program, (GLchar*)pchar);
    }

    // Loading textures
    textures[0].setImage("../../World_Imaker/assets/textures/rouge.png");
    textures[1].setImage("../../World_Imaker/assets/textures/bois.png");
    textures[2].setImage("../../World_Imaker/assets/textures/brique.png");
    textures[3].setImage("../../World_Imaker/assets/textures/cailloux.png");
    textures[4].setImage("../../World_Imaker/assets/textures/eau.png");
    textures[5].setImage("../../World_Imaker/assets/textures/goudron.png");
    textures[6].setImage("../../World_Imaker/assets/textures/herbe.png");
    textures[7].setImage("../../World_Imaker/assets/textures/marbre.png");
    textures[8].setImage("../../World_Imaker/assets/textures/mosaique.png");
    textures[9].setImage("../../World_Imaker/assets/textures/sol_metalique.png");

    // Textures
    for(uint i = 0; i<textures.size(); i++){
        glGenTextures(1,textures[i].getTexturePointer());
        glBindTexture(GL_TEXTURE_2D,textures[i].getTexture());   // Texture binding
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,textures[i].getImageWidth(),textures[i].getImageHeight(),0,GL_RGBA,GL_FLOAT,textures[i].getImagePixels());

        //Filters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    /** INITIALIZE VBOs **/
    // For the cursor
    // Generate one buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, myCubeList.getVBOListItem(0));
    // Bind buffer
    glBindBuffer(GL_ARRAY_BUFFER, *myCubeList.getVBOListItem(0));
    // Send data to CG
    glBufferData(GL_ARRAY_BUFFER, cursor.getVertexCount()*sizeof(Vertex3DTexture), cursor.getDataPointer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, *myCubeList.getVBOListItem(0));

    // Repeat for all cubes of the scene (i+1 because the cursor is 0)
    myCubeList.generateVBO();

    /** INITIALIZE VAOs **/
    // Generate a VAO
    glGenVertexArrays(1, myCubeList.getVAOListItem(0));
    // VAO Binding
    glBindVertexArray(*myCubeList.getVAOListItem(0));
    glBindBuffer(GL_ARRAY_BUFFER, *myCubeList.getVAOListItem(0));
    // 1st attribute buffer : position
    glEnableVertexAttribArray(myCubeList.getVBOPositionAttribute());
    glVertexAttribPointer(myCubeList.getVBOPositionAttribute(),3,GL_FLOAT, GL_FALSE, sizeof(Vertex3DTexture), (const GLvoid*)offsetof(Vertex3DTexture, position));
    // 2nd attribute buffer : normal
    glEnableVertexAttribArray(myCubeList.getVBONormalAttribute());
    glVertexAttribPointer(myCubeList.getVBONormalAttribute(),3,GL_FLOAT, GL_FALSE, sizeof(Vertex3DTexture), (const GLvoid*)offsetof(Vertex3DTexture, normal));
    // 3rd attribute buffer : texture
    glEnableVertexAttribArray(myCubeList.getVBOTextureAttribute());
    glVertexAttribPointer(myCubeList.getVBOTextureAttribute(),2,GL_FLOAT, GL_FALSE, sizeof(Vertex3DTexture), (const GLvoid*)offsetof(Vertex3DTexture, texture));

    //Repeat for all cubes of the scene (i+1 because the cursor is 0)
    myCubeList.generateVAO();

    /** INITIALIZE IBOs **/
    // Generate buffer
    glGenBuffers(1, myCubeList.getIBOListItem(0));
    // Bind IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *myCubeList.getIBOListItem(0));
    // Send data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cursor.getIBOCountBorder()*sizeof(uint32_t), cursor.getIBOPointerBorder(), GL_STATIC_DRAW);

    //Repeat for all cubes of the scene (i+1 because the cursor is 0)
    myCubeList.generateIBO();

    // Stop binding
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /** SORT CUBES BY TEXTURE **/
    myCubeList.printCubes();
    myCubeList.sortCubes();
    myCubeList.printCubes();

    // Create and compile our GLSL program from the shaders
	//GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(program.getGLId(), "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(program.getGLId(), "V");
	GLuint ModelMatrixID = glGetUniformLocation(program.getGLId(), "M");

	// Load the texture
	GLuint Texture = loadDDS("../assets/models/uvmap.DDS");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(program.getGLId(), "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("../assets/models/suzanne.obj", vertices, uvs, normals);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    /** INITIALIZE LOOP **/
    bool done = false; // is looping

    int currentActive = -1; // current selected cube

    int item_LightP = 0;
    int item_LightD = 0; // Lights on/off

    // Cursor position
    std::vector<int> cursorPosition{1,1,1};

    // Directive light position
    std::vector<int> positionLightD{1,1,1};
    std::vector<int> lightIntensity{2,2};

    // Spotlight position
    std::vector<int> positionLightP{1,1,1};

    // Extrude/Dig state
    bool thereIsACubeAbove, thereIsACubeUnder = false;

    // Load/Save file
    std::string filePath;
    std::string loadFilePath, loadFilePathRBF;
    std::string rbf = "default";
    float epsilon = 1.0;

    // Camera initialisation
    Controls c;
    c.calculateVectors();
    c.computeFinalMatrices();

    // Initialize control points matrix for RBF
    Eigen::MatrixXd controlPoints(0,3);

    // Nb menus
    int nbMenus = 5;

    /** APPLICATION LOOP **/
    while(!done){
        // Collapsed windows
        int collapsedWindow = -1;
        bool addCube = false;
        bool deleteCube = false;
        bool deleteControlPoints = false;
        int indexDeleteControlPoint = -1;

        // Event loop
        SDL_Event e;
        while(windowManager.pollEvent(e)){
            // Send event to ImGui
            ImGui_ImplSDL2_ProcessEvent(&e);           

            if(e.type == SDL_QUIT){
                myCubeList.save("../backup/backup.txt", item_LightD, positionLightD, item_LightP, positionLightP, lightIntensity);
                done = true;
            }

            c.calculateVectors();   // Calculate the new vectors of the camera
            if(e.type == SDL_KEYDOWN){
                if(!ImGui::IsAnyItemActive()){ // Avoid keyboard events when an input is focused
                    // Move the cursor
                    if (e.key.keysym.sym == SDLK_a){
                        cursor.setTrans(cursor.getTrans().x - 1, cursor.getTrans().y, cursor.getTrans().z);
                    }
                    if (e.key.keysym.sym == SDLK_q){
                        cursor.setTrans(cursor.getTrans().x + 1, cursor.getTrans().y, cursor.getTrans().z);
                    }
                    if (e.key.keysym.sym == SDLK_z){
                        cursor.setTrans(cursor.getTrans().x, cursor.getTrans().y  - 1, cursor.getTrans().z);
                    }
                    if (e.key.keysym.sym == SDLK_s){
                        cursor.setTrans(cursor.getTrans().x, cursor.getTrans().y + 1, cursor.getTrans().z);
                    }
                    if (e.key.keysym.sym == SDLK_e){
                        cursor.setTrans(cursor.getTrans().x, cursor.getTrans().y, cursor.getTrans().z - 1);
                    }
                    if (e.key.keysym.sym == SDLK_d){
                        cursor.setTrans(cursor.getTrans().x, cursor.getTrans().y, cursor.getTrans().z + 1);
                    }

                    // Move the camera
                    if (e.key.keysym.sym == SDLK_KP_8){
                        glm::vec3 newPos = c.getPosition() + (c.getUp() * c.getSpeed());
                        c.setPosition(newPos);
                    }
                    if (e.key.keysym.sym == SDLK_KP_2){
                        glm::vec3 newPos = c.getPosition() - (c.getUp() * c.getSpeed());
                        c.setPosition(newPos);
                    }
                    if (e.key.keysym.sym == SDLK_KP_6){
                        glm::vec3 newPos = c.getPosition() + (c.getRight() * c.getSpeed());
                        c.setPosition(newPos);
                    }
                    if (e.key.keysym.sym == SDLK_KP_4){
                        glm::vec3 newPos = c.getPosition() - (c.getRight() * c.getSpeed());
                        c.setPosition(newPos);
                    }
                    if (e.key.keysym.sym == SDLK_KP_PLUS){
                        glm::vec3 newPos = c.getPosition() + (c.getDirection() * c.getSpeed());
                        c.setPosition(newPos);
                    }
                    if (e.key.keysym.sym == SDLK_KP_MINUS){
                        glm::vec3 newPos = c.getPosition() - (c.getDirection() * c.getSpeed());
                        c.setPosition(newPos);
                    }
                    if (e.key.keysym.sym == SDLK_KP_9){
                        c.calculateVectors();
                        c.setHorizontalAngle(c.getHorizontalAngle() + c.getSpeed());
                        c.computeFinalMatrices();
                    }
                    if (e.key.keysym.sym == SDLK_KP_7){
                        c.setHorizontalAngle(c.getHorizontalAngle() - c.getSpeed());
                    }
                    if (e.key.keysym.sym == SDLK_KP_3){
                        c.setVerticalAngle(c.getVerticalAngle() + c.getSpeed());
                    }
                    if (e.key.keysym.sym == SDLK_KP_1){
                        c.setVerticalAngle(c.getVerticalAngle() - c.getSpeed());
                    }
                    if (e.key.keysym.sym == SDLK_KP_5){
                        c.setPosition(glm::vec3(0,0,5));
                        c.setHorizontalAngle(3.14f);
                        c.setVerticalAngle(0.0f);
                    }
                }
                
            }
            c.computeFinalMatrices();   // Calculate the new matrix for the camera
        }
                    
        // Feed inputs to Dear ImGui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(windowManager.window);
        ImGui::NewFrame();
        ImGui::SetWindowCollapsed(true, ImGuiCond_Appearing);

        // Set up MVP from Camera
        const glm::mat4 ProjectionMatrix = c.getProjectionMatrix();
        const glm::mat4 ViewMatrix = c.getViewMatrix();

        // Clear window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Render the GUI
        // Cursor menu
        ImGui::Begin("CURSOR settings", NULL, ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos(ImVec2(windowWidth,0), true);
        ImGui::SetWindowSize(ImVec2(menuWidth,windowHeight+menuWidth-16*nbMenus));
        if(!ImGui::IsWindowCollapsed()){
            collapsedWindow = 0;
            ImGui::SetNextWindowCollapsed(true, 0);
        }

        ImGui::Text("Coordinates");
        cursorPosition[0] = cursor.getTrans().x;
        cursorPosition[1] = cursor.getTrans().y;
        cursorPosition[2] = cursor.getTrans().z;
        ImGui::Text("X :");
        ImGui::InputInt("x", &cursorPosition[0]);
        ImGui::Text("Y :");
        ImGui::InputInt("y", &cursorPosition[1]);
        ImGui::Text("Z :");
        ImGui::InputInt("z", &cursorPosition[2]);

        ImGui::End();

        // Light menu
        ImGui::Begin("LIGHT settings", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowSize(ImVec2(menuWidth,windowHeight+menuWidth-16*nbMenus));
        if(!ImGui::IsWindowCollapsed()){
            collapsedWindow = 1;
            ImGui::SetNextWindowCollapsed(true, 0);
        }
        if(collapsedWindow==0){
            ImGui::SetWindowPos(ImVec2(windowWidth,windowHeight+menuWidth-80), true);
        }else{
            ImGui::SetWindowPos(ImVec2(windowWidth,20), true);
        }
        
        // Directive light
        const char* itemsLight[] = { "On", "Off"};
        ImGui::Text("Lumiere directionnelle :");
        ImGui::Combo("D", &item_LightD, itemsLight, IM_ARRAYSIZE(itemsLight));
        ImGui::Text("Coordinates D");
        ImGui::InputInt("xD", &positionLightD[0]);
        ImGui::Text("Y :");
        ImGui::InputInt("yD", &positionLightD[1]);
        ImGui::Text("Z :");
        ImGui::InputInt("zD", &positionLightD[2]);

        // Spotlight
        ImGui::Text("Lumiere ponctuelle :");
        ImGui::Combo("P", &item_LightP, itemsLight, IM_ARRAYSIZE(itemsLight));
        ImGui::Text("Coordinates P");
        ImGui::InputInt("xP", &positionLightP[0]);
        ImGui::Text("Y :");
        ImGui::InputInt("yP", &positionLightP[1]);
        ImGui::Text("Z :");
        ImGui::InputInt("zP", &positionLightP[2]);

        ImGui::End();

        // File menu
        ImGui::Begin("FILE settings", NULL, ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoMove);
        ImGui::SetWindowSize(ImVec2(menuWidth,windowHeight+menuWidth-16*nbMenus));
        if(!ImGui::IsWindowCollapsed()){
            collapsedWindow = 2;
            ImGui::SetNextWindowCollapsed(true, 0);
        }
        if(collapsedWindow!=-1 && collapsedWindow<2){
            ImGui::SetWindowPos(ImVec2(windowWidth,windowHeight+menuWidth-60), true);
        }else{
            ImGui::SetWindowPos(ImVec2(windowWidth,40), true);
        }
        
        // Save
        ImGui::Text("Save file :");
        ImGui::InputText("Save Path", &filePath);
        if(ImGui::Button("Save")){
            myCubeList.save(filePath, item_LightD, positionLightD, item_LightP, positionLightP, lightIntensity);
        }

        // Load
        ImGui::Text("Load file :");
        ImGui::InputText("Load Path", &loadFilePath);
        if(ImGui::Button("Load")){

            // Read file to load
            std::vector<int> file;
            myCubeList.read(loadFilePath, file);
            
            // Save current file
            myCubeList.save("../backup/backup.txt", item_LightD, positionLightD, item_LightP, positionLightP, lightIntensity);

            // Reset cube list
            std::cout << "Deleting ..." << myCubeList.getSize() << "...cubes" << std::endl;
            while(myCubeList.getSize()){
                int i = 0;
                myCubeList.deleteCube(i);
                currentActive = -1;
            }
            
            // Load file
            myCubeList.load(file, cursorPosition, currentActive, item_LightD, positionLightD, item_LightP, positionLightP, lightIntensity);
        }

        ImGui::End();

        // File menu
        ImGui::Begin("Procedural generation", NULL, ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoMove);
        ImGui::SetWindowSize(ImVec2(menuWidth,windowHeight+menuWidth-16*nbMenus));
        if(!ImGui::IsWindowCollapsed()){
            collapsedWindow = 3;
            ImGui::SetNextWindowCollapsed(true, 0);
        }
        if(collapsedWindow!=-1 && collapsedWindow<3){
            ImGui::SetWindowPos(ImVec2(windowWidth,windowHeight+menuWidth-40), true);
        }else{
            ImGui::SetWindowPos(ImVec2(windowWidth,60), true);
        }
        
        // Save
        ImGui::Text("Control points :");
        for(int i=0; i<controlPoints.rows(); i++){
            char labelX[i+1];
            sprintf(labelX, "%s%d", "X", i);
            char labelY[i+1];
            sprintf(labelY, "%s%d", "Y", i);
            char labelZ[i+1];
            sprintf(labelZ, "%s%d", "Z", i);
            ImGui::Text("Point %d:", i);
            ImGui::InputDouble(labelX, &controlPoints(i,0));
            ImGui::InputDouble(labelY, &controlPoints(i,1));
            ImGui::InputDouble(labelZ, &controlPoints(i,2));

            char labelButton[i+20];
            sprintf(labelButton, "%s%d", "Delete control point", i);
            if(ImGui::Button(labelButton)){
                indexDeleteControlPoint = i;
                deleteControlPoints = true;
            }
        }
        if(deleteControlPoints){
            for(int j=indexDeleteControlPoint+1; j<controlPoints.rows(); j++){
                controlPoints.row(j-1) = controlPoints.row(j);
            }
            controlPoints.conservativeResize(controlPoints.rows()-1, Eigen::NoChange);
        }
        if(ImGui::Button("Add control point")){
            controlPoints.conservativeResize(controlPoints.rows()+1, Eigen::NoChange);
            controlPoints(controlPoints.rows()-1, 0) =0;
            controlPoints(controlPoints.rows()-1, 1) =0;
            controlPoints(controlPoints.rows()-1, 2) =0;
        }
        
        ImGui::Text("Load file :");
        ImGui::InputText("Load Path", &loadFilePathRBF);
        if(ImGui::Button("Load")){
            // Read file to load
            std::ifstream file;
            file.open(loadFilePathRBF);
            std::getline(file, rbf);
            float x;
            std::vector<float> res;
            while (file >> x) {
                res.push_back(x);
            }
            file.close();
            epsilon = res[0];
            res.erase(res.begin());
            controlPoints.resize(res.size()/3,3);
            int row = 0;
            for(int i=0; i<res.size(); i+=3){
                controlPoints(row, 0) = res[i];
                controlPoints(row, 1) = res[i+1];
                controlPoints(row, 2) = res[i+2];
                row++;
            }
            std::cout<<controlPoints<<std::endl;
        }

        // Generate
        if(ImGui::Button("Generate scene")){
            // Save current file
            myCubeList.save("../backup/backup.txt", item_LightD, positionLightD, item_LightP, positionLightP, lightIntensity);

            // Reset cube list
            std::cout << "Deleting ..." << myCubeList.getSize() << "...cubes" << std::endl;
            while(myCubeList.getSize()){
                int i = 0;
                myCubeList.deleteCube(i);
                currentActive = -1;
            }
            
            // Generate scene
            if(controlPoints.rows()>0){
                double lowerX, higherX = controlPoints(0,0);
                double lowerZ, higherZ = controlPoints(0,2);

                for(int i=1; i<controlPoints.rows(); i++){
                    if(controlPoints(i,0)<lowerX){
                        lowerX=controlPoints(i,0);
                    }else if(controlPoints(i,0)>higherX){
                        higherX=controlPoints(i,0);
                    }else if(controlPoints(i,2)>higherZ){
                        higherZ=controlPoints(i,2);
                    }else if(controlPoints(i,2)<lowerZ){
                        lowerZ=controlPoints(i,2);
                    }
                }
                for(int i=lowerX;i<=higherX;i++){
                    for(int j=lowerZ; j<=higherZ; j++){
                        int y = myCubeList.interpolatePoints(i,j, controlPoints, rbf, epsilon);
                        if(y>-15){
                            myCubeList.addCube(Cube());
                            myCubeList.setTrans(myCubeList.getSize()-1, i,y,j);
                            if(i==controlPoints(0,0) && y==controlPoints(0,1)&&j==controlPoints(0,2)){
                                myCubeList.setTextureIndex(myCubeList.getSize()-1, 2);
                            }
                            myCubeList.setTextureIndex(myCubeList.getSize()-1, 1);
                        }
                    }
                }
            }
            
        }

        ImGui::End();

        // Cube menu
        ImGui::Begin("CUBE settings", NULL, ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoMove);
        ImGui::SetWindowSize(ImVec2(menuWidth,windowHeight+menuWidth-16*nbMenus));
        if(!ImGui::IsWindowCollapsed()){
            collapsedWindow = 4;
        }
        if(collapsedWindow!=-1 && collapsedWindow<4){
            ImGui::SetWindowPos(ImVec2(windowWidth,windowHeight+menuWidth-20), true);
        }else{
            ImGui::SetWindowPos(ImVec2(windowWidth,80), true);
        }

        // Selected cube
        int selectedCube = currentActive;
        ImGui::Text("Selected cube :");
        ImGui::InputInt("index", &selectedCube);

        // Texture
        const char* itemsTextures[] = { "Bois", "Brique", "Cailloux", "Eau", "Goudron", "Herbe", "Marbre", "Mosaique", "Sol metalique"};
        int item_currentTexture = myCubeList.getTextureIndex(selectedCube)-1;
        ImGui::Text("Texture:");
        ImGui::Combo("Texture", &item_currentTexture, itemsTextures, IM_ARRAYSIZE(itemsTextures));
        
        // Add/Delete cube
        if(selectedCube == -1){
            addCube = ImGui::Button("Add cube");
        }else{
            deleteCube = ImGui::Button("Delete cube");
        }

        // Extrude/Dig cube
        ImGui::Text("Modify cube :");
        if(ImGui::Button("Extrude")){
            if(selectedCube!=-1 && !thereIsACubeAbove){
                cursorPosition[1]++;
                myCubeList.addCube(Cube());
                myCubeList.setTrans(myCubeList.getSize()-1, cursorPosition[0], cursorPosition[1], cursorPosition[2]);
                myCubeList.setTextureIndex(myCubeList.getSize()-1, item_currentTexture+1);
                currentActive = myCubeList.getSize()-1;
            }else{
                std::cout << "[ERROR] Cannot extrude a non-cube or cube with no space above!" << std::endl;
            }
        };
        if(ImGui::Button("Dig")){
            if(selectedCube!=-1 && thereIsACubeUnder && !thereIsACubeAbove){
                myCubeList.deleteCube(selectedCube);
                cursorPosition[1]--;
            }else{
                std::cout << "[ERROR] Cannot dig a non-cube or cube that is not above a column!" << std::endl;
            }
        };

        ImGui::End();

        // Reset texture index (from ImGui)
        myCubeList.setTextureIndex(selectedCube, item_currentTexture+1);

        // Add/Delete cube (from ImGui)
        if(addCube == true){
            myCubeList.addCube(Cube());
            myCubeList.setTrans(myCubeList.getSize()-1, cursorPosition[0], cursorPosition[1], cursorPosition[2]);
            myCubeList.setTextureIndex(myCubeList.getSize()-1, 1);
            currentActive = myCubeList.getSize()-1;
        }else if(deleteCube == true){
            myCubeList.deleteCube(selectedCube);
            currentActive = -1;
        }

        // Rendu lumière
        glUniform3f(uKd, 0.6, 0.6, 0.6);
        glUniform3f(uKs, 0, 0.0, 0.0);
        glUniform1f(uShininess, 32.0);
        glm::vec4 LightPos = ViewMatrix * glm::vec4((float) positionLightP[0], (float) positionLightP[1], (float) positionLightP[2], 1);
        glUniform3f(uLightPos_vs, LightPos.x, LightPos.y, LightPos.z);
        glm::vec4 LightDir = ViewMatrix * glm::vec4((float) positionLightD[0], (float) positionLightD[1], (float) positionLightD[2], 1);
        glUniform3f(uLightDir_vs, LightDir.x, LightDir.y, LightDir.z);
        
        // On/Off lights
        if (item_LightD == 0){
            glUniform3f(uLightIntensityD, 2.0, 2.0, 2.0);   
        }
        else {
            glUniform3f(uLightIntensityD, 0.0, 0.0, 0.0);
        }
        if (item_LightP == 0){
            glUniform3f(uLightIntensityP, 5.0, 5.0, 5.0);   
        }
        else {
            glUniform3f(uLightIntensityP, 0.0, 0.0, 0.0);
        }

        // Cursor move
        cursor.setTrans(cursorPosition[0], cursorPosition[1], cursorPosition[2]);

        // Reset MVP
        glm::mat4 ModelMatrix = glm::mat4(1.0f);
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(ViewMatrix * ModelMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix * ViewMatrix * ModelMatrix)); //Model View Projection
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(ViewMatrix * ModelMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        
        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        // Draw cube list

        // Active first texture
        int currentTexture = myCubeList.getTextureIndex(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[myCubeList.getTextureIndex(0)].getTexture()); // bind texture on GL_TEXTURE0
        glUniform1i(textures[myCubeList.getTextureIndex(0)].getUniformLocation(), 0);
        
        // Reset variables
        currentActive = -1;
        thereIsACubeAbove = false;
        thereIsACubeUnder = false;

        // Loop on cubes
        for(int i=0; i<myCubeList.getSize(); i++){

            // Get current cube
            if ((myCubeList.getTrans(i).x == cursor.getTrans().x) && (myCubeList.getTrans(i).y == cursor.getTrans().y) && (myCubeList.getTrans(i).z == cursor.getTrans().z)){
                currentActive = myCubeList.getCubeIndex(i);
            }
            // Is there a cube above current ?
            if ((myCubeList.getTrans(i).x == cursor.getTrans().x) && (myCubeList.getTrans(i).y == cursor.getTrans().y+1) && (myCubeList.getTrans(i).z == cursor.getTrans().z)){
                thereIsACubeAbove = true;
            }
            // Is there a cube under current ?
            if ((myCubeList.getTrans(i).x == cursor.getTrans().x) && (myCubeList.getTrans(i).y == cursor.getTrans().y-1) && (myCubeList.getTrans(i).z == cursor.getTrans().z)){
                thereIsACubeUnder = true;
            }

            // Active another texture if needed
            if(currentTexture != myCubeList.getTextureIndex(i)){
                currentTexture = myCubeList.getTextureIndex(i);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures[myCubeList.getTextureIndex(i)].getTexture());
                glUniform1i(textures[myCubeList.getTextureIndex(i)].getUniformLocation(), 0);
            }

            // Bind VAO
            glBindVertexArray(*myCubeList.getVAOListItem(myCubeList.getCubeIndex(i)+1));

            // Transform
            ModelMatrix = glm::scale(glm::mat4(1.0f), myCubeList.getScale(myCubeList.getCubeIndex(i)));
            ModelMatrix = glm::rotate(ModelMatrix, myCubeList.getRotDeg(myCubeList.getCubeIndex(i)), myCubeList.getRot(myCubeList.getCubeIndex(i)));
            ModelMatrix = glm::translate(ModelMatrix, myCubeList.getTrans(myCubeList.getCubeIndex(i)));
            // Reset ModelViewProjection matrices
            glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix * ViewMatrix * ModelMatrix)); //Model View Projection
            glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr( ViewMatrix * ModelMatrix)); //Model View Projection
            glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(ViewMatrix * ModelMatrix)))); //Model View Projection

            // Draw cube
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *myCubeList.getIBOListItem(myCubeList.getCubeIndex(i)+1));
            glDrawElements(GL_TRIANGLES, myCubeList.getIBOCount(i), GL_UNSIGNED_INT, (void *)0);
        }
        
        // Disable depth for cursor
        glDisable(GL_DEPTH_TEST);

        // Repeat for drawing the cursor alone
        glBindVertexArray(*myCubeList.getVAOListItem(0));

        glBindTexture(GL_TEXTURE_2D, textures[cursor.getTextureIndex()].getTexture()); // la texture est bindée sur l'unité GL_TEXTURE0
        glUniform1i(textures[cursor.getTextureIndex()].getUniformLocation(), 0);

        ModelMatrix = glm::scale(glm::mat4(1.0f), cursor.getScale());
        ModelMatrix = glm::rotate(ModelMatrix, cursor.getRotDeg(), cursor.getRot());
        ModelMatrix = glm::translate(ModelMatrix, cursor.getTrans());
        
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix * ViewMatrix * ModelMatrix)); //Model View Projection

        // Draw cursor
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *myCubeList.getIBOListItem(0));
        glLineWidth(5.0);
        glDrawElements(GL_LINES, cursor.getIBOCountBorder(), GL_UNSIGNED_INT, (void *)0);

        // Unbind
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);


        /*** Load 3D object ***/
        // NormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(ViewMatrix * ModelMatrix)))); //Model View Projection
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            2,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
            1,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update the display
        windowManager.swapBuffers();
    }

    // Destroy ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return EXIT_SUCCESS;
}
