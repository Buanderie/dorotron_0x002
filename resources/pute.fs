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
uniform vec2 resolution = vec2(800, 450);

const float offset = 0.003;

void main() 
{
	vec4 col = texture2D(texture0, fragTexCoord);
	{
			vec4 colsum = texture2D(texture0, vec2(fragTexCoord.x + offset, fragTexCoord.y)) +
			texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y - offset)) +
			texture2D(texture0, vec2(fragTexCoord.x - offset, fragTexCoord.y)) +
			texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y + offset));
			if (col.a > 0.0)
			{
				finalColor = col;
			}
			else
			{
				if( colsum.a > 0.001 )
				finalColor = vec4(0.3, 0.3, 0.3, 1.0);
			}
	}
}
