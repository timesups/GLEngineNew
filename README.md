# GLEngineNew

基于 OpenGL 的学习用小型渲染引擎，采用 **Entity–Component** 组织场景，自定义 **包裹式 GLSL** 管线描述，并支持着色器 **热重载**。

---

## 项目简介

本仓库实现了一个轻量级图形程序框架：使用 **GLFW** 创建窗口与上下文，**Glad** 加载 OpenGL 函数，通过 **Assimp** 加载模型、**stb_image** 加载纹理，**GLM** 处理数学。**渲染入口**为 `Renderer::Render`，**资源**由 `AssetManager` 与 `LoaderManager` 管理；场景实体由 `EntityManager` 维护，网格绘制由 `MeshRender` + `Material` + `ShaderPass` 完成。

---

## 技术栈

| 类别 | 依赖 |
|------|------|
| 语言与标准 | C++20 |
| 图形 API | OpenGL（Core Profile，上下文版本在 `Window` 中配置） |
| 窗口 / 输入 | GLFW |
| OpenGL 加载 | Glad（`external/glad`） |
| 模型 | Assimp |
| 纹理 | stb_image（`external/stb`） |
| 数学 | GLM |
| 日志 | 自研 `Log`（`std::format`） |
| 构建 | Visual Studio 工程 `GLEngineNew.vcxproj`（v143） |

第三方库头文件与库路径在工程属性中配置（如本机 `AdditionalIncludeDirectories` / `AdditionalLibraryDirectories`），克隆后需按你的环境调整或改为 vcpkg/CMake。

---

## 功能概况

- **窗口**：创建上下文、帧循环、帧缓冲尺寸、鼠标捕获与 **F1 切换光标显示**（边沿检测，避免长按连切）。
- **渲染**：清屏、相机 UBO（`UniformBuffer` + `DataInputsStd140` 与 `GLInput.glsl` 对齐）、遍历实体渲染。
- **相机**：`Camera` + `Transform`，自由移动、鼠标视角、滚轮 FOV 等（由 `Window` 写入输入状态）。
- **资源**：着色器 / 纹理 / 模型加载；`AssetManager` 持有默认材质与缓存；**着色器文件变更后自动重新编译**（`LoaderManager::UpdateAssetFromDisk`）。
- **材质**：`Material` 基于 `std::variant` 的属性与多种 `Set*Property`，纹理单元在 `Apply` 中顺序绑定。
- **着色器格式**：根目录 `.glsl` 使用下文「自定义 GLSL」块结构，非裸 GLSL 文件。

---

## 仓库结构（概要）

```text
GLEngineNew/
├── assets/                 # 运行时资源（模型、纹理、着色器）
│   ├── shaders/            # 根着色器 + include/
│   ├── models/
│   └── textures/
├── external/               # glad、stb、imgui 等第三方源码
├── src/
│   ├── main.cpp
│   └── Engine/
│       ├── Core/           # Window、LoaderManager、Log
│       ├── Entity/         # Entity、EntityManager、Camera、MeshRender…
│       └── Renderer/       # Renderer、AssetManager、UniformBuffer、Asset/
└── GLEngineNew.vcxproj
```

---

## 构建与运行

1. 使用 **Visual Studio** 打开 `GLEngineNew.vcxproj`（或先打开解决方案若你有 `.sln`）。
2. 配置与平台选择 **x64**，确保已安装工程所用 **Windows SDK** 与 **MSVC v143**。
3. 将 **GLFW、Assimp、GLM** 等头文件与 `.lib` 路径指向本机安装位置（与 `.vcxproj` 中一致），并保证运行时可找到对应 **DLL**（若有）。
4. 将 **工作目录** 设为项目根或保证 `assets/` 相对路径可从可执行文件解析（`main` 中使用 `assets/shaders/...` 等相对路径）。

运行成功后可通过修改 `assets/shaders` 下文件触发着色器热重载（在渲染循环中更新）。

---

## 输入与操作（参考）

具体键位以 `Window.cpp` 中 `processInput` 为准，常见包括：**WASD** 移动、**E/Q** 垂直、**Shift** 冲刺、**鼠标** 视角、**ESC** 关闭窗口、**F1** 显示/隐藏光标。

---

## 自定义 GLSL 着色器文件格式说明

本文描述引擎中 `LoaderManager` 对 **根着色器文件**（如 `assets/shaders/*.glsl`）的解析规则。该格式在标准 GLSL 外包了一层「Unity 风格」的块结构，并由加载器切出 `GLSLPROGRAM`～`ENDGLSL` 之间的代码，分别打上 `VERTEX` / `FRAGMENT` 宏后编译为 OpenGL 4.2 Core 着色器。

### 1. 整体结构

根文件必须包含 **`SubShader`** 块（大小写不敏感，加载器会对全文做小写后再查找 `subshader`）。

推荐骨架：

```text
GLSLShader
{
    Properties
    {
        /* 预留：当前加载器不解析 Properties 内容 */
    }
    SubShader
    {
        Pass
        {
            /* 渲染状态（可选） */
            GLSLPROGRAM
            /* 共用 GLSL 源码：通过 #ifdef VERTEX / FRAGMENT 分片 */
            ENDGLSL
        }
        /* 可有多个 Pass */
    }
}
```

- **`GLSLShader` / `Properties`**：仅为约定式外层，**加载器不读取 Properties**。
- **`SubShader`**：必填；其 **第一个 `{` 与配对的 `}`** 之间的内容参与后续 `Pass` 扫描。
- **`Pass`**：至少一个；见下文 **Pass 关键字与换行** 限制。

### 2. Pass 块与换行要求

加载器在 `SubShader` 内部用子串查找 **`pass` 后紧跟换行** 的模式（实现上为 `pass\n`，且对当前子串做过小写处理）。

因此请保证：

- 关键字写作 `Pass` 或 `pass` 均可（子串会先转小写）。
- **`Pass` 与下一行之间必须是换行**，例如：

```text
    Pass
    {
```

若写成 `Pass {` 同一行，**可能无法匹配** `pass\n`，导致该 Pass 被忽略或无法解析。

每个 `Pass` 对应引擎里的一个 **ShaderPass**（一对顶点/片段程序；若检测到几何着色器宏则走特殊分支，见第 6 节）。

### 3. GLSL 源码区间：`GLSLPROGRAM` … `ENDGLSL`

- 仅在 **`GLSLPROGRAM` 与 `ENDGLSL` 之间** 的行会进入实际 GLSL 源码（行内包含这两个标记的整行会被去掉，不进入源码）。
- 该区间的代码会 **原样拼接**（前面加换行累积），再分别包上版本与宏（见第 6 节）。

#### 顶点 / 片元分工（推荐写法）

使用宏区分阶段，与加载器注入的 `#define VERTEX` / `#define FRAGMENT` 配合：

```glsl
#ifdef VERTEX
void main() { ... }
#endif

#ifdef FRAGMENT
out vec4 FragColor;
void main() { ... }
#endif
```

### 4. `#include` 规则

- 写在 **最终会进入 `GLSLPROGRAM` 的文本里**（或与之一同被预处理的根文件/被包含文件中）。
- 语法：`#include "相对路径字符串"`（须 **双引号**）。
- **解析方式**：取**当前正在读取的文件**路径中，最后一个 `/` **之前**的目录为 `rootFolder`，再拼接为：  
  `rootFolder + "/" + 引号内路径`  
  例如根文件为 `assets/shaders/Foo.glsl`，`#include "/include/Core.glsl"` 会变为 `assets/shaders//include/Core.glsl`（连续斜杠一般仍可被文件系统接受）。

被包含文件中的 `#include` 同样按**该文件自身路径**计算 `rootFolder`，支持多级嵌套。

### 5. Pass 内的渲染状态（`GLSLPROGRAM` 之外）

在 `Pass {` 与 `GLSLPROGRAM` 之间可写若干**整行**配置（解析时对行做小写后再匹配子串）。常见项如下。

#### 5.1 Pass 名称

- 行内含 `name` 且带双引号时，引号内内容作为 **Pass 名称**（`passName`）。

#### 5.2 深度

- **`ZWrite on|off`**：`off` 关闭深度写入，否则开启。
- **`ZFunc <比较函数>`**：深度比较函数，字符串需与下表一致（小写）：  
  `always`、`never`、`less`、`equal`、`lequal`、`greater`、`notequal`、`gequal`。

#### 5.3 裁剪

- **`Cull front|back|off`**

#### 5.4 混合

- **`Blend <源因子> <目标因子>`**  
  因子名为小写、无空格枚举名，例如：`srcalpha`、`oneminussrcalpha`。  
  支持的符号与 `LoaderManager.cpp` 中 `DeterminBlendFunc` 一致，例如：`zero`、`one`、`srccolor`、`oneminuscrccolor`、`dstcolor`、`oneminusdstcolor`、`srcalpha`、`oneminussrcalpha`、`dstalpha`、`oneminusdstalpha`。
- **`BlendEq <方程>`**：`add`、`subtract`、`reversesubtract`、`min`、`max`。

#### 5.5 模板（Stencil）

出现含 **`stencil`** 的一行后进入模板子块，直到匹配的 `}`（简化状态机，与括号嵌套无关）。

子块内可包含（关键字匹配为小写子串）：

- **`Mask <值>`**：按 C++ `stoi(..., 0)` 解析，支持 `0xff` 等进制形式。
- **`Func <深度比较名>`**：同 ZFunc 的字符串集合。
- **`Ref <值>`**：整数。
- **`Fail` / `DpFail` / `DpPass`**：模板操作，字符串为 `keep`、`zero`、`replace`、`incr`、`incrwrap`、`decr`、`decrwrap`、`invert` 等（见 `DeterminOpByString`）。

注意：实现上 **`Func` 会与任意含 `func` 的行匹配**；在 Stencil 子块内应使用约定好的行格式，避免与无关字段混淆。

### 6. 编译时注入与几何着色器

对每个 Pass 提取出的 `realPassCode`，加载器会生成：

- **顶点着色器**：`#version 420 core` + `#define VERTEX` + `realPassCode`
- **片段着色器**：`#version 420 core` + `#define FRAGMENT` + `realPassCode`

若 **整个 Pass 原始文本**（`passCode`）中出现子串 **`#ifdef GEOMETRY`**（大小写敏感，按源码查找），则会把 **片段着色器侧** 的拼接改为带 `#define GEOMETRY` 的同一份 `realPassCode`（与引擎内几何着色器路径一致；具体以 `ShaderPass` 实现为准）。

### 7. 与引擎内置头文件的关系

`#include` 的公共头（如 `GLInput.glsl`、`Transform.glsl`）中通常声明：

- 顶点属性：`aPosition`、`aNormal`、`aTexcoord0` 等（与网格顶点布局一致）。
- UBO：`camera_buffer`（`std140`，binding 与 C++ 侧 `UniformBuffer` 一致）。
- 宏/函数：如 `ObjectToClipPos` 等（见各 `include` 文件）。

编写 Pass 内 GLSL 时应与这些声明保持一致。

### 8. 热重载

根文件及被 `#include` 的文件若参与依赖登记，在磁盘修改时间变化时会触发对应 `Shader` 重新走上述流程编译。路径与依赖关系由 `LoaderManager` 的 `m_shaderFiles` / `FileState` 维护。

### 9. 常见错误

| 现象 | 可能原因 |
|------|----------|
| 报错无 SubShader | 根文件缺少 `SubShader` 块或拼写无法被小写后匹配为 `subshader`。 |
| 报错无 Pass | 无有效 `Pass` 块，或 `Pass` 未单独起行（未形成 `pass\n`）。 |
| 包含文件找不到 | `#include` 引号路径与 `rootFolder` 拼接后不是实际文件路径；或用了非双引号。 |
| 编译失败 | `GLSLPROGRAM` 内语法与 420 core 不符；或缺少 `#ifdef VERTEX`/`FRAGMENT` 对应入口。 |

### 10. 参考示例

仓库内可参考：

- `assets/shaders/DefaultShader.glsl` — 最简 Pass + 共用 GLSL。
- `assets/shaders/DefaultTexture.glsl` — 含 Cull / Stencil / Blend 与纹理采样。

更细节的解析实现见：`src/Engine/Core/LoaderManager.cpp`（`LoadShader`、`ReadAndPerprocessShaderFile`、`getBlockContent`）。
