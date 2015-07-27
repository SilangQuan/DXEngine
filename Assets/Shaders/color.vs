///////////// 
// GLOBALS // 
//shader中使用的全局变量都在定义在const buffer中 
//这样shader编译后，这些变量放在gpu的const buffer中 
///////////// 
cbuffer MatrixBuffer 
{ 
    matrix worldMatrix; 
    matrix viewMatrix; 
    matrix projectionMatrix; 
};

////////////// 
// TYPEDEFS // 
//注意：POSITION, COLOR等是我们在定义顶点布局时定义的名字。 
////////////// 
struct VertexInputType 
{ 
    float4 position : POSITION; 
    float4 color : COLOR; 
};

struct PixelInputType 
{ 
    float4 position : SV_POSITION; //SV表示系统自动定义的格式。 
    float4 color : COLOR; 
};

//////////////////////////////////////////////////////////////////////////////// 
// Vertex Shader 
//////////////////////////////////////////////////////////////////////////////// 
PixelInputType ColorVertexShader(VertexInputType input) 
{ 
    PixelInputType output; 
   

    //顶点坐标扩展成四个分量，并设置为1，以便矩阵运算 
    input.position.w = 1.0f;

    // 乘以3个矩阵，得到clip空间的坐标。 
     output.position = mul(input.position, worldMatrix); 
   output.position = mul(output.position, viewMatrix); 
    output.position = mul(output.position, projectionMatrix); 
    
    //直接输出顶点的颜色(顶点之间的颜色，会在光栅化阶段采用插值的方式计算） 
    output.color = input.color; 
    
    return output; 
   // return input;
}