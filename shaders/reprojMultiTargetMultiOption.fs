#version 460 core
//layout(early_fragment_tests) in;
out vec4 color;

#define NUMTARGET 4
struct TargetTexture{
  sampler2D tDiffuse;
  sampler2D tDepth;
};
// caching
in vec4 targetPos[NUMTARGET];
uniform TargetTexture tts[NUMTARGET];
uniform float threshold;
uniform float epsilon;
uniform int renderOption;
uniform int usePhong;

// reshading
in vec3 vFragPos;
in vec3 vNormal;
in vec3 vLightPos;

uniform vec3 uDirLightColor;
uniform vec3 uAmbientLightColor;
uniform vec3 uMaterialColor;
uniform float uKd;

// noise
in vec3 v_texCoord3D;

layout(binding=0, offset=0) uniform atomic_uint ac_frag;
layout(binding=0, offset=4) uniform atomic_uint ac_frag2;

//---------------------------------- 3D Noise ----------------------------------//
vec3 mod289(vec3 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P)
{
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

vec3 noise_diffuse()
{
    float time = 1.0;
    vec3 v = vec3(2.0 * v_texCoord3D.xyz * (2.0 + sin(0.5*2.0)));  
    v = v * 10.0;
    float n = cnoise(v);
    // vec3 color = 0.5 + 0.3 * vec3(n, n, n);
    vec3 color = 0.8 + 0.1 * vec3(n, n, n);
    return color;
}

vec4 reShadingNoise()
{
  vec3 noise_color = noise_diffuse();
  vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
  
  color.xyz += uAmbientLightColor * noise_color;

  vec3 lVector = normalize(vLightPos); 
  vec3 normal = normalize(vNormal);
  float diffuse = max(dot(normal, lVector), 0.0);
  color.xyz += uKd * noise_color * uDirLightColor * diffuse * 1.5;
  return color;
}

vec4 reShadingPhong()
{
  // vec3 vFragPos = vec3(curModelView * vec4(vPos, 1.0));
  // vec3 vNormal = mat3(invTransModelView) * vaNormal;
  // vec3 vLightPos = vec3(curWorldView * vec4(lightPos, 1.0));

  vec4 color = vec4(uAmbientLightColor * uMaterialColor, 1.0);

  vec3 lVector = normalize(vLightPos); // check this lightDir, from camera to lightPos
  vec3 normal = normalize(vNormal);
  float diffuse = max(dot(normal, lVector), 0.0);
  color.xyz += uKd * uMaterialColor * uDirLightColor * diffuse;
  return color;
}

vec4 reShading()
{
  if(usePhong == 1){
    return reShadingPhong();
  }
  else{
    return reShadingNoise();
  }
}

//---------------------------------- Caching ----------------------------------//

float LinearizeDepth(float z)
{
  float n = 0.1f;
  float f = 100.0f;
  return (2.0*n)/(f+n -z*(f-n)); // convert from [-n, -f] to [n/f, 1.0]
}

vec4 reprojColor(vec4 targetPos, TargetTexture tts)
{
  vec3 targetPos_ndc = vec3(targetPos.x, targetPos.y, targetPos.z) / targetPos.w;
  vec2 targetCoord = (vec2(targetPos_ndc.x, targetPos_ndc.y) + vec2(1.0)) / 2.0;
  vec4 tempColor = texture2D(tts.tDiffuse, targetCoord);
  vec3 repColor = tempColor.rgb;
  //float repDepth = tempColor.a;
  float repDepth = texture2D(tts.tDepth, targetCoord).r;

  float fragDepth = (targetPos_ndc.z + 1.0) / 2.0;
  repDepth = LinearizeDepth(repDepth);
  fragDepth = LinearizeDepth(fragDepth);
  float repDiff = repDepth - fragDepth;
  return vec4(repColor, repDiff);
}

void main()
{
  vec3 repColor[NUMTARGET];
  float repDiff[NUMTARGET];
  for(int i=0; i<NUMTARGET; i++){
    vec4 temp = reprojColor(targetPos[i], tts[i]);
    repColor[i] = temp.rgb;
    repDiff[i] = temp.a;
  }

  if(renderOption == 1){
    if(repDiff[0] < threshold && repDiff[0] > epsilon){
      color = vec4(repColor[0], 1.0);
    }
    else if(repDiff[0] >= threshold){
      discard;
    } 
    else{
      color = reShading();
      // color = vec4(1.0, 0.0, 0.0, 1.0);
    }
  }
  else if(renderOption == 0){
    if(repDiff[1] < threshold && repDiff[1] > epsilon){
      color = vec4(repColor[1], 1.0);
    }
    else if(repDiff[1] >= threshold){
      discard;
    }
    else{
      color = reShading();
      // color = vec4(0.0, 0.0, 1.0, 1.0);
    }
  }
  else if(renderOption == 2){
    if(repDiff[0] < threshold && repDiff[0] > epsilon){
      color = vec4(repColor[0], 1.0);
    }
    else if(repDiff[1] < threshold && repDiff[1] > epsilon){
      color = vec4(repColor[1], 1.0);
    }
    else if(repDiff[0] >= threshold || repDiff[1] >= threshold){
      discard;
    }
    else{
      color = reShading();
    }
  }
  else if(renderOption == 6){
    if(repDiff[0] >= threshold || repDiff[1] >= threshold){
      discard;
    }
    else if(repDiff[0] < threshold && repDiff[0] > epsilon){
      color = vec4(repColor[0], 1.0);
    }
    else if(repDiff[1] < threshold && repDiff[1] > epsilon){
      color = vec4(repColor[1], 1.0);
    }
    else{
      color = reShading();
    }
  }
  else if(renderOption == 5){
    if(repDiff[1] < threshold && repDiff[1] > epsilon){
      color = vec4(repColor[1], 1.0);
    }
    else if(repDiff[0] < threshold && repDiff[0] > epsilon){
      color = vec4(repColor[0], 1.0);
    }
    else if(repDiff[0] >= threshold || repDiff[1] >= threshold){
      discard;
    }
    else{
      color = reShading();
    }
  }
  else if(renderOption == 3){
    if(repDiff[0] < threshold && repDiff[0] > epsilon){
      color = vec4(repColor[0], 1.0);
    }
    else if(repDiff[1] < threshold && repDiff[1] > epsilon){
      color = vec4(repColor[1], 1.0);
    }
    else if(repDiff[2] < threshold && repDiff[2] > epsilon){
      color = vec4(repColor[2], 1.0);
    }
    else if(repDiff[0] >= threshold || repDiff[1] >= threshold || repDiff[2] >= threshold){
      discard;
    }
    else{
      color= reShading();
    }
  }
  else if(renderOption == 4){
    if(repDiff[0] < threshold && repDiff[0] > epsilon){
      color = vec4(repColor[0], 1.0);
    }
    else if(repDiff[1] < threshold && repDiff[1] > epsilon){
      color = vec4(repColor[1], 1.0);
    }
    else if(repDiff[2] < threshold && repDiff[2] > epsilon){
      color = vec4(repColor[2], 1.0);
    }
    else if(repDiff[3] < threshold && repDiff[3] > epsilon){
      color = vec4(repColor[3], 1.0);
    }
    else if(repDiff[0] >= threshold || repDiff[1] >= threshold || repDiff[2] >= threshold || repDiff[3] >= threshold){
      discard;
    }
    else{
      color= reShading();
    }
  }
}
