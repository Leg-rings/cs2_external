class Matrix
{
public:
	float Matrix[4][4];
	auto operator[](int i) { return Matrix[i]; }
};

class vector
{
public:
	float x, y, z;
	vector(float x = 0.f, float y = 0.f, float z = 0.f) : x(x), y(y), z(z) {}
	vector Transform(Matrix matrix)
	{
		return vector
		{
			x * matrix[0][0] + y * matrix[0][1] + z * matrix[0][2] + matrix[0][3],
			x * matrix[1][0] + y * matrix[1][1] + z * matrix[1][2] + matrix[1][3],
			x * matrix[2][0] + y * matrix[2][1] + z * matrix[2][2] + matrix[2][3]
		};
	}
	vector WorldToScreen(Matrix matrix, float screenx, float screeny)
	{
		float Z = matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3];
		return vector
		{
			(1.f + (matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3]) / Z) * screenx * 0.5f,
			(1.f - (matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3]) / Z) * screeny * 0.5f,
			Z
		};
	}
};

class Quaternion
{
public:
	float x, y, z, w;
};

class alignas(16) CTransform
{
public:
	alignas(16) vector pos;
	alignas(16) Quaternion rot;
	Matrix ToMatrix()
	{
		Matrix matrix{};
		matrix[0][0] = 1.0f - 2.0f * rot.y * rot.y - 2.0f * rot.z * rot.z;
		matrix[1][0] = 2.0f * rot.x * rot.y + 2.0f * rot.w * rot.z;
		matrix[2][0] = 2.0f * rot.x * rot.z - 2.0f * rot.w * rot.y;

		matrix[0][1] = 2.0f * rot.x * rot.y - 2.0f * rot.w * rot.z;
		matrix[1][1] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.z * rot.z;
		matrix[2][1] = 2.0f * rot.y * rot.z + 2.0f * rot.w * rot.x;

		matrix[0][2] = 2.0f * rot.x * rot.z + 2.0f * rot.w * rot.y;
		matrix[1][2] = 2.0f * rot.y * rot.z - 2.0f * rot.w * rot.x;
		matrix[2][2] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.y * rot.y;

		matrix[0][3] = pos.x;
		matrix[1][3] = pos.y;
		matrix[2][3] = pos.z;

		return matrix;
	}
};