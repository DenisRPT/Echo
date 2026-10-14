#version 330 core

in vec2 vWorld;
out vec4 FragColor;

uniform sampler2D uMap; // map texture
uniform vec4 uPulses[8]; // xy origin , z = age , w = strength
uniform int uPulseCount;
uniform float uSpeed;
uniform float uMaxRadius;

const int MAX_STEPS = 48;

bool isWall(ivec2 c){
	ivec2 size = textureSize(uMap, 0);
	if(c.x < 0 || c.y < 0 || c.x >= size.x || c.y >= size.y) return true; // out of bounds is wall
	return texelFetch(uMap,c,0).r > 0.5;
}

bool lineOfSight(vec2 from , vec2 to){
	ivec2 cell = ivec2(floor(from));
	ivec2 target = ivec2(floor(to));
	if(cell == target) return true;

	vec2 d = to - from;
	d.x = abs(d.x) < 1e-6 ? (d.x < 0.0 ? -1e-6 : 1e-6) : d.x;
	d.y = abs(d.y) < 1e-6 ? (d.y < 0.0 ? -1e-6 : 1e-6) : d.y;

	ivec2 stepI = ivec2(sign(d));
	vec2 tDelta = abs(1.0/d);
	vec2 tMax = (vec2(cell) + max(sign(d),0.0) - from) / d;

	for(int i=0; i<MAX_STEPS; ++i){
		if(tMax.x < tMax.y){
		   cell.x += stepI.x;
		   tMax.x += tDelta.x;
		} else {
		   cell.y += stepI.y;
		   tMax.y += tDelta.y;
		}
		if(isWall(cell)) return false;
		if(cell == target) return true;
	}
	return false;
}


void main() {
    ivec2 tile = ivec2(floor(vWorld));
    bool  wall = isWall(tile);

    // walls remain brghter , floors fade faster
    vec3  tint = wall ? vec3(0.30, 0.85, 1.00) : vec3(0.08, 0.30, 0.80);
    float decay = wall ? 2.0 : 6.0;
    float floorGain = wall ? 1.0 : 0.30;

    float light = 0.0;
    for (int i = 0; i < uPulseCount; ++i) {
        vec4  p = uPulses[i];
        float radius = p.z * uSpeed;  // current radius of the pulse
        float dist = distance(vWorld, p.xy);
        float behind = radius - dist;  // how far behind the pulse is this fragment
        if (behind < 0.0)
            continue;  //not yet reached this fragment

        float since = behind / uSpeed;
        float range = 1.0 - clamp(dist / uMaxRadius, 0.0, 1.0);
        float trail = exp(-since * decay) * range * p.w;
        if (trail < 0.02)
            continue;  // too dim to matter

        if (!lineOfSight(p.xy, vWorld))
            continue;  // blocked by wall

        float rise = smoothstep(0.0, 0.25, behind); 
        float rd   = behind - 0.35;
        float ring = exp(-rd * rd * 20.0); // ring is a gaussian around the pulse front
        light += rise * (0.5 * trail + ring * range * p.w);
    }

   
    vec2 f = fract(vWorld);
    float edge = min(min(f.x, 1.0 - f.x), min(f.y, 1.0 - f.y));
    float shade = wall ? (0.6 + 0.4 * smoothstep(0.0, 0.12, edge)) : 1.0;

    vec3 col = vec3(0.01, 0.01, 0.02) + tint * (light * floorGain) * shade;
    FragColor = vec4(col, 1.0);
}