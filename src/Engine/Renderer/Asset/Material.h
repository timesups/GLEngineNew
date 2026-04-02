#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/ext/matrix_float3x3.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"


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
	void Apply(MeshSection& section,const glm::mat4& mModel,const glm::mat4& mNormal)  
	{

		//逐pass渲染
		for(auto& pass:m_shader->m_passes)
		{
				pass->use();
				//设置矩阵信息
				pass->SetValue("GL_MATRIX_M",mModel);
				pass->SetValue("GL_MATRIX_N",mNormal);
				m_nextTextureUnit = 0;
				//批量设置参数
				for(auto& [name,property]:m_properties)
				{
					if(property.type==MaterialPropertyType::Texture2D || property.type == MaterialPropertyType::TextureCube)
					{
						const std::shared_ptr<Texture> texture = std::get<std::shared_ptr<Texture>>(property.value);
						if (!texture)
							continue;
						texture->Bind(m_nextTextureUnit);
						pass->SetValue(property.name, m_nextTextureUnit);
						m_nextTextureUnit += 1;
					}
					else 
					{
						switch (property.type) 
						{
						case MaterialPropertyType::Float:
							pass->SetValue(property.name,std::get<float>(property.value));
							break;
						case  MaterialPropertyType::Int:
							pass->SetValue(property.name,std::get<int>(property.value));
							break;
						case MaterialPropertyType::Bool:
							pass->SetValue(property.name,std::get<bool>(property.value));
							break;
						case MaterialPropertyType::Vec2:
							pass->SetValue(property.name,std::get<glm::vec2>(property.value));
							break;
						case MaterialPropertyType::Vec3:
							pass->SetValue(property.name,std::get<glm::vec3>(property.value));
							break;
						case MaterialPropertyType::Vec4:
							pass->SetValue(property.name,std::get<glm::vec4>(property.value));
							break;
						case MaterialPropertyType::Mat3:
							pass->SetValue(property.name,std::get<glm::mat3>(property.value));
							break;
						case MaterialPropertyType::Mat4:
							pass->SetValue(property.name,std::get<glm::mat4>(property.value));
							break;
						}
						
					}
				}
				//设置pass的状态
				pass->SetState();
				//绘制网格体
				section.mesh->Draw();
		}
	}
	void SetFloatProperty(const std::string& name, float value) { setProperty(name, MaterialPropertyType::Float, value); }
	void SetIntProperty(const std::string& name, int value) { setProperty(name, MaterialPropertyType::Int, value); }
	void SetBoolProperty(const std::string& name, bool value) { setProperty(name, MaterialPropertyType::Bool, value); }
	void SetVec2Property(const std::string& name, const glm::vec2& value) { setProperty(name, MaterialPropertyType::Vec2, value); }
	void SetVec3Property(const std::string& name, const glm::vec3& value) { setProperty(name, MaterialPropertyType::Vec3, value); }
	void SetVec4Property(const std::string& name, const glm::vec4& value) { setProperty(name, MaterialPropertyType::Vec4, value); }
	void SetMat3Property(const std::string& name, const glm::mat3& value) { setProperty(name, MaterialPropertyType::Mat3, value); }
	void SetMat4Property(const std::string& name, const glm::mat4& value) { setProperty(name, MaterialPropertyType::Mat4, value); }
	void SetTexture2DProperty(const std::string& name, std::shared_ptr<Texture> texture)
	{
		setProperty(name, MaterialPropertyType::Texture2D, std::move(texture));
	}
	void SetTextureCubeProperty(const std::string& name, std::shared_ptr<Texture> texture)
	{
		setProperty(name, MaterialPropertyType::TextureCube, std::move(texture));
	}

	std::string m_name;

private:
	template<typename T>
	void setProperty(const std::string& name, MaterialPropertyType type, T&& value)
	{
		MaterialProperty prop;
		prop.name = name;
		prop.type = type;
		prop.value = std::forward<T>(value);
		prop.isDirty = true;
		m_properties[name] = std::move(prop);
	}

	std::shared_ptr<Shader> m_shader;
	std::unordered_map<std::string, MaterialProperty> m_properties;

	int m_nextTextureUnit = 0;
	bool m_isApplied = false;
};