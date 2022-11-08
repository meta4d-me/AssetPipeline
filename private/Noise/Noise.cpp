#include "Noise.h"

// Adopted from https://github.com/KdotJPG/OpenSimplex2/blob/master/java/OpenSimplex2S.java
namespace Details {

    constexpr int64_t PRIME_X = 0x5205402B9270C86FL;
    constexpr int64_t PRIME_Y = 0x598CD327003817B5L;
    constexpr int64_t PRIME_Z = 0x5BCC226E9FA0BACBL;
    constexpr int64_t PRIME_W = 0x56CC5227E58F554BL;
    constexpr int64_t HASH_MULTIPLIER = 0x53A3F72DEEC546F5L;
    constexpr int64_t SEED_FLIP_3D = -0x52D547B2E96ED629L;
    constexpr int64_t SEED_OFFSET_4D = 0xE83DC3E0DA7164DL;

    constexpr double ROOT2OVER2 = 0.7071067811865476;
    constexpr double SKEW_2D = 0.366025403784439;
    constexpr double UNSKEW_2D = -0.21132486540518713;

    constexpr int32_t N_GRADS_2D_EXPONENT = 7;
    constexpr int32_t N_GRADS_2D = 1 << N_GRADS_2D_EXPONENT;
    constexpr double NORMALIZER_2D = 0.01001634121365712;
    constexpr float RSQUARED_2D = 0.5f;

    constexpr int32_t GRADIENTS_SEED_LENGTH = 48;
    float GRADIENTS_SEED[GRADIENTS_SEED_LENGTH] = {
        0.38268343236509f,   0.923879532511287f,
        0.923879532511287f,  0.38268343236509f,
        0.923879532511287f, -0.38268343236509f,
        0.38268343236509f,  -0.923879532511287f,
        -0.38268343236509f,  -0.923879532511287f,
        -0.923879532511287f, -0.38268343236509f,
        -0.923879532511287f,  0.38268343236509f,
        -0.38268343236509f,   0.923879532511287f,
        //-------------------------------------//
        0.130526192220052f,  0.99144486137381f,
        0.608761429008721f,  0.793353340291235f,
        0.793353340291235f,  0.608761429008721f,
        0.99144486137381f,   0.130526192220051f,
        0.99144486137381f,  -0.130526192220051f,
        0.793353340291235f, -0.60876142900872f,
        0.608761429008721f, -0.793353340291235f,
        0.130526192220052f, -0.99144486137381f,
        -0.130526192220052f, -0.99144486137381f,
        -0.608761429008721f, -0.793353340291235f,
        -0.793353340291235f, -0.608761429008721f,
        -0.99144486137381f,  -0.130526192220052f,
        -0.99144486137381f,   0.130526192220051f,
        -0.793353340291235f,  0.608761429008721f,
        -0.608761429008721f,  0.793353340291235f,
        -0.130526192220052f,  0.99144486137381f,
    };
    constexpr int32_t GRADIENTS_2D_LENGTH = N_GRADS_2D * 2;
    float GRADIENTS_2D[GRADIENTS_2D_LENGTH];

    float* InitializeGradients2D() {
        for (int32_t i = 0; i < GRADIENTS_SEED_LENGTH; ++i) {
            GRADIENTS_SEED[i] = static_cast<float>(GRADIENTS_SEED[i] / NORMALIZER_2D);
        }
        for (int32_t i = 0, j = 0; i < GRADIENTS_2D_LENGTH; ++i, ++j) {
            if (j == GRADIENTS_SEED_LENGTH) j = 0;
            GRADIENTS_2D[i] = GRADIENTS_SEED[j];
        }
        return GRADIENTS_2D;
    }
    float* Gradien2D = InitializeGradients2D();

    int32_t FastFloor(double i) {
        int32_t iAsInt = static_cast<int32_t>(i);
        return i < iAsInt ? iAsInt - 1 : iAsInt;
    }	// namespace Details

    int32_t FastRound(double i) {
        return i < 0 ? static_cast<int32_t>(i - 0.5) : static_cast<int32_t>(i + 0.5);
    }

    float Gradient(int64_t seed, int64_t xsvp, int64_t ysvp, float dx, float dy) {
        int64_t hash = seed ^ xsvp ^ ysvp;
        hash *= HASH_MULTIPLIER;
        hash ^= hash >> (64 - N_GRADS_2D_EXPONENT + 1);
        int32_t gi = static_cast<int32_t>(hash) & ((N_GRADS_2D - 1) << 1);
        return GRADIENTS_2D[gi | 0] * dx + GRADIENTS_2D[gi | 1] * dy;
    }

    /**
     * 2D Simplex noise base.
     */
    float Noise2D_UnskewedBase(int64_t seed, double xs, double ys) {
        // Get base points and offsets.
        int32_t xsb = FastFloor(xs), ysb = FastFloor(ys);
        float xi = static_cast<float>(xs - xsb), yi = static_cast<float>(ys - ysb);
        // Prime pre-multiplication for hash.
        int64_t xsbp = xsb * PRIME_X, ysbp = ysb * PRIME_Y;
        // Unskew.
        float t = (xi + yi) * static_cast<float>(UNSKEW_2D);
        float dx0 = xi + t, dy0 = yi + t;
        // First vertex.
        float value = 0;
        float a0 = RSQUARED_2D - dx0 * dx0 - dy0 * dy0;
        if (a0 > 0) {
            value = (a0 * a0) * (a0 * a0) * Gradient(seed, xsbp, ysbp, dx0, dy0);
        }
        // Second vertex.
        float a1 = static_cast<float>(2 * (1 + 2 * UNSKEW_2D) * (1 / UNSKEW_2D + 2)) * t + (static_cast<float>(-2 * (1 + 2 * UNSKEW_2D) * (1 + 2 * UNSKEW_2D)) + a0);
        if (a1 > 0) {
            float dx1 = dx0 - static_cast<float>(1 + 2 * UNSKEW_2D);
            float dy1 = dy0 - static_cast<float>(1 + 2 * UNSKEW_2D);
            value += (a1 * a1) * (a1 * a1) * Gradient(seed, xsbp + PRIME_X, ysbp + PRIME_Y, dx1, dy1);
        }
        // Third vertex.
        if (dy0 > dx0) {
            float dx2 = dx0 - static_cast<float>(UNSKEW_2D);
            float dy2 = dy0 - static_cast<float>(UNSKEW_2D + 1);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0) {
                value += (a2 * a2) * (a2 * a2) * Gradient(seed, xsbp, ysbp + PRIME_Y, dx2, dy2);
            }
        }
        else {
            float dx2 = dx0 - static_cast<float>(UNSKEW_2D + 1);
            float dy2 = dy0 - static_cast<float>(UNSKEW_2D);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;
            if (a2 > 0) {
                value += (a2 * a2) * (a2 * a2) * Gradient(seed, xsbp + PRIME_X, ysbp, dx2, dy2);
            }
        }
        return value;
    }

}

using namespace Details;

namespace cdtools {

    float Noise::SimplexNoise2D(int64_t seed, double x, double y) {
        // Get points for A2* lattice
        double s = SKEW_2D * (x + y);
        double xs = x + s, ys = y + s;
        // Rescale to [0.0, 1.0]
        return Noise2D_UnskewedBase(seed, xs, ys) / 2.0 + 0.5;
    }

}	// namespace cdtools