#pragma once

#include <WindowsFramework.hpp>
#include <Camera.hpp>
#include <Cube.hpp>

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "OccupancyHistogramTree.hpp"
#include "Helper.hpp"

//max instance count, because some problem , we can not use it now.
#define MAX_INSTANCE_PER_DRAW 1

//cube size
#define CUBE_SIZE glm::vec3(3, 3, 3)

//max depth
#define MAX_DEPTH 5

//max value that we can think it is empty
#define EMPTY_LIMIT 0.145

//max ray segment list count
#define MAX_RAYSEGMENT 30

/**
 * @brief framework for rendering
 */
class RenderFramework : public WindowsFramework {
private:
	VertexBuffer* mCubeVertexBuffer; //vertex buffer
	IndexBuffer* mCubeIndexBuffer; //index buffer
	ConstantBuffer* mMatrixBuffer; //matrix buffer
	ConstantBuffer* mInstanceBuffer; //instance buffer

	InputLayout* mInputLayout; //input layout

	RasterizerState* mRasterizerState; //rasterizer state
	DepthStencilState* mDepthStencilState; //depth state

	VertexShader* mOccupancyGeometryVertexShader; //generate Occupancy geometry shader
	VertexShader* mRayCastRenderingVertexShader; //ray-cast shader

	PixelShader* mOccupancyGeometryPixelShader; //generate Occupancy geometry shader
	PixelShader* mRayCastRenderingPixelShader; //ray-cast shader

	Texture2D* mOccupancyGeometryRenderTexture; //render target texture, it used in generate Occupancy geometry for debug

	Texture2D* mRaySegmentListCountTexture; //ray segment list count texture
	Texture3D* mRaySegmentListDepthTexture; //ray segment list depth texture
	Texture3D* mRaySegmentListBoxTypeTexture; //ray segment list occupancy type texture
	Texture3D* mRaySegmentListEventTypeTexture; //ray segment list event type texture
	
	Texture3D* mVolumeTexture; //volume data texture

	RenderTarget* mOccupancyGeometryRenderTarget; //render target
	
	UnorderedAccessUsage* mRaySegmentListCountUAVUsage; //ray segment list count texture view(RWTexture)
	UnorderedAccessUsage* mRaySegmentListDepthUAVUsage; //ray segment list depth texture view(RWTexture)
	UnorderedAccessUsage* mRaySegmentListBoxTypeUAVUsage; //ray segment list occupancy type texture view(RWTexture)
	UnorderedAccessUsage* mRaySegmentListEventTypeUAVUsage; //ray segment list event type texture view(RWTexture)

	ResourceUsage* mVolumeTextureUsage; //volume data texture view
	ResourceUsage* mRaySegmentListCountSRVUsage; //ray segment list count texture view
	ResourceUsage* mRaySegmentListDepthSRVUsage; //ray segment list depth texture view
	ResourceUsage* mRaySegmentListBoxTypeSRVUsage; //ray segment list occupancy type texture view
	ResourceUsage* mRaySegmentListEventTypeSRVUsage; //ray segment list event type texture view

	OccupancyHistogramTree* mOccupancyHistogramTree; //tree

	Camera mCamera; //camera

	glm::mat4 mMatrix[4]; //matrix data

	std::vector<unsigned char> mVolumeData; //volume data
	std::vector<InstanceData> mInstanceData; //instance data
	std::vector<OccupancyHistogramNodeCompareComponent> mOccupancyGeometry; //aabb node

	/**
	 * @brief update function
	 */
	virtual void update(void* sender)override;

	/**
	 * @brief render function
	 */
	virtual void render(void* sender)override;
	
	/**
	 * @brief render current instance
	 */
	void renderInstance(int requireInstanceCount);

	/**
	 * @generate ray segment list
	 */
	void renderRaySegmentList();

	/**
	 * @sample volume data, return the average value
	 */
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