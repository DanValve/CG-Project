#include "cbpch.h"
#include"Shader.h"
#include "GLCore.h"

namespace Cobalt {

	Shader::Shader(const std::string& filePath)
		:m_RenderID(0), m_FilePath(filePath)
	{

		ShaderProgram shaderSource = ParseShader(filePath);
		m_RenderID = CreateShader(shaderSource);
	}

	Shader::~Shader()
	{
		GLCall(glDeleteProgram(m_RenderID));
	}



	Shader::ShaderProgram Shader::ParseShader(const std::string& filePath) {
		enum class ShaderType {
			NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2,COMPUTE=3
		};
		std::ifstream stream(filePath);
		std::string line;
		std::stringstream ss[4];
		ShaderType type = ShaderType::NONE;

		while (std::getline(stream, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					type = ShaderType::FRAGMENT;
				}
				else if (line.find("geometry") != std::string::npos) {
					type = ShaderType::GEOMETRY;
				}
				else if (line.find("compute") != std::string::npos) {
					type = ShaderType::COMPUTE;
				}
			}
			else {
				ss[(int)type] << line << '\n';
			}
		}
		return{ ss[0].str(),ss[1].str(),ss[2].str(),ss[3].str()};
	}

	unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();//returns pointer to a null terminated const array

		glShaderSource(id, 1, &src, nullptr);
		//the shader, how many source codes,ptr to the ptr that has the source code
		//null assumes the string is null terminated
		glCompileShader(id);
		int validation;
		glGetShaderiv(id, GL_COMPILE_STATUS, &validation);

		if (!validation) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)_malloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			switch (type) {

			case GL_VERTEX_SHADER:
				std::cout << "Failed to compile Vertex Shader\n";
				break;
			case GL_FRAGMENT_SHADER:
				std::cout << "Failed to compile Fragment Shader\n";
				break;
			case GL_GEOMETRY_SHADER:
				std::cout << "Failed to compile Geometry Shader\n";
				break;
			case GL_COMPUTE_SHADER:
				std::cout << "Failed to compile Compute Shader\n";
				break;
			}
			std::cout << message << "\n";
			glDeleteShader(id);
			return 0;
		}
		return id;
	}
	unsigned int Shader::CreateShader(const ShaderProgram& shader) {
		//we cant to generate and bind a shader
		unsigned int program = glCreateProgram();

		unsigned int vs;
		unsigned int fs;
		unsigned int gs;
		unsigned int cs;
		if (shader.VertexShader != "\0") {
			vs = CompileShader(GL_VERTEX_SHADER, shader.VertexShader);
			glAttachShader(program, vs);
			glDeleteShader(vs);
		}
		if (shader.FragmentShader != "\0") {
			fs = CompileShader(GL_FRAGMENT_SHADER, shader.FragmentShader);
			glAttachShader(program, fs);
			glDeleteShader(fs);
		}

		if (shader.GeometryShader != "\0") {
			gs = CompileShader(GL_GEOMETRY_SHADER, shader.GeometryShader);
			glAttachShader(program, gs);
			glDeleteShader(gs);
		}
		if (shader.ComputeShader != "\0") {
			cs = CompileShader(GL_COMPUTE_SHADER, shader.ComputeShader);
			glAttachShader(program, cs);
			glDeleteShader(cs);
		}
		glLinkProgram(program);
		glValidateProgram(program);
		if (shader.VertexShader != "\0" && shader.ComputeShader != "\0") {
			ASSERT(0,"Compute Shaders cant be linked to other programs")
		}
		return program;

	}


	void Shader::Bind() const
	{
		GLCall(glUseProgram(m_RenderID));
	}

	void Shader::UnBind() const
	{
		GLCall(glUseProgram(0));
	}


	void Shader::SetUniform1i(const std::string& name, int value)
	{
		GLCall(glUniform1i(GetUniformLocation(name), value));

	}
	void Shader::SetUniform1iv(const std::string& name, int size, int* value) {
		GLCall(glUniform1iv(GetUniformLocation(name), size, value));
	}

	void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
	{
		GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
	}

	void Shader::SetUniform3f(const std::string& name, const glm::vec3& v)
	{
		GLCall(glUniform3f(GetUniformLocation(name), v.x, v.y, v.z));
	}

	void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
	}

	void Shader::SetUniform4f(const std::string& name, const glm::vec4& v)
	{
		GLCall(glUniform4f(GetUniformLocation(name), v.x, v.y, v.z, v.w));
	}

	void Shader::SetUniform1f(const std::string& name, float value)
	{
		GLCall(glUniform1f(GetUniformLocation(name), value));


	}

	void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
	{
		GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
		//expects(location,#of matrices,?transpose,ptr to the first element)
	}

	int Shader::GetUniformLocation(const std::string& name)
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {

			return m_UniformLocationCache[name];
		}

		GLCall(int location = glGetUniformLocation(m_RenderID, name.c_str()));
		m_UniformLocationCache[name] = location;
		return location;
	}
}