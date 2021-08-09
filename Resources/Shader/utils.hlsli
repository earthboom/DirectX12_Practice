
struct LightColor
{
    float4      diffuse;
    float4      ambient;
    float4      specular;
};

struct LightInfo
{
    LightColor	color;		
    float4		Position;	
    float4		direction;	
    int 		lightType;	
    float		range;		
    float		angle;		
    int 		padding;	
};

cbuffer GLOBAL_PARAMS : register(b0)
{
    int         g_lightCount;
    float3      g_lightPadding;
    LightInfo   g_light[50];
}

//row_major
// - DirectX에서 기본 행렬과 Shader 문법에서 행렬 접근 순서가 다르기 때문에,
// - 그 순서를 DirectX 방식으로 맞춰주기 위함.(row(행)을 기준으로 읽어나감)
cbuffer TRANSFORM_PARAMS : register(b1)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProjection;
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
};

cbuffer MATERIAL_PARAMS : register(b2)
{
    int     g_int_0;
    int     g_int_1;
    int     g_int_2;
    int     g_int_3;
    int     g_int_4;
    
    float   g_float_0;
    float   g_float_1;
    float   g_float_2;
    float   g_float_3;
    float   g_float_4;
};

// Texture를 받음.
Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);

SamplerState g_sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;   //sv가 붙으면 예약된 것임.
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.0f), matWVP);
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex_0.Sample(sam_0, input.uv);

    return color;
}