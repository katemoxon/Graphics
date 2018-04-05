#include "MyApplication.h"
#include <iostream>

int MyApplication::Run()
{
	Init();

	while (true)
	{
		Update();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		Draw(fCam->GetProjectionView());
	}

	Destroy();

	return 0;
}

void MyApplication::setCamera()
{
	fCam = new FlyCamera;
	fCam->m_pWindow = m_window;

	fCam->SetTransform(mat4(1));
	fCam->SetPosition(vec3(5000, 5000, 5000));
	fCam->SetLookAt(vec3(0, 15, 0), vec3(50, 0, 50), vec3(0, 1, 0));
	fCam->SetupPerspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);

	fCam->SetFlySpeed(25);

	createWindow("Application", 1280, 720);
	fCam->SetInputWindow(m_window);
}

bool MyApplication::Init()
{
	setCamera();

	m_clearColour = vec3(0.5, 0.5, 0.5);
	ImGui_ImplGlfwGL3_Init(m_window, true);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = (float)m_uiScreenWidth;
	io.DisplaySize.y = (float)m_uiScreenHeight;

	m_particles = new ParticleEmitter();
	m_light = new LightingApp();
	m_light2 = new LightingApp();
	m_texture = new Textures();
	boundingSphere = new BoundingSphere();

	boundingSphere->centre = vec3(0, 0, 0);
	boundingSphere->radius = 5;

	const char* soulspear = "./data/models/soulspear/soulspear.fbx";
	//const char* dragon = "./data/models/characters/Marksman/Marksman.fbx";

	deltaTime = 0;
	previousTime = 0;

	generateGrid(100, 100);

	//create shaders
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Colour; \
							layout(location=2) in vec2 texCoords; \
							out vec4 vColour; \
							out vec2 vTexCoords; \
							out vec4 vPosition; \
							uniform mat4 ProjectionView; \
							void main() { vPosition = Position; vColour = Colour; vTexCoords = texCoords; gl_Position = ProjectionView * Position;}";

	const char* fsSource = "#version 410\n \
							in vec4 vPosition; \
							in vec4 vColour; \
							in vec2 vTexCoords; \
							uniform sampler2D diffuse; \
							uniform float snowLevel; \
							out vec4 FragColor; \
							void main() {\
							if (vPosition.y > snowLevel) \
							{ FragColor = vColour * vec4(1,1,1,1);}\
							else{FragColor = texture(diffuse, vTexCoords); }}";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	m_particles->Init(5000, 1000, 5.0f, 20.0f, 1, 5, 0.1f, 0.1f,
		glm::vec4(0, 1, 1, 1), glm::vec4(1, 1, 1, 1));
	m_light->Init(soulspear);
	//m_light2->Init(dragon);
	m_texture->Init();

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	return true;
}

bool MyApplication::Update()
{
	float currentTime = (float)glfwGetTime();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	m_particles->Update(deltaTime, fCam->GetTransform());
	m_light->Update(deltaTime);
	m_light2->Update(deltaTime);

	fCam->Update(deltaTime);



ImGui_ImplGlfwGL3_NewFrame();
	return true;
}

void MyApplication::Draw(const mat4& m_projectionViewMatrix)
{
	glClearColor(m_clearColour.r, m_clearColour.g,
		m_clearColour.b, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui::ColorEdit3("clear color", glm::value_ptr(m_clearColour));
	ImGui::DragFloat("Snow Level", &snowLevel);
	
	glUseProgram(m_programID);

	unsigned int projectionViewUniform =
		glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false,
		glm::value_ptr(m_projectionViewMatrix));
	glBindVertexArray(m_VAO);


	/////////////////////// grid
	glUseProgram(m_programID);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(m_texture->m_program, "tex0"), 0);
	glBindTexture(GL_TEXTURE_2D, m_texture->m_texture);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	///////////////////////


	m_particles->Draw(m_projectionViewMatrix);
	/////////////////////// soulspear code
	glUseProgram(m_texture->m_program);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(m_texture->m_program, "tex0"), 1);
	glBindTexture(GL_TEXTURE_2D, m_texture->m_texture2);

	if (fCam->Culling(boundingSphere))
	{
		m_light->Draw(fCam);
	}
	/////////////////////// endcode

	/////////////////////// dragon code
	//glUseProgram(m_texture->m_program);
	//glActiveTexture(GL_TEXTURE0);
	//glUniform1i(glGetUniformLocation(m_texture->m_program, "tex0"), 2);
	//glBindTexture(GL_TEXTURE_2D, m_texture->m_texture3);
	//m_light2->Draw(fCam);
	/////////////////////// endcode

	glUseProgram(m_programID);
	glUniform1f(glGetUniformLocation(m_programID, "snowLevel"), snowLevel);
		
	ImGui::Render();

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void MyApplication::Destroy()
{
	m_particles->Destroy();
	m_light->Destroy();
	m_light2->Destroy();
	m_texture->Destroy();
}

void MyApplication::generateGrid(unsigned int rows, unsigned int cols)
{


	float scale = (1.0f / rows) * 3;

	float frequency = 5;
	float amplitude = 10;



	Vertex* aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			float y = (glm::perlin(glm::vec2(c, r) * scale * frequency) * 0.5f + 0.5f) * amplitude;

			aoVertices[r * cols + c].position = vec4(
				(float)c, y, (float)r, 1);
			// create some arbitrary colour based off something
			// that might not be related to tiling a texture
			//vec3 colour = vec3(sinf((c / (float)(cols - 1)) *(r / (float)(rows - 1))));
			vec3 colour = vec3(y / (rows - 1), y / (rows - 1), y / (rows - 1));
			aoVertices[r * cols + c].colour = vec4(colour, 1);
			aoVertices[r * cols + c].texCoords = vec2((float)c / (cols - 1), (float)r / (rows - 1));

		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) *
		6];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			// triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			// triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	// Generate our GL Buffers
	// Lets move these so that they are all generated together
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	//Add the following line to generate a VertexArrayObject
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);


	// create and bind buffers to a vertex array object (VBO)
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex),
		aoVertices, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);


	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)offsetof(Vertex, texCoords));

	//IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 *
		sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	indexCount = ((rows - 1) * (cols - 1)) * 6;

	delete[] auiIndices;
	delete[] aoVertices;
}

bool MyApplication::createWindow(const char* title, int width, int height)
{
	if (glfwInit() == GL_FALSE)
		return false;

	m_window = glfwCreateWindow(1280, 720, "Application", nullptr, nullptr);
	if (m_window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); });

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	std::cout << "GL: " << major << "." << minor << std::endl;

	glClearColor(0.25f, 0.25f, 0.25f, 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void MyApplication::destroyWindow()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
	ImGui_ImplGlfwGL3_Shutdown();
}