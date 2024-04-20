#version 150 core

in vec3 texCoord;

uniform int ticks;

out vec4 outColor;

/* 				some info
 *
 *
 * we use spherical coordinates to plot sun, sunset/rise gradients and stars
 * inclination = theta [0:pi], theta = 0 <=> coord = (0, 0, 1), theta = pi <=> coord = (0, 0, -1)
 * azimuth = phi [-pi:pi], phi = 0 <=> coord = (1, 0), phi = -pi <=> coord = (-1, 0) <=> phi = pi
 *
 *
 * ticks 0 - 23999 == (06:00) - (05:59 next day)
 *
 * Internal sky light
 * level		up					down
 * 4  		to 4:18	22300						sky color is 0 0 0
 * 5  		to 4:28	22466		to 19:40	13670
 * 6  		to 4:38	22632		to 19:30	13504
 * 7  		to 4:48	22798		to 19:20	13338
 * 8  		to 4:58	22964		to 19:10	13172
 * 9  		to 5:08	23130		to 19:00	13006
 * 10 		to 5:18	23296		to 18:50	12840
 * 11 		to 5:28	23462		to 18:40	12674
 * 12 		to 5:38	23628		to 18:30	12508
 * 13 		to 5:48	23794		to 18:20	12342
 * 14 		to 5:58	23960		to 18:10	12176
 * 15 		to 18:00			12010			sky color is 120 169 255
 *
 *
 * sun wakes east at 0 ticks (06:00) and sleeps west at 12000 (18:00)
 * sun highest at 6000 ticks(12:00), lowest at 18000 ticks (00:00)
 * sun's y always == 0
 *
 * (y)
 * ^       north
 * |
 * |west         east
 * |
 * |       south
 *  -------------> (x)
*/

const float pi = 3.14159265358f;

const vec3 white = vec3(1, 1, 1);
const vec3 black = vec3(0, 0, 0);
const vec3 sky_day = vec3(120.0f / 255.0f, 169.0f / 255.0f, 1.0f);
const vec3 sky_night = black;

// const vec3 sun_wakes = vec3(1, 0, 0);
// const vec3 sun_highest = vec3(0, 0, 1.0f);
// const vec3 sun_sleeps = vec3(-1, 0, 0);
// const vec3 sun_lowest = vec3(0, 0, -1.0f);

void main()
{
	// outColor = vec4(1, 0, 0, 1);
	vec3 color;
	vec3 sun;
	if (ticks > 12000 && ticks < 13671) {
		color = mix(sky_day, sky_night, (ticks - 12010.0f) / (13670.0f - 12010));
	} else if (ticks > 22299) {
		color = mix(sky_night, sky_day, (ticks - 22299.0f) / (24000.0f - 22299));
	} else {
		color = (ticks < 12500) ? sky_day : sky_night;
	}
	// outColor = vec4(mix(white, color, texCoord.z * 4), 1.0f);
	// outColor = vec4(texCoord, 1.0f);
	outColor = vec4(color, 1.0f);

/* spherical coordinates will be used for sun and stars
	float r = length(texCoord);
	float inclination = acos(texCoord.z / r);
	float azimuth = sign(texCoord.y) * acos(texCoord.x / length(texCoord.xy));

	// float shade = inclination / pi;
	// float shade = (azimuth + pi) / (2.0f * pi);
	// outColor = vec4(shade, shade, shade, 1.0f);*/
}
