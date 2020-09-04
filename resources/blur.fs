#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

// NOTE: Render size values must be passed from code
const float renderWidth = 1280;
const float renderHeight = 720;

float offset[3] = float[](0.0, 2.3846153846, 10.2307692308);
float weight[3] = float[](0.4270270270, 0.6162162162, 0.1402702703);

// See: https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch40.html
const float sigma   = 10.0;             // Gaussian sigma
const int   support = int(sigma * 3.0); // int(sigma * 3.0) truncation
void main()
{
	vec2 iResolution = vec2( renderWidth, renderHeight );
    vec2 loc   = fragTexCoord;         // center pixel cooordinate
    vec2 dir   = vec2( 1.0 / iResolution.x, 0.0 ); // horiz=(1.0, 0.0), vert=(0.0, 1.0)
	vec4 acc   = vec4( 0.0 );                      // accumulator
	float norm = 0.0;
	for (int i = -support; i <= support; i++) {
		float coeff = exp(-0.5 * float(i) * float(i) / (sigma * sigma));
		acc += (texture(texture0, loc + float(i) * dir)) * coeff;
		norm += coeff;
	}
	acc *= 1.0/norm;                               // normalize for unity gain

    // Output to screen
    finalColor = acc;
}

