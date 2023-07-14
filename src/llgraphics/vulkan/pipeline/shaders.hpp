#pragma once 

#include "llgraphics/vulkan/utils.hpp"
#include "utils/file.hpp"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace plt 
{


	enum ShaderKind  : int{
		None = 0,
		Vertex = 1,
		Fragment = 2,
		Geometry = 3,
		Compute = 4,
		TessellationControl = 5,
		TessellationEvaluation = 6,
		ShaderKindCount = 7,
	};

	static constexpr vk::ShaderStageFlagBits kind2flagBits(ShaderKind kind)
	{
		switch (kind)
		{
		case ShaderKind::Vertex:
			return vk::ShaderStageFlagBits::eVertex;
		case ShaderKind::Fragment:
			return vk::ShaderStageFlagBits::eFragment;
		case ShaderKind::Geometry:
			return vk::ShaderStageFlagBits::eGeometry;
		case ShaderKind::Compute:
			return vk::ShaderStageFlagBits::eCompute;
		case ShaderKind::TessellationControl:
			return vk::ShaderStageFlagBits::eTessellationControl;
		case ShaderKind::TessellationEvaluation:
			return vk::ShaderStageFlagBits::eTessellationEvaluation;
		default:
			return vk::ShaderStageFlagBits::eAll;
		}
	}

	class Shader 
	{
		ShaderKind _kind = ShaderKind::None;
	
		vk::ShaderModule _module;
		std::vector<uint8_t> _data;
		public:

		Shader() {}
		Shader(ShaderKind kind) : _kind(kind) {}

		ShaderKind kind() const { return _kind; }

		vk::ShaderModule module() const { return _module; }

		static Result<Shader> fromData(vk::Device& dev, const std::vector<uint8_t>& data, ShaderKind kind)
		{
			Shader shader(kind);
			shader._data = data;


			vk::ShaderModuleCreateInfo createInfo = vk::ShaderModuleCreateInfo()
				.setCodeSize(data.size())
				.setPCode(reinterpret_cast<const uint32_t*>(data.data()));


			vkTry$(dev.createShaderModule(&createInfo, nullptr, &shader._module));

			return Result<Shader>::ok(std::move(shader));
		}
		static Result<Shader> fromFile(vk::Device& dev, const std::string& path, ShaderKind kind)
		{
			auto file = try$(File::openBinary(path));
			auto data = try$(file.readWhole());

			return Shader::fromData(dev, data, kind);
		}


		void deinit(vk::Device& dev)
		{
			dev.destroyShaderModule(_module);
		}
	};

	class ShaderPipeline
	{

		std::vector<Shader> _shaders;
		std::vector<vk::PipelineShaderStageCreateInfo> _stageInfos;	
		public:

		ShaderPipeline() {}

		void addShader(Shader shader)
		{
			_shaders.push_back(shader);

			auto info = vk::PipelineShaderStageCreateInfo()
				.setStage(kind2flagBits(shader.kind()))
				.setModule(shader.module())
				.setPName("main");

			_stageInfos.push_back(info);
		}

		auto stageInfos() const { return _stageInfos; }

		void deinit(vk::Device& dev)
		{
			for (auto& s : _shaders)
			{
				s.deinit(dev);
			}
			_shaders.clear();
		}
	};

}
