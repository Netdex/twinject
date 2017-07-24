#include "stdafx.h"
#include "ID3D9Device_Wrapper.h"
#include "cdraw.h"

Direct3DDevice9Wrapper::Direct3DDevice9Wrapper(IDirect3DDevice9 *pDirect3DDevice9, Direct3D9Wrapper *pDirect3D9, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	Direct3DDevice9 = pDirect3DDevice9;
	Direct3D9 = pDirect3D9;
}

Direct3DDevice9Wrapper::~Direct3DDevice9Wrapper() {  }

HRESULT Direct3DDevice9Wrapper::QueryInterface(const IID &riid, void **ppvObj)
{
	HRESULT hRes = Direct3DDevice9->QueryInterface(riid, ppvObj);
	if (hRes == S_OK)
		*ppvObj = this;
	else
		*ppvObj = NULL;

	return hRes;
}
ULONG Direct3DDevice9Wrapper::AddRef()
{
	return Direct3DDevice9->AddRef();
}
ULONG Direct3DDevice9Wrapper::Release()
{
	return Direct3DDevice9->Release();
}
HRESULT Direct3DDevice9Wrapper::TestCooperativeLevel()
{
	return Direct3DDevice9->TestCooperativeLevel();
}
UINT Direct3DDevice9Wrapper::GetAvailableTextureMem()
{
	return Direct3DDevice9->GetAvailableTextureMem();
}
HRESULT Direct3DDevice9Wrapper::EvictManagedResources()
{
	return Direct3DDevice9->EvictManagedResources();
}
HRESULT Direct3DDevice9Wrapper::GetDirect3D(IDirect3D9 **ppD3D9)
{
	*ppD3D9 = Direct3D9;
	return D3D_OK;
}
HRESULT Direct3DDevice9Wrapper::GetDeviceCaps(D3DCAPS9* pCaps)
{
	return Direct3DDevice9->GetDeviceCaps(pCaps);
}
HRESULT Direct3DDevice9Wrapper::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	return Direct3DDevice9->GetDisplayMode(iSwapChain, pMode);
}
HRESULT Direct3DDevice9Wrapper::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters)
{
	return Direct3DDevice9->GetCreationParameters(pParameters);
}
HRESULT Direct3DDevice9Wrapper::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
	return Direct3DDevice9->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}
void Direct3DDevice9Wrapper::SetCursorPosition(int X, int Y, DWORD Flags)
{
	return Direct3DDevice9->SetCursorPosition(X, Y, Flags);
}
BOOL Direct3DDevice9Wrapper::ShowCursor(BOOL bShow)
{
	return Direct3DDevice9->ShowCursor(bShow);
}
HRESULT Direct3DDevice9Wrapper::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
	return Direct3DDevice9->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}
HRESULT Direct3DDevice9Wrapper::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
	return Direct3DDevice9->GetSwapChain(iSwapChain, pSwapChain);
}
UINT Direct3DDevice9Wrapper::GetNumberOfSwapChains()
{
	return Direct3DDevice9->GetNumberOfSwapChains();
}
HRESULT Direct3DDevice9Wrapper::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	return Direct3DDevice9->Reset(pPresentationParameters);
}
HRESULT Direct3DDevice9Wrapper::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	return Direct3DDevice9->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}
HRESULT Direct3DDevice9Wrapper::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	return Direct3DDevice9->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}
HRESULT Direct3DDevice9Wrapper::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	return Direct3DDevice9->GetRasterStatus(iSwapChain, pRasterStatus);
}
HRESULT Direct3DDevice9Wrapper::SetDialogBoxMode(BOOL bEnableDialogs)
{
	return Direct3DDevice9->SetDialogBoxMode(bEnableDialogs);
}
void Direct3DDevice9Wrapper::SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp)
{
	return Direct3DDevice9->SetGammaRamp(iSwapChain, Flags, pRamp);
}
void Direct3DDevice9Wrapper::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
	return Direct3DDevice9->GetGammaRamp(iSwapChain, pRamp);
}
HRESULT Direct3DDevice9Wrapper::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	return Direct3DDevice9->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}
HRESULT Direct3DDevice9Wrapper::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	return Direct3DDevice9->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}
HRESULT Direct3DDevice9Wrapper::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	return Direct3DDevice9->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}
HRESULT Direct3DDevice9Wrapper::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	return Direct3DDevice9->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}
HRESULT Direct3DDevice9Wrapper::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	return Direct3DDevice9->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}
HRESULT Direct3DDevice9Wrapper::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return Direct3DDevice9->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}
HRESULT Direct3DDevice9Wrapper::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return Direct3DDevice9->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}
HRESULT Direct3DDevice9Wrapper::UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestPoint)
{
	return Direct3DDevice9->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}
HRESULT Direct3DDevice9Wrapper::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	return Direct3DDevice9->UpdateTexture(pSourceTexture, pDestinationTexture);
}
HRESULT Direct3DDevice9Wrapper::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	return Direct3DDevice9->GetRenderTargetData(pRenderTarget, pDestSurface);
}
HRESULT Direct3DDevice9Wrapper::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	return Direct3DDevice9->GetFrontBufferData(iSwapChain, pDestSurface);
}
HRESULT Direct3DDevice9Wrapper::StretchRect(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	return Direct3DDevice9->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}
HRESULT Direct3DDevice9Wrapper::ColorFill(IDirect3DSurface9* pSurface, const RECT* pRect, D3DCOLOR color)
{
	return Direct3DDevice9->ColorFill(pSurface, pRect, color);
}
HRESULT Direct3DDevice9Wrapper::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return Direct3DDevice9->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}
HRESULT Direct3DDevice9Wrapper::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	return Direct3DDevice9->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}
HRESULT Direct3DDevice9Wrapper::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	return Direct3DDevice9->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}
HRESULT Direct3DDevice9Wrapper::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	return Direct3DDevice9->SetDepthStencilSurface(pNewZStencil);
}
HRESULT Direct3DDevice9Wrapper::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
	return Direct3DDevice9->GetDepthStencilSurface(ppZStencilSurface);
}
HRESULT Direct3DDevice9Wrapper::BeginScene()
{
	if(Direct3D9->Hook.BeginSceneHook)
		Direct3D9->Hook.BeginSceneHook(Direct3DDevice9);
	return Direct3DDevice9->BeginScene();
}
HRESULT Direct3DDevice9Wrapper::EndScene()
{
	if(Direct3D9->Hook.EndSceneHook)
		Direct3D9->Hook.EndSceneHook(Direct3DDevice9);
	return Direct3DDevice9->EndScene();
}
HRESULT Direct3DDevice9Wrapper::Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return Direct3DDevice9->Clear(Count, pRects, Flags, Color, Z, Stencil);
}
HRESULT Direct3DDevice9Wrapper::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix)
{
	return Direct3DDevice9->SetTransform(State, pMatrix);
}
HRESULT Direct3DDevice9Wrapper::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	return Direct3DDevice9->GetTransform(State, pMatrix);
}
HRESULT Direct3DDevice9Wrapper::MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix)
{
	return Direct3DDevice9->MultiplyTransform(State, pMatrix);
}
HRESULT Direct3DDevice9Wrapper::SetViewport(const D3DVIEWPORT9* pViewport)
{
	return Direct3DDevice9->SetViewport(pViewport);
}
HRESULT Direct3DDevice9Wrapper::GetViewport(D3DVIEWPORT9* pViewport)
{
	return Direct3DDevice9->GetViewport(pViewport);
}
HRESULT Direct3DDevice9Wrapper::SetMaterial(const D3DMATERIAL9* pMaterial)
{
	return Direct3DDevice9->SetMaterial(pMaterial);
}
HRESULT Direct3DDevice9Wrapper::GetMaterial(D3DMATERIAL9* pMaterial)
{
	return Direct3DDevice9->GetMaterial(pMaterial);
}
HRESULT Direct3DDevice9Wrapper::SetLight(DWORD Index, const D3DLIGHT9* pLight)
{
	return Direct3DDevice9->SetLight(Index, pLight);
}
HRESULT Direct3DDevice9Wrapper::GetLight(DWORD Index, D3DLIGHT9* pLight)
{
	return Direct3DDevice9->GetLight(Index, pLight);
}
HRESULT Direct3DDevice9Wrapper::LightEnable(DWORD Index, BOOL Enable)
{
	return Direct3DDevice9->LightEnable(Index, Enable);
}
HRESULT Direct3DDevice9Wrapper::GetLightEnable(DWORD Index, BOOL* pEnable)
{
	return Direct3DDevice9->GetLightEnable(Index, pEnable);
}
HRESULT Direct3DDevice9Wrapper::SetClipPlane(DWORD Index, const float* pPlane)
{
	return Direct3DDevice9->SetClipPlane(Index, pPlane);
}
HRESULT Direct3DDevice9Wrapper::GetClipPlane(DWORD Index, float* pPlane)
{
	return Direct3DDevice9->GetClipPlane(Index, pPlane);
}
HRESULT Direct3DDevice9Wrapper::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	return Direct3DDevice9->SetRenderState(State, Value);
}
HRESULT Direct3DDevice9Wrapper::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
	return Direct3DDevice9->GetRenderState(State, pValue);
}
HRESULT Direct3DDevice9Wrapper::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	return Direct3DDevice9->CreateStateBlock(Type, ppSB);
}
HRESULT Direct3DDevice9Wrapper::BeginStateBlock()
{
	return Direct3DDevice9->BeginStateBlock();
}
HRESULT Direct3DDevice9Wrapper::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	return Direct3DDevice9->EndStateBlock(ppSB);
}
HRESULT Direct3DDevice9Wrapper::SetClipStatus(const D3DCLIPSTATUS9* pClipStatus)
{
	return Direct3DDevice9->SetClipStatus(pClipStatus);
}
HRESULT Direct3DDevice9Wrapper::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
	return Direct3DDevice9->GetClipStatus(pClipStatus);
}
HRESULT Direct3DDevice9Wrapper::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
	return Direct3DDevice9->GetTexture(Stage, ppTexture);
}
HRESULT Direct3DDevice9Wrapper::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	return Direct3DDevice9->SetTexture(Stage, pTexture);
}
HRESULT Direct3DDevice9Wrapper::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	return Direct3DDevice9->GetTextureStageState(Stage, Type, pValue);
}
HRESULT Direct3DDevice9Wrapper::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	return Direct3DDevice9->SetTextureStageState(Stage, Type, Value);
}
HRESULT Direct3DDevice9Wrapper::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
	return Direct3DDevice9->GetSamplerState(Sampler, Type, pValue);
}
HRESULT Direct3DDevice9Wrapper::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	return Direct3DDevice9->SetSamplerState(Sampler, Type, Value);
}
HRESULT Direct3DDevice9Wrapper::ValidateDevice(DWORD* pNumPasses)
{
	return Direct3DDevice9->ValidateDevice(pNumPasses);
}
HRESULT Direct3DDevice9Wrapper::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries)
{
	return Direct3DDevice9->SetPaletteEntries(PaletteNumber, pEntries);
}
HRESULT Direct3DDevice9Wrapper::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
	return Direct3DDevice9->GetPaletteEntries(PaletteNumber, pEntries);
}
HRESULT Direct3DDevice9Wrapper::SetCurrentTexturePalette(UINT PaletteNumber)
{
	return Direct3DDevice9->SetCurrentTexturePalette(PaletteNumber);
}
HRESULT Direct3DDevice9Wrapper::GetCurrentTexturePalette(UINT* PaletteNumber)
{
	return Direct3DDevice9->GetCurrentTexturePalette(PaletteNumber);
}
HRESULT Direct3DDevice9Wrapper::SetScissorRect(const RECT* pRect)
{
	return Direct3DDevice9->SetScissorRect(pRect);
}
HRESULT Direct3DDevice9Wrapper::GetScissorRect(RECT* pRect)
{
	return Direct3DDevice9->GetScissorRect(pRect);
}
HRESULT Direct3DDevice9Wrapper::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	return Direct3DDevice9->SetSoftwareVertexProcessing(bSoftware);
}
BOOL Direct3DDevice9Wrapper::GetSoftwareVertexProcessing()
{
	return Direct3DDevice9->GetSoftwareVertexProcessing();
}
HRESULT Direct3DDevice9Wrapper::SetNPatchMode(float nSegments)
{
	return Direct3DDevice9->SetNPatchMode(nSegments);
}
float Direct3DDevice9Wrapper::GetNPatchMode()
{
	return Direct3DDevice9->GetNPatchMode();
}
HRESULT Direct3DDevice9Wrapper::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	return Direct3DDevice9->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}
HRESULT Direct3DDevice9Wrapper::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return Direct3DDevice9->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}
HRESULT Direct3DDevice9Wrapper::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return Direct3DDevice9->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}
HRESULT Direct3DDevice9Wrapper::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return Direct3DDevice9->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}
HRESULT Direct3DDevice9Wrapper::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
	return Direct3DDevice9->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}
HRESULT Direct3DDevice9Wrapper::CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
	return Direct3DDevice9->CreateVertexDeclaration(pVertexElements, ppDecl);
}
HRESULT Direct3DDevice9Wrapper::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	return Direct3DDevice9->SetVertexDeclaration(pDecl);
}
HRESULT Direct3DDevice9Wrapper::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	return Direct3DDevice9->GetVertexDeclaration(ppDecl);
}
HRESULT Direct3DDevice9Wrapper::SetFVF(DWORD FVF)
{
	return Direct3DDevice9->SetFVF(FVF);
}
HRESULT Direct3DDevice9Wrapper::GetFVF(DWORD* pFVF)
{
	return Direct3DDevice9->GetFVF(pFVF);
}
HRESULT Direct3DDevice9Wrapper::CreateVertexShader(const DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
	return Direct3DDevice9->CreateVertexShader(pFunction, ppShader);
}
HRESULT Direct3DDevice9Wrapper::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	return Direct3DDevice9->SetVertexShader(pShader);
}
HRESULT Direct3DDevice9Wrapper::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	return Direct3DDevice9->GetVertexShader(ppShader);
}
HRESULT Direct3DDevice9Wrapper::SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
	return Direct3DDevice9->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}
HRESULT Direct3DDevice9Wrapper::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return Direct3DDevice9->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}
HRESULT Direct3DDevice9Wrapper::SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{
	return Direct3DDevice9->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}
HRESULT Direct3DDevice9Wrapper::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return Direct3DDevice9->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}
HRESULT Direct3DDevice9Wrapper::SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{
	return Direct3DDevice9->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}
HRESULT Direct3DDevice9Wrapper::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return Direct3DDevice9->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}
HRESULT Direct3DDevice9Wrapper::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride)
{
	if (StreamNumber == 0)
		m_Stride = Stride;

	return Direct3DDevice9->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}
HRESULT Direct3DDevice9Wrapper::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
	return Direct3DDevice9->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}
HRESULT Direct3DDevice9Wrapper::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
	return Direct3DDevice9->SetStreamSourceFreq(StreamNumber, Setting);
}
HRESULT Direct3DDevice9Wrapper::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting)
{
	return Direct3DDevice9->GetStreamSourceFreq(StreamNumber, pSetting);
}
HRESULT Direct3DDevice9Wrapper::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	return Direct3DDevice9->SetIndices(pIndexData);
}
HRESULT Direct3DDevice9Wrapper::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
	return Direct3DDevice9->GetIndices(ppIndexData);
}
HRESULT Direct3DDevice9Wrapper::CreatePixelShader(const DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	return Direct3DDevice9->CreatePixelShader(pFunction, ppShader);
}
HRESULT Direct3DDevice9Wrapper::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	return Direct3DDevice9->SetPixelShader(pShader);
}
HRESULT Direct3DDevice9Wrapper::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
	return Direct3DDevice9->GetPixelShader(ppShader);
}
HRESULT Direct3DDevice9Wrapper::SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
	return Direct3DDevice9->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}
HRESULT Direct3DDevice9Wrapper::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return Direct3DDevice9->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}
HRESULT Direct3DDevice9Wrapper::SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{
	return Direct3DDevice9->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}
HRESULT Direct3DDevice9Wrapper::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return Direct3DDevice9->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}
HRESULT Direct3DDevice9Wrapper::SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{
	return Direct3DDevice9->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}
HRESULT Direct3DDevice9Wrapper::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return Direct3DDevice9->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}
HRESULT Direct3DDevice9Wrapper::DrawRectPatch(UINT Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo)
{
	return Direct3DDevice9->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}
HRESULT Direct3DDevice9Wrapper::DrawTriPatch(UINT Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo)
{
	return Direct3DDevice9->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}
HRESULT Direct3DDevice9Wrapper::DeletePatch(UINT Handle)
{
	return Direct3DDevice9->DeletePatch(Handle);
}
HRESULT Direct3DDevice9Wrapper::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	return Direct3DDevice9->CreateQuery(Type, ppQuery);
}