varying vec3 N;
varying vec3 v;
uniform float time;
varying float xs, zs;

void main() {

    xs = 1.0;
    zs = 1.0;

    N = normalize(gl_NormalMatrix * gl_Normal);
    v = vec3(gl_ModelViewMatrix * gl_Vertex);

    vec4 vPosition = vec4(gl_Vertex);
    vPosition.z = 0.2*sin(xs*time + 60.0*vPosition.z) + vPosition.z;

    gl_Position = gl_ModelViewProjectionMatrix*vPosition;
}


