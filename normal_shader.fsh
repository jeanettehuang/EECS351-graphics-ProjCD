#ifdef GL_ES
    precision mediump float;
#endif

varying vec3 N, v;
uniform int start_l, end_l;

void main (void) {

   vec3 N = normalize(N);
   vec4 shaded = vec4(0.0, 0.0, 0.0, 0.0);

   for (int i=start_l;i<=end_l;i++) {
      vec3 L = normalize(gl_LightSource[i].position.xyz - v);
      vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
      vec3 R = normalize(-reflect(L,N));

      // Ambient
      vec4 ambient = gl_FrontLightProduct[i].ambient;

      // Diffuse
      vec4 diffuse = gl_FrontLightProduct[i].diffuse;
      diffuse *= max(dot(N, L), 0.0);

      // Specular
      vec4 Ispec = gl_FrontLightProduct[i].specular;
      Ispec *= pow(max(dot(R,E),0.0),0.5*gl_FrontMaterial.shininess);

      shaded += ambient + diffuse + Ispec;
    }

    // Find the total color
    gl_FragColor = shaded + gl_FrontLightModelProduct.sceneColor;
}
