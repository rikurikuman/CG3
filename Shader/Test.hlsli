// �r���{�[�h�s��
cbuffer ConstBufferDataBillBoard : register(b0)
{
	matrix matBillBoard;
};

// �r���[&�ˉe�ϊ��s��
cbuffer ConstBufferDataViewProjection : register(b1)
{
	matrix matViewProjection;
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
	float4 pos : POSITION;// ���_���W
	float3 scale : SCALE;
	float4 color : COLOR;
};

// �W�I���g���V�F�[�_�[�̏o�͍\����
struct GSOutput
{
	float4 svpos : SV_POSITION;// �V�X�e���p���_���W
	float2 uv : TEXCOORD;// uv�l
	float4 color : COLOR;
};