#pragma once


static constexpr float PI = 3.1415926f;

static constexpr float DEFAULT_EPSILON = 0.00001f;

[[nodiscard]]
bool isEqualFloat(float a, float b, float epsilon = DEFAULT_EPSILON);
