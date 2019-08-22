#pragma once

#include <stdexcept>
#include <cassert>

#include "PixelFormat.hpp"
#include "PrimitiveType.hpp"
#include "FillMode.hpp"
#include "CullMode.hpp"
#include "ComparisonMode.hpp"
#include "BindUsage.hpp"
#include "HeapType.hpp"
#include "CpuAccessFlag.hpp"

#ifdef _WIN32

#include <d3d11.h>

#endif

typedef unsigned char codebyte;

class Utility {
public:
	template<typename T>
	static void dispose(T &object) {
		if (object != nullptr)
			object->Release();

		object = nullptr;
	}

	template<typename T>
	static void Delete(T &object) {
		if (object != nullptr)
			delete object;

		object = nullptr;
	}

	template<typename T>
	static T clamp(T value, T min, T max) {
		if (value < min) return min;
		if (value > max) return max;

		return value;
	}

	template<typename T>
	static T sign(T value) {
		if (value < 0) return 0;
		return 1;
	}

	static auto computePixelFormatBytes(PixelFormat pixelFormat) -> int {
		switch (pixelFormat)
		{
		case PixelFormat::R8G8B8A8Unknown:
		case PixelFormat::R8G8B8A8Uint:
		case PixelFormat::D32Float:
		case PixelFormat::R32Float:
		case PixelFormat::R32Uint:
		case PixelFormat::R32TypeLess:
		case PixelFormat::R8G8B8A8TypeLess:
			return 4;
		case PixelFormat::R8Unknown:
		case PixelFormat::R8Uint:
			return 1;
		case PixelFormat::R32G32Uint:
			return 8;
		case PixelFormat::R32G32B32Float:
			return 12;
		default:
			std::runtime_error("The pixel format is not supported.");
		}

		return 0;
	}

	static bool hasBindUsage(BindUsage usage, BindUsage requirement) {
		if ((int(usage) & int(requirement)) != 0) return true;
		return false;
	}

	static bool hasCpuAccessFlag(CpuAccessFlag flag, CpuAccessFlag requirement) {
		if ((int(flag) & int(requirement)) != 0) return true;
		return false;
	}

#ifdef _WIN32

	static auto convertPixelFormat(PixelFormat pixelFormat) -> DXGI_FORMAT {
		switch (pixelFormat)
		{
		case PixelFormat::R8G8B8A8Unknown:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		case PixelFormat::R8G8B8A8Uint:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT;
		case PixelFormat::R8Unknown:
			return DXGI_FORMAT::DXGI_FORMAT_R8_UNORM;
		case PixelFormat::D32Float:
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		case PixelFormat::R32Float:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		case PixelFormat::R32TypeLess:
			return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		case PixelFormat::R8G8B8A8TypeLess:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case PixelFormat::R8Uint:
			return DXGI_FORMAT::DXGI_FORMAT_R8_UINT;
		case PixelFormat::R32Uint:
			return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
		case PixelFormat::R32G32Uint:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT;
		case PixelFormat::R32G32B32Float:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		case PixelFormat::Unknown:
			return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		default:
			std::runtime_error("The pixel format is not supported.");
		}

		return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	}

	static auto convertPrimitiveType(PrimitiveType primitiveType) -> D3D11_PRIMITIVE_TOPOLOGY {
		switch (primitiveType)
		{
		case PrimitiveType::TriangleList:
			return D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		default:
			std::runtime_error("The primitive type is not supported.");
		}

		return D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	static auto convertFillMode(FillMode fillMode) -> D3D11_FILL_MODE {
		switch (fillMode)
		{
		case FillMode::Wireframe:
			return D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		case FillMode::Solid:
			return D3D11_FILL_MODE::D3D11_FILL_SOLID;
		default:
			std::runtime_error("The fill mode is not supported.");
		}

		return D3D11_FILL_MODE::D3D11_FILL_SOLID;
	}

	static auto convertCullMode(CullMode cullMode) -> D3D11_CULL_MODE {
		switch (cullMode)
		{
		case CullMode::None:
			return D3D11_CULL_MODE::D3D11_CULL_NONE;
		case CullMode::Front:
			return D3D11_CULL_MODE::D3D11_CULL_FRONT;
		case CullMode::Back:
			return D3D11_CULL_MODE::D3D11_CULL_BACK;
		default:
			std::runtime_error("The cull mode is not supported.");
		}

		return D3D11_CULL_MODE::D3D11_CULL_NONE;
	}

	static auto convertComparisonMode(ComparisonMode comparisonMode) -> D3D11_COMPARISON_FUNC {
		switch (comparisonMode)
		{
		case ComparisonMode::Never:
			return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		case ComparisonMode::Less:
			return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		case ComparisonMode::Equal:
			return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
		case ComparisonMode::LessEqual:
			return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		case ComparisonMode::Greater:
			return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;
		case ComparisonMode::NotEqual:
			return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;
		case ComparisonMode::GreaterEqual:
			return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;
		case ComparisonMode::Always:
			return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		default:
			std::runtime_error("The comparison mode is not supported.");
		}
		
		return D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	}

	static auto convertBindUsage(BindUsage bindUsage) -> D3D11_BIND_FLAG {
		static BindUsage bindUsagePool[] = {
			BindUsage::VertexBufferUsage, BindUsage::IndexBufferUsage,
			BindUsage::ConstantBufferUsage, BindUsage::ShaderResourceUsage,
			BindUsage::StreamOutputUsage, BindUsage::RenderTargetUsage,
			BindUsage::DepthStencilUsage, BindUsage::UnorderedAccessUsage,
			BindUsage::DecoderUsage, BindUsage::VideoEncoderUsage
		};

		static D3D11_BIND_FLAG bindFlagPool[] = {
			D3D11_BIND_VERTEX_BUFFER, D3D11_BIND_INDEX_BUFFER,
			D3D11_BIND_CONSTANT_BUFFER, D3D11_BIND_SHADER_RESOURCE,
			D3D11_BIND_STREAM_OUTPUT, D3D11_BIND_RENDER_TARGET,
			D3D11_BIND_DEPTH_STENCIL, D3D11_BIND_UNORDERED_ACCESS,
			D3D11_BIND_DECODER, D3D11_BIND_VIDEO_ENCODER
		};

		D3D11_BIND_FLAG result = (D3D11_BIND_FLAG)0;

		auto arraySize = sizeof(bindUsagePool) / sizeof(BindUsage);

		for (size_t i = 0; i < arraySize; i++) {
			if (hasBindUsage(bindUsage, bindUsagePool[i]) == true)
				result = D3D11_BIND_FLAG(result | bindFlagPool[i]);
		}

		return result;
	}

	static auto convertHeapType(HeapType heapType) -> D3D11_USAGE {
		switch (heapType)
		{
		case HeapType::Default:
			return D3D11_USAGE::D3D11_USAGE_DEFAULT;
		case HeapType::Immutable:
			return D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
		case HeapType::Dynamic:
			return D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		case HeapType::Staging:
			return D3D11_USAGE::D3D11_USAGE_STAGING;
		default:
			std::runtime_error("The heap type is not support.");
		}

		return D3D11_USAGE::D3D11_USAGE_DEFAULT;
	}

	static auto convertCpuAccessFlag(CpuAccessFlag cpuAccess) -> D3D11_CPU_ACCESS_FLAG {
		D3D11_CPU_ACCESS_FLAG result = (D3D11_CPU_ACCESS_FLAG)0;

		if (hasCpuAccessFlag(cpuAccess, CpuAccessFlag::Read)) result = D3D11_CPU_ACCESS_FLAG(result | D3D11_CPU_ACCESS_READ);
		if (hasCpuAccessFlag(cpuAccess, CpuAccessFlag::Write)) result = D3D11_CPU_ACCESS_FLAG(result | D3D11_CPU_ACCESS_WRITE);

		return result;
	}

	static auto convertCpuAccesssFlagToMapType(CpuAccessFlag cpuAccess) -> D3D11_MAP {

		if (hasCpuAccessFlag(cpuAccess, CpuAccessFlag::Read) == true &&
			hasCpuAccessFlag(cpuAccess, CpuAccessFlag::Write) == true) return D3D11_MAP::D3D11_MAP_READ_WRITE;

		if (hasCpuAccessFlag(cpuAccess, CpuAccessFlag::Read) == true) return D3D11_MAP::D3D11_MAP_READ;
		if (hasCpuAccessFlag(cpuAccess, CpuAccessFlag::Write) == true) return D3D11_MAP::D3D11_MAP_WRITE;

		return (D3D11_MAP)0;
	}
#endif // _WIN32

};
