#version 150 core

in vec3 texCoord;

uniform int ticks;

out vec4 outColor;

/* 				some info
 *
 *
 * nevermind the following lines, we use cartesian coords
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
const float threepi2 = 3 * pi / 2;

const vec3 white = vec3(1, 1, 1);
const vec3 black = vec3(0, 0, 0);
const vec3 sunColor = vec3(1, 1, 0);
const vec3 sunsetColor = vec3 (1.0f, 0.3f, 0);
const vec3 sky_day = vec3(120.0f / 255.0f, 169.0f / 255.0f, 1.0f);
const vec3 sky_night = black;

// return (inclination, azimuth), both in range [0:1]
/*vec2 cartesianToSpherical( vec3 pos )
{
	float r = length(pos);
	float inclination = acos(pos.z / r);
	float azimuth = sign(pos.y) * acos(pos.x / length(pos.xy));

	inclination /= pi;
	azimuth /= pi;
	azimuth = (azimuth + 1) * 0.5f;

	return vec2(inclination, azimuth);
}*/

void main()
{
	// outColor = vec4(1, 0, 0, 1);

	vec3 skyColor;
	if (ticks > 12000 && ticks < 13671) {
		skyColor = mix(sky_day, sky_night, (ticks - 12010.0f) / (13670.0f - 12010));
	} else if (ticks > 22299) {
		skyColor = mix(sky_night, sky_day, (ticks - 22299.0f) / (24000.0f - 22299));
	} else {
		skyColor = (ticks < 12500) ? sky_day : sky_night;
	}

	/*/ spherical coordinates will be used for sun and stars
	vec2 sunPos = vec2(0.3f, 0.5f); // vec2(inclination, azimuth)
	if (ticks < 6000) {
		// 0 - 6000 azimuth = 0.5f, inclination = 0.5 - 0
		sunPos = vec2(0.5f - ticks / 12000.0f, 0.5f);
	} else if (ticks < 12000) {
		// 6000 - 12000 azimuth = 0, inclination = 0 - 0.5
		sunPos = vec2((ticks - 6000) / 12000.0f, 0);
	}
	vec2 texPos = cartesianToSpherical(texCoord);

	// skyColor = mix(skyColor, yellow, clamp(1 - 3 * distance(sunPos, vec2(texPos.x, texPos.y)), 0, 1));
	// skyColor = mix(skyColor, yellow, clamp(1 - 30 * distance(vec2(sunPos.x, sunPos.y * 0.5f), vec2(texPos.x, texPos.y * 0.5f)), 0, 1)); // fails close to pole
	skyColor = mix(skyColor, yellow, clamp(1 - 30 * (abs(sunPos.x - texPos.x) + abs(sunPos.y - texPos.y)), 0, 1)); // losange
	// skyColor = mix(skyColor, yellow, clamp(1 - 30 * (abs(sunPos.x - texPos.x) * abs(sunPos.y - texPos.y)), 0, 1)); // funny line
	// if (abs(sunPos.x - texPos.x) < 0.05 && abs(sunPos.y - texPos.y) < 0.1) {
	// 	skyColor = yellow;
	// }*/

	// float r = length(texCoord);
	// float inclination = acos(texCoord.z / r);
	// float azimuth = sign(texCoord.y) * acos(texCoord.x / length(texCoord.xy));

	// ticks -> (cos, 0, sin) = angle   <=> ticks / 12000 * pi = radians
	// 00000 -> ( 1,  0,   0) = 0
	// 06000 -> ( 0,  0,   1) = pi/2
	// 12000 -> (-1,  0,   0) = pi
	// 18000 -> ( 0,  0,  -1) = 3pi/2
	// 24000 -> ( 1,  0,   0) = 2pi
	float angle = ticks / 12000.0f * pi;
	vec3 sunPos = vec3(cos(angle), 0, sin(angle));
	vec3 moonPos = vec3(sunPos.x, 0, sin((clamp(angle, pi, 7) - 1) * threepi2 / (threepi2 - 1)));
	sunPos = normalize(sunPos);
	moonPos = normalize(moonPos);

	vec3 dir = normalize(texCoord);

	float sunAngle = dot(dir, sunPos);
	// skyColor = mix(sky_night, sky_day, (sunAngle + 1) * 0.5f);
	skyColor = mix(skyColor, sunColor, clamp(sunAngle * 10 - 9, 0, 1) * 0.3f); // dimmer light around sun
	skyColor = mix(skyColor, sunColor, clamp(sunAngle * 160 - 159, 0, 1));

	// float moonAngle = -sunAngle; // just do this for moon opposite from sun
	float moonAngle = -dot(dir, moonPos);
	skyColor = mix(skyColor, white, clamp(moonAngle * 20 - 19, 0, 1) * 0.1f);
	skyColor = mix(skyColor, white, clamp(moonAngle * 320 - 319, 0, 1));

	// compute skyColor from normalized sunPos and texCoord
	const float atmospherePow = 10.0f; // increase to reduce atmosphere height
    const float scatterPow = 0.25f; // increase to make sunset last longer

    float atmosphere = sqrt(pow(1.0f - abs(dir.z), atmospherePow));
    atmosphere = clamp(atmosphere / 1.3f, 0, 1);

    float scatter = 1 - pow(clamp(abs(sunPos.z) + 0.01f, 0, 1) * 2, scatterPow);
    vec3 scatterColor = mix(skyColor, sunsetColor * 1.5f, scatter);

    skyColor = mix(skyColor, scatterColor, atmosphere);

	// float shade = dir.z;
	// float shade = inclination / pi;
	// float shade = (azimuth + pi) / (2.0f * pi);
	// outColor = vec4(shade, shade, shade, 1.0f);
	// outColor = vec4(texCoord, 1.0f);

	outColor = vec4(skyColor, 1.0f);
}
