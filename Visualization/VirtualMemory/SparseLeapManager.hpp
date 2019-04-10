#pragma once

#include <WindowsFramework.hpp>

#include "OccupancyHistogramTree.hpp"
#include "SharedTexture3D.hpp"
#include "Helper.hpp"

class SparseLeapManager {
private:
	Factory* mFactory;
	int mWidth;
	int mHeight;
	glm::vec3 mCube;

	VertexShader* mOccupancyGeometryVertexShader; //generate Occupancy geometry shader
	
	PixelShader* mOccupancyGeometryPixelShader; //generate Occupancy geometry shader
	
	Texture2D* mOccupancyGeometryRenderTexture; //render target texture, it used in generate Occupancy geometry for debug

	Texture2D* mRaySegmentListCountTexture; //ray segment list count texture
	Texture3D* mRaySegmentListDepthTexture; //ray segment list depth texture
	Texture3D* mRaySegmentListBoxTypeTexture; //ray segment list occupancy type texture
	Texture3D* mRaySegmentListEventTypeTexture; //ray segment list event type texture
	
	RenderTarget* mOccupancyGeometryRenderTarget; //render target

	UnorderedAccessUsage* mRaySegmentListCountUAVUsage; //ray segment list count texture view(RWTexture)
	UnorderedAccessUsage* mRaySegmentListDepthUAVUsage; //ray segment list depth texture view(RWTexture)
	UnorderedAccessUsage* mRaySegmentListBoxTypeUAVUsage; //ray segment list occupancy type texture view(RWTexture)
	UnorderedAccessUsage* mRaySegmentListEventTypeUAVUsage; //ray segment list event type texture view(RWTexture)

	ResourceUsage* mRaySegmentListCountSRVUsage; //ray segment list count texture view
	ResourceUsage* mRaySegmentListDepthSRVUsage; //ray segment list depth texture view
	ResourceUsage* mRaySegmentListBoxTypeSRVUsage; //ray segment list occupancy type texture view
	ResourceUsage* mRaySegmentListEventTypeSRVUsage; //ray segment list event type texture view

	OccupancyHistogramTree* mOccupancyHistogramTree; //tree

	std::vector<OccupancyHistogramNodeCompareComponent> mOccupancyGeometry; //aabb node

	friend class VMRenderFramework;
public:
	SparseLeapManager(Factory* factory, int width, int height);

	void initialize(const glm::vec3 &cube);

	void finalize();

	void update(const glm::vec3& cameraPosition);

	auto cube() const -> glm::vec3;

	auto tree() const -> OccupancyHistogramTree*;

	~SparseLeapManager();
};