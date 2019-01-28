#pragma once

#include "BindUsage.hpp"
#include "HeapType.hpp"
#include "CpuAccessFlag.hpp"

struct ResourceInfo {
	CpuAccessFlag CpuAccessFlag;
	BindUsage BindUsage;
	HeapType HeapType;
	
	ResourceInfo(::BindUsage bindUsage, 
		::CpuAccessFlag cpuAccessFlag = CpuAccessFlag::None, 
		::HeapType heapType = HeapType::Default)
		: BindUsage(bindUsage), CpuAccessFlag(cpuAccessFlag), HeapType(heapType) {}

	static auto ConstantBuffer(
		::CpuAccessFlag cpuAccessFlag = CpuAccessFlag::None,
		::HeapType heapType = HeapType::Default) {
		return ResourceInfo(BindUsage::ConstantBufferUsage, cpuAccessFlag, heapType);
	}

	static auto VertexBuffer(
		::CpuAccessFlag cpuAccessFlag = CpuAccessFlag::None,
		::HeapType heapType = HeapType::Default) {
		return ResourceInfo(BindUsage::VertexBufferUsage, cpuAccessFlag, heapType);
	}

	static auto IndexBuffer(
		::CpuAccessFlag cpuAccessFlag = CpuAccessFlag::None,
		::HeapType heapType = HeapType::Default) {
		return ResourceInfo(BindUsage::IndexBufferUsage, cpuAccessFlag, heapType);
	}

	static auto ShaderResource(
		::CpuAccessFlag cpuAccessFlag = CpuAccessFlag::None,
		::HeapType heapType = HeapType::Default) {
		return ResourceInfo(BindUsage::ShaderResourceUsage, cpuAccessFlag, heapType);
	}

	static auto UnorderedAccess(
		::CpuAccessFlag cpuAccessFlag = CpuAccessFlag::None,
		::HeapType heapType = HeapType::Default) {
		return ResourceInfo(BindUsage::UnorderedAccessUsage, cpuAccessFlag, heapType);
	}
};