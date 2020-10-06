
#if defined(TWO_DIMENSIONS)
struct PolynomialCoefficients {
    int degree;
    float constant;
    vec2 linear;
    mat2x2 quadratic;
    mat2x2[2] cubic;
    vec2 center;
    float scale;
};

#else
struct PolynomialCoefficients {
    int degree;
    float constant;
    vec3 linear;
    mat3x3 quadratic;
    mat3x3[3] cubic;
    vec2 center;
    float scale;
};
#endif


uniform PolynomialCoefficients polynomial_coefficients;

#if defined(TWO_DIMENSIONS)
float polynomial_eval(vec2 p) {
#else
float polynomial_eval(vec3 p) {
#endif
    p = p - polynomial_coefficients.center;
    p = p / polynomial_coefficients.scale;
    float val = polynomial_coefficients.constant;
    if(polynomial_coefficients.degree > 0) {

        val += dot(polynomial_coefficients.linear,p);
        if(polynomial_coefficients.degree > 1) {
            val += dot(p,polynomial_coefficients.quadratic*p);
            if(polynomial_coefficients.degree > 2) {
                for(int j = 0; j < polynomial_coefficients.cubic.length(); ++j) {
                    val += dot(p,polynomial_coefficients.cubic[j]*p) * p[j];
                }
            }
        }
    }
    return val;
}

