#pragma once

enum class BindUsage : unsigned int {
	None = 0,
	VertexBufferUsage = 0x1L,
	IndexBufferUsage = 0x2L,
	ConstantBufferUsage = 0x4L,
	ShaderResourceUsage = 0x8L,
	StreamOutputUsage = 0x10L,
	RenderTargetUsage = 0x20L,
	DepthStencilUsage = 0x40L,
	UnorderedAccessUsage = 0x80L,
	DecoderUsage = 0x200L,
	VideoEncoderUsage = 0x400L
};
