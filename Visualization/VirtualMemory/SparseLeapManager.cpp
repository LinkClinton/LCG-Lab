#include "SparseLeapManager.hpp"

SparseLeapManager::SparseLeapManager(Factory* factory, int width, int height) :
	mFactory(factory), mWidth(width), mHeight(height)
{
}

void SparseLeapManager::initialize(const glm::vec3 &cube)
{
	mCube = cube;

	//create shader
	mOccupancyGeometryVertexShader = mFactory->createVertexShader(Helper::readFile("OccupancyGeometryVertexShader.cso"), true);
	mOccupancyGeometryPixelShader = mFactory->createPixelShader(Helper::readFile("OccupancyGeometryPixelShader.cso"), true);

	//create texture
	mOccupancyGeometryRenderTexture = mFactory->createTexture2D(mWidth, mHeight, PixelFormat::R8G8B8A8Unknown, ResourceInfo(BindUsage::RenderTargetUsage));

	auto resourceInfo = ResourceInfo((BindUsage)((unsigned int)BindUsage::ShaderResourceUsage | (unsigned int)BindUsage::UnorderedAccessUsage));

	mRaySegmentListCountTexture = mFactory->createTexture2D(mWidth, mHeight, PixelFormat::R32Uint, resourceInfo);
	mRaySegmentListDepthTexture = mFactory->createTexture3D(mWidth, mHeight, MAX_RAYSEGMENT_COUNT, PixelFormat::R32Float, resourceInfo);
	mRaySegmentListBoxTypeTexture = mFactory->createTexture3D(mWidth, mHeight, MAX_RAYSEGMENT_COUNT, PixelFormat::R8Uint, resourceInfo);
	mRaySegmentListEventTypeTexture = mFactory->createTexture3D(mWidth, mHeight, MAX_RAYSEGMENT_COUNT, PixelFormat::R8Uint, resourceInfo);

	mOccupancyGeometryRenderTarget = mFactory->createRenderTarget(mOccupancyGeometryRenderTexture, PixelFormat::R8G8B8A8Unknown);

	mRaySegmentListCountUAVUsage = mFactory->createUnorderedAccessUsage(mRaySegmentListCountTexture, PixelFormat::R32Uint);
	mRaySegmentListDepthUAVUsage = mFactory->createUnorderedAccessUsage(mRaySegmentListDepthTexture, PixelFormat::R32Float);
	mRaySegmentListBoxTypeUAVUsage = mFactory->createUnorderedAccessUsage(mRaySegmentListBoxTypeTexture, PixelFormat::R8Uint);
	mRaySegmentListEventTypeUAVUsage = mFactory->createUnorderedAccessUsage(mRaySegmentListEventTypeTexture, PixelFormat::R8Uint);

	mRaySegmentListCountSRVUsage = mFactory->createResourceUsage(mRaySegmentListCountTexture, PixelFormat::R32Uint);
	mRaySegmentListDepthSRVUsage = mFactory->createResourceUsage(mRaySegmentListDepthTexture, PixelFormat::R32Float);
	mRaySegmentListBoxTypeSRVUsage = mFactory->createResourceUsage(mRaySegmentListBoxTypeTexture, PixelFormat::R8Uint);
	mRaySegmentListEventTypeSRVUsage = mFactory->createResourceUsage(mRaySegmentListEventTypeTexture, PixelFormat::R8Uint);

	//create OccupancyHistogramTree
	mOccupancyHistogramTree = new OccupancyHistogramTree();

	mOccupancyHistogramTree->setMaxDepth(MAX_DEPTH);
	mOccupancyHistogramTree->setSize(AxiallyAlignedBoundingBox(-cube * 0.5f, cube * 0.5f));
}

void SparseLeapManager::finalize()
{
	mFactory->destoryVertexShader(mOccupancyGeometryVertexShader);
	mFactory->destoryPixelShader(mOccupancyGeometryPixelShader);

	mFactory->destoryTexture2D(mOccupancyGeometryRenderTexture);

	mFactory->destoryTexture2D(mRaySegmentListCountTexture);
	mFactory->destoryTexture3D(mRaySegmentListDepthTexture);
	mFactory->destoryTexture3D(mRaySegmentListBoxTypeTexture);
	mFactory->destoryTexture3D(mRaySegmentListEventTypeTexture);

	mFactory->destoryRenderTarget(mOccupancyGeometryRenderTarget);

	mFactory->destoryUnorderedAccessUsage(mRaySegmentListCountUAVUsage);
	mFactory->destoryUnorderedAccessUsage(mRaySegmentListDepthUAVUsage);
	mFactory->destoryUnorderedAccessUsage(mRaySegmentListBoxTypeUAVUsage);
	mFactory->destoryUnorderedAccessUsage(mRaySegmentListEventTypeUAVUsage);

	mFactory->destoryResourceUsage(mRaySegmentListCountSRVUsage);
	mFactory->destoryResourceUsage(mRaySegmentListDepthSRVUsage);
	mFactory->destoryResourceUsage(mRaySegmentListBoxTypeSRVUsage);
	mFactory->destoryResourceUsage(mRaySegmentListEventTypeSRVUsage);

	Utility::Delete(mOccupancyHistogramTree);
}

void SparseLeapManager::update(const glm::vec3& cameraPosition)
{
	mOccupancyGeometry.clear();

	//new occupancy geometry
	mOccupancyHistogramTree->setEyePosition(cameraPosition);
	mOccupancyHistogramTree->getOccupancyGeometry(mOccupancyGeometry);
}

auto SparseLeapManager::getCube() const -> glm::vec3
{
	return mCube;
}

auto SparseLeapManager::tree() -> OccupancyHistogramTree*
{
	return mOccupancyHistogramTree;
}

SparseLeapManager::~SparseLeapManager()
{
	
}
