#include "LightingApp.h"


LightingApp::LightingApp()
{
}


LightingApp::~LightingApp()
{
}

void LightingApp::createOpenGLBuffers(FBXFile* fbx)
{
	//create the GL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		//storage for the OpenGL data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
					 mesh->m_vertices.size() * sizeof(FBXVertex),
					 mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					 mesh->m_indices.size() * sizeof(unsigned int),
					 mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
							  sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
							  sizeof(FBXVertex),
							  ((char*)0) + FBXVertex::NormalOffset);

		glEnableVertexAttribArray(2); // texCoords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE,
							  sizeof(FBXVertex),
							  (char*)offsetof(FBXVertex, texCoord1));



		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		mesh->m_userData = glData;
	}
}

void LightingApp::cleanupOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

bool LightingApp::Init(const char* location)
{

	m_fbx = new FBXFile();
	m_fbx->load(location);
	createOpenGLBuffers(m_fbx);


	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Normal; \
							layout(location=2) in vec2 TexCoords; \
							out vec4 vNormal; \
							out vec2 vTexCoords; \
							uniform mat4 ProjectionView; \
							void main() { vNormal = Normal; vTexCoords = TexCoords; \
							gl_Position = ProjectionView*Position; }";

	const char* fsSource = "#version 410\n \
							in vec4 vNormal; \
							in vec2 vTexCoords; \
							out vec4 FragColor; \
							uniform float time; \
							uniform sampler2D diffuse; \
							void main() { \
							float d = max(0.2f, \
							dot(normalize(vNormal.xyz), \
							vec3(sin(time),1,cos(time)) ) ); \
							FragColor = texture(diffuse, vTexCoords) * vec4(d,d,d,1)*2; }";


	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
	
		glGetProgramInfoLog(m_program, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

bool LightingApp::Update(float deltaTime)
{
	return true;
}

void LightingApp::Draw(FlyCamera *fCam)
{
	glUseProgram(m_program);


	glUniform1f(glGetUniformLocation(m_program, "time"), glfwGetTime());

	// bind the camera
	int loc = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(fCam->GetProjectionView()[0][0]));

	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void LightingApp::Destroy()
{
	cleanupOpenGLBuffers(m_fbx);
	cleanupOpenGLBuffers(m_fbx2);
	glDeleteProgram(m_program);
}