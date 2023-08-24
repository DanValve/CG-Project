#pragma once
#include <string>
#include <unordered_map>
#include "BaseGLM.h"

namespace Cobalt {

	class Shader {

	public:
		Shader(const std::string& filePath);
		~Shader();

		void Bind()const;
		void UnBind()const;
		//set uniforms
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1iv(const std::string& name, int size, int* value);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform3f(const std::string& name, const glm::vec3& v);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform4f(const std::string& name,const glm::vec4& v);
		void SetUniform1f(const std::string& name, float value);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
		unsigned int GetRendererID()const { return m_RenderID; }

	private:
		struct ShaderProgram {

		public:
			std::string VertexShader = "\0";
			std::string FragmentShader = "\0";
			std::string GeometryShader = "\0";
			std::string ComputeShader = "\0";

		};
		unsigned int m_RenderID;
		std::string m_FilePath;
		std::unordered_map<std::string, int> m_UniformLocationCache;

	private:
		ShaderProgram ParseShader(const std::string& filePath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const ShaderProgram& shader);
		int GetUniformLocation(const std::string& name);
	};
}