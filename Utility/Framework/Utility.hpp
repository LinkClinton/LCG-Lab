#pragma once

#include <stdexcept>

#include "PixelFormat.hpp"
#include "PrimitiveType.hpp"
#include "FillMode.hpp"
#include "CullMode.hpp"
#include "ComparisonMode.hpp"
#include "BindUsage.hpp"
#include "HeapType.hpp"

#ifdef _WIN32

#include <d3d11.h>

#endif

typedef unsigned char codebyte;

class Utility {
public:
	template<typename T>
	static void Dispose(T &object) {
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

	static auto ComputePixelFomratBytes(PixelFormat pixelFormat) -> int {
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
		default:
			std::runtime_error("The pixel format is not supported.");
		}

		return 0;
	}

#ifdef _WIN32

	static auto ConvertPixelFormat(PixelFormat pixelFormat) -> DXGI_FORMAT {
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
		default:
			std::runtime_error("The pixel format is not supported.");
		}

		return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	}

	static auto ConvertPrimitiveType(PrimitiveType primitiveType) -> D3D11_PRIMITIVE_TOPOLOGY {
		switch (primitiveType)
		{
		case PrimitiveType::TriangleList:
			return D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		default:
			std::runtime_error("The primitive type is not supported.");
		}

		return D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	static auto ConvertFillMode(FillMode fillMode) -> D3D11_FILL_MODE {
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

	static auto ConvertCullMode(CullMode cullMode) -> D3D11_CULL_MODE {
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

	static auto ConvertComparisonMode(ComparisonMode comparisonMode) -> D3D11_COMPARISON_FUNC {
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

	static auto ConvertBindUsage(BindUsage bindUsage) -> D3D11_BIND_FLAG {
		switch (bindUsage)
		{
		case BindUsage::NoneUsage:
			return (D3D11_BIND_FLAG)0;
		case BindUsage::DepthStencilUsage:
			return D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		case BindUsage::RenderTargetUsage:
			return D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
		default:
			std::runtime_error("The bind usage is not supported.");
		};

		return (D3D11_BIND_FLAG)0;
	}

	static auto ConvertHeapType(HeapType heapType) -> D3D11_USAGE {
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
#endif // _WIN32

};
