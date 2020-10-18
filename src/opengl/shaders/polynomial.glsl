
#if defined(TWO_DIMENSIONS)
struct PolynomialCoefficients {
    int degree;
    highp float constant;
    highp vec2 linear;
    highp mat2x2 quadratic;
    highp mat2x2[2] cubic;
    highp vec2 center;
    highp float scale;
};

#else
struct PolynomialCoefficients {
    int degree;
    highp float constant;
    highp vec3 linear;
    highp mat3x3 quadratic;
    highp mat3x3[3] cubic;
    highp vec3 center;
    highp float scale;
};
#endif


uniform PolynomialCoefficients polynomial_coefficients;

#if defined(TWO_DIMENSIONS)
highp float polynomial_eval(highp vec2 p) {
#else
highp float polynomial_eval(highp vec3 p) {
#endif
    p = p - polynomial_coefficients.center;
    p = p / polynomial_coefficients.scale;
    highp float val = polynomial_coefficients.constant;
    // ret = C + p * (L + p * (Q + p * (U)))

#if defined(TWO_DIMENSIONS)
    highp mat2x2 Q = polynomial_coefficients.quadratic;
    highp vec2 L = polynomial_coefficients.linear;
#else
    highp mat3x3 Q = polynomial_coefficients.quadratic;
    highp vec3 L = polynomial_coefficients.linear;
#endif

    if(polynomial_coefficients.degree >= 3) {
        for(int j = 0; j < polynomial_coefficients.cubic.length(); ++j) {
            Q += polynomial_coefficients.cubic[j] * p[j];
        }
    }
    if(polynomial_coefficients.degree >= 2) {
        L += Q * p;
    }
    if(polynomial_coefficients.degree >= 1) {
        val += dot(L, p);
    }
    return val;
}

