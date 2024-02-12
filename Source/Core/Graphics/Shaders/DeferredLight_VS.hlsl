#include "deferred_common.hlsli"
#include "common.hlsli"


DeferredVertexToPixel main(DeferredVertexInput aInput)
{
    DeferredVertexToPixel output;

    output.position = aInput.position;

	return output;
}