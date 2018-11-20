#pragma once

#include <WindowsFramework.hpp>
#include <Camera.hpp>

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "OccupancyHistogramTree.hpp"
#include "Helper.hpp"

#define MAX_INSTANCE_PER_DRAW 1

#define CUBE_SIZE glm::vec3(3, 3, 3)
#define MAX_DEPTH 5
#define EMPTY_LIMIT 0.145
#define MAX_RAYSEGMENT 30

class RenderFramework : public WindowsFramework {
private:
	VertexBuffer* mCubeVertexBuffer;
	IndexBuffer* mCubeIndexBuffer;
	ConstantBuffer* mMatrixBuffer;
	ConstantBuffer* mInstanceBuffer;

	InputLayout* mInputLayout;

	RasterizerState* mRasterizerState;
	DepthStencilState* mDepthStencilState;

	VertexShader* mOccupancyGeometryVertexShader;
	VertexShader* mRayCastRenderingVertexShader;

	PixelShader* mOccupancyGeometryPixelShader;
	PixelShader* mRayCastRenderingPixelShader;

	Texture2D* mOccupancyGeometryRenderTexture;

	Texture2D* mRaySegmentListCountTexture;
	Texture3D* mRaySegmentListDepthTexture;
	Texture3D* mRaySegmentListBoxTypeTexture;
	Texture3D* mRaySegmentListEventTypeTexture;
	
	Texture3D* mVolumeTexture;

	RenderTarget* mOccupancyGeometryRenderTarget;
	
	UnorderedAccessUsage* mRaySegmentListCountUAVUsage;
	UnorderedAccessUsage* mRaySegmentListDepthUAVUsage;
	UnorderedAccessUsage* mRaySegmentListBoxTypeUAVUsage;
	UnorderedAccessUsage* mRaySegmentListEventTypeUAVUsage;

	ResourceUsage* mVolumeTextureUsage;
	ResourceUsage* mRaySegmentListCountSRVUsage;
	ResourceUsage* mRaySegmentListDepthSRVUsage;
	ResourceUsage* mRaySegmentListBoxTypeSRVUsage;
	ResourceUsage* mRaySegmentListEventTypeSRVUsage;

	OccupancyHistogramTree* mOccupancyHistogramTree;

	Camera mCamera;

	glm::mat4 mMatrix[4];

	std::vector<unsigned char> mVolumeData;
	std::vector<InstanceData> mInstanceData;
	std::vector<OccupancyHistogramNodeCompareComponent> mOccupancyGeometry;

	virtual void update(void* sender)override;
	virtual void render(void* sender)override;
	
	void renderInstance(int requireInstanceCount);
	void renderRaySegmentList();

	static auto sampleVolumeData(AxiallyAlignedBoundingBox sampleBox,
		int width, int height, int depth, std::vector<unsigned char> &volumeData) -> float;

	void buildState();
	void buildCamera();
	void buildBuffer();
	void buildShader();
	void buildTexture();
	void buildVolumeData();
	void buildInputLayout();

	void destoryState();
	void destoryCamera();
	void destoryBuffer();
	void destoryShader();
	void destoryTexture();
	void destoryVolumeData();
	void destoryInputLayout();
public:
	RenderFramework(const std::string &name, int width, int height);

	~RenderFramework();

	void build();
};