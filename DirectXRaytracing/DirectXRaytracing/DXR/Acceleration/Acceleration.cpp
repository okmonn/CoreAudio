#include "Acceleration.h"
#include "../Device/Device.h"
#include "../List/List.h"
#include "../Matrix.h"
#include <d3d12.h>

// �R���X�g���N�^
Acceleration::Acceleration(std::weak_ptr<List>list, const DXR::AccelerationType& type, const size_t& instanceNum) : list(list),
	input(std::make_unique<D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS>()), instanceNum(instanceNum),buf(nullptr)
{
	rsc.resize(size_t(type));
}

// �f�X�g���N�^
Acceleration::~Acceleration()
{
	if (buf != nullptr)
	{
		UnMap(0);
		buf = nullptr;
	}
	for (auto& i : rsc)
	{
		if (i != nullptr)
		{
			i->Release();
			i = nullptr;
		}
	}
}

// �g�b�v���x���̐���
void Acceleration::CreateTop(const size_t& instanceNum, const size_t& rayNum, const Acceleration* bottom, const size_t& bottomNum, const Matrix3x4* initMat)
{
	size_t index = rsc.size() - 1;

	//�C���X�^���X���\�[�X�̐���
	D3D12_RESOURCE_DESC desc{};
	desc.DepthOrArraySize = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags            = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	desc.Format           = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	desc.Height           = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.MipLevels        = 1;
	desc.SampleDesc       = { 1, 0 };
	desc.Width            = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceNum;
	CreateRsc(&rsc[index--], UploadProp(), desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ);

	//�v���r���h���̎擾
	input->DescsLayout   = D3D12_ELEMENTS_LAYOUT::D3D12_ELEMENTS_LAYOUT_ARRAY;;
	input->Flags         = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS::D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	input->InstanceDescs = (*rsc.rbegin())->GetGPUVirtualAddress();
	input->NumDescs      = unsigned int(instanceNum);
	input->Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE::D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info{};
	Device::Get().Dev()->GetRaytracingAccelerationStructurePrebuildInfo(&(*input), &info);

	//�X�N���b�`���\�[�X�̐���
	desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Width = info.ScratchDataSizeInBytes;
	CreateRsc(&rsc[index--], DefaultProp(), desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//���U���g���\�[�X�̐���
	desc.Width = info.ResultDataMaxSizeInBytes;
	CreateRsc(&rsc[index], DefaultProp(), desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

	Map(index, (void**)&buf);
	for (size_t i = bottomNum; i < bottomNum; ++i)
	{
		for (size_t n = 0; n < bottom[i].instanceNum; ++i)
		{
			index = i * bottomNum + n;

			buf[index].AccelerationStructure = bottom[i].Result()->GetGPUVirtualAddress();
			buf[index].Flags = D3D12_RAYTRACING_INSTANCE_FLAGS::D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
			buf[index].InstanceContributionToHitGroupIndex = index * rayNum;
			buf[index].InstanceID = unsigned int(index);
			buf[index].InstanceMask = 0xff;
			std::memcpy(buf[index].Transform, initMat[index].mat, sizeof(Matrix3x4));
		}
	}

	Build();
}

// �{�g�����x���̐���
void Acceleration::CreateBottom(ID3D12Resource* vertex)
{
	size_t index = rsc.size() - 1;

	//�W�I���g�����
	D3D12_RAYTRACING_GEOMETRY_DESC geo{};
	geo.Flags = D3D12_RAYTRACING_GEOMETRY_FLAGS::D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	geo.Triangles.VertexBuffer.StartAddress  = vertex->GetGPUVirtualAddress();
	geo.Triangles.VertexBuffer.StrideInBytes = sizeof(Vec3f);
	geo.Triangles.VertexCount                = unsigned int(vertex->GetDesc().Width / geo.Triangles.VertexBuffer.StrideInBytes);
	geo.Triangles.VertexFormat               = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	//�v���r���h���̎擾
	input->DescsLayout    = D3D12_ELEMENTS_LAYOUT::D3D12_ELEMENTS_LAYOUT_ARRAY;
	input->Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS::D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	input->NumDescs       = 1;
	input->pGeometryDescs = &geo;
	input->Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE::D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info{};
	Device::Get().Dev()->GetRaytracingAccelerationStructurePrebuildInfo(&(*input), &info);

	//�X�N���b�`���\�[�X�̐���
	D3D12_RESOURCE_DESC desc{};
	desc.DepthOrArraySize = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags            = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Format           = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	desc.Height           = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.MipLevels        = 1;
	desc.SampleDesc       = { 1, 0 };
	desc.Width            = info.ScratchDataSizeInBytes;
	CreateRsc(&rsc[index--], DefaultProp(), desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//���U���g���\�[�X�̐���
	desc.Width = info.ResultDataMaxSizeInBytes;
	CreateRsc(&rsc[index], DefaultProp(), desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

	Build();
}

// �r���h
void Acceleration::Build(void)
{
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
	desc.DestAccelerationStructureData = rsc[0]->GetGPUVirtualAddress();
	desc.Inputs = *input;
	desc.ScratchAccelerationStructureData = rsc[1]->GetGPUVirtualAddress();
	list.lock()->AccelerationBuild(desc);

	list.lock()->Barrier(Result());
}

// �g�b�v���x���̍X�V
void Acceleration::UpData(const Matrix3x4* matrix)
{
	list.lock()->Barrier(Result());

	for (size_t i = 0; i < instanceNum; ++i)
	{
		std::memcpy(buf[i].Transform, matrix[i].mat, sizeof(Matrix3x4));
	}

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
	desc.DestAccelerationStructureData    = rsc[0]->GetGPUVirtualAddress();
	desc.Inputs                           = *input;
	desc.Inputs.Flags                    |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS::D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
	desc.Inputs.InstanceDescs             = rsc[2]->GetGPUVirtualAddress();
	desc.ScratchAccelerationStructureData = rsc[1]->GetGPUVirtualAddress();
	desc.SourceAccelerationStructureData  = rsc[0]->GetGPUVirtualAddress();
	list.lock()->AccelerationBuild(desc);

	list.lock()->Barrier(Result());
}

// ���U���g���\�[�X�̎擾
ID3D12Resource* Acceleration::Result(void) const
{
	return *rsc.rbegin();
}

// �X�N���b�`���\�[�X�̎擾
ID3D12Resource* Acceleration::Scratch(void) const
{
	return rsc[1];
}

// �C���X�^���X���\�[�X�̎擾
ID3D12Resource* Acceleration::Instance(void) const
{
	return *rsc.rbegin();
}

// �C���X�^���X���̎擾
size_t Acceleration::InstanceNum(void) const
{
	return instanceNum;
}
