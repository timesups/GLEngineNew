#pragma once

#include <cstddef>
#include <type_traits>

#include <glad/glad.h>
#include <glm/glm.hpp>

/// OpenGL Uniform Buffer Object 封装：分配 GPU 存储、上传数据、绑定到 binding 与着色器中 layout(binding=N) 对应。
class UniformBuffer
{
public:
	UniformBuffer() = default;

	explicit UniformBuffer(size_t sizeBytes, GLenum usage = GL_STATIC_DRAW);
	~UniformBuffer();

	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;

	UniformBuffer(UniformBuffer&& other) noexcept;
	UniformBuffer& operator=(UniformBuffer&& other) noexcept;

	/// 创建或重建缓冲；已有缓冲会先删除。
	void Create(size_t sizeBytes, GLenum usage = GL_DYNAMIC_DRAW);
	void Destroy();

	bool IsValid() const { return m_id != 0; }
	GLuint GetId() const { return m_id; }
	size_t GetByteSize() const { return m_size; }

	/// 将整个 UBO 绑到绑定点（对应 uniform block 的 binding = N）。
	void BindBase(GLuint uniformBlockBinding) const;

	/// 上传 [offset, offset + size)；size 不得超过 Create 时的大小。
	void Upload(const void* data, size_t size, size_t offset = 0);

	/// trivially copyable 类型整块上传；须与 GLSL std140 内存布局一致。
	template<typename T>
	void UploadStruct(const T& value, size_t offset = 0)
	{
		static_assert(std::is_trivially_copyable_v<T>, "UBO payload must be trivially copyable");
		Upload(&value, sizeof(T), offset);
	}

private:
	GLuint m_id = 0;
	size_t m_size = 0;
	GLenum m_usage = GL_DYNAMIC_DRAW;
};

/// 与 assets/shaders/include/GLInput.glsl 中 `DataInputs`（std140）二进制布局一致。
/// GLSL 中 vec3 + float 占同一 16 字节槽位，CPU 侧用 vec4 表达以免与 glm::vec3 对齐差异错位。
struct alignas(16) DataInputsStd140
{
	glm::mat4 GL_MATRIX_V{};
	glm::mat4 GL_MATRIX_P{};
	glm::vec4 _CameraPosition_zNear{}; // xyz = _CameraPosition, w = _zNear
	glm::vec4 _zFar_pad{};             // x = _zFar，其余为 std140 块尾填充
};

static_assert(sizeof(DataInputsStd140) == 160,
	"DataInputsStd140 must match std140 DataInputs in GLInput.glsl (expected 160 bytes)");
