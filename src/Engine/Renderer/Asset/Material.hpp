#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Texture.hpp"


enum class MaterialPropertyType
{
	Float,
	Int,
	Bool,
	Vec2,
	Vec3,
	Vec4,
	Mat3,
	Mat4,
	Texture2D,
	TextureCube
};

using MaterialPropertyValue = std::variant<
	float,
	int,
	bool,
	glm::vec2,
	glm::vec3,
	glm::vec4,
	glm::mat3,
	glm::mat4,
	std::shared_ptr<Texture>
>;


struct MaterialProperty
{
	std::string name;
	MaterialPropertyType type;
	MaterialPropertyValue value;
	bool isDirty = true;
};

struct MeshSection {
	std::shared_ptr<Mesh> mesh;
	int materialIndex; // 指向材质数组的索引
	std::string name;
};


class Material 
{
public:
	Material(std::shared_ptr<Shader> shader):m_shader(shader) {}
	void Apply(MeshSection& section,const glm::mat4& mModel,const glm::mat4& mNormal) const 
	{
		for(auto& pass:m_shader->m_passes)
		{
				pass->use();
				//设置矩阵信息
				pass->SetValue("GL_MATRIX_M",mModel);
				pass->SetValue("GL_MATRIX_N",mNormal);
				//批量设置参数
				
				section.mesh->Draw();
		}
	}
public:
	std::string m_name;
private:
	std::shared_ptr<Shader> m_shader;
	std::unordered_map<std::string, MaterialProperty> m_properties;

	int m_nextTextureUnit = 0;
	std::unordered_map<std::string, int>m_textureUnits;

	bool m_isApplied = false;
};