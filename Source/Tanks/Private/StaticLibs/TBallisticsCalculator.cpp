// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticLibs/TBallisticsCalculator.h"

double UTBallisticsCalculator::GetCubicRoot(double Value)
{

	if (Value > 0.0)
	{
		//return System.Math.Pow(Value, 1.0 / 3.0);
		return FMath::Pow(Value, 1.0 / 3.0);
	}
	else if (Value < 0)
	{
		//return -System.Math.Pow(-Value, 1.0 / 3.0);
		return -FMath::Pow(-Value, 1.0 / 3.0);
	}
	else
	{
		return 0.0;
	}
}


int32 UTBallisticsCalculator::SolveQuadric(double C0, double C1, double C2, double& S0, double& S1)
{
	/*s0 = double.NaN;
	s1 = double.NaN;*/

	// Avoid Divide by 0
	if (C0 == 0.0)
	{
		return 0;
	}

	double p, q, D;

	/* normal form: x^2 + px + q = 0 */
	p = C1 / (2 * C0);
	q = C2 / C0;

	D = p * p - q;

	if (FMath::IsNearlyZero(D))
	{
		S0 = -p;
		return 1;
	}
	else if (D < 0)
	{
		return 0;
	}
	else /* if (D > 0) */
	{
		double sqrt_D = FMath::Sqrt(D);

		S0 = sqrt_D - p;
		S1 = -sqrt_D - p;
		return 2;
	}
}

int UTBallisticsCalculator::SolveCubic(double C0, double C1, double C2, double C3, double& S0, double& S1, double& S2)
{
	if (C0 == 0.0)
	{
		return 0;
	}

	int32     num;
	double  sub;
	double  A, B, C;
	double  sq_A, p, q;
	double  cb_p, D;

	/* normal form: x^3 + Ax^2 + Bx + C = 0 */
	A = C1 / C0;
	B = C2 / C0;
	C = C3 / C0;

	/*  substitute x = y - A/3 to eliminate quadric term:  x^3 +px + q = 0 */
	sq_A = A * A;
	p = 1.0 / 3 * (-1.0 / 3 * sq_A + B);
	q = 1.0 / 2 * (2.0 / 27 * A * sq_A - 1.0 / 3 * A * B + C);

	/* use Cardano's formula */
	cb_p = p * p * p;
	D = q * q + cb_p;

	if (FMath::IsNearlyZero(D))
	{
		if (FMath::IsNearlyZero(q)) /* one triple solution */
		{
			S0 = 0;
			num = 1;
		}
		else /* one single and one double solution */
		{
			double u = GetCubicRoot(-q);
			S0 = 2 * u;
			S1 = -u;
			num = 2;
		}
	}
	else if (D < 0) /* Casus irreducibilis: three real solutions */
	{
		double phi = 1.0 / 3 * FMath::Acos(-q / FMath::Sqrt(-cb_p));
		double t = 2 * FMath::Sqrt(-p);


		S0 = t * FMath::Cos(phi);
		S1 = -t * FMath::Cos(phi + PI / 3);
		S2 = -t * FMath::Cos(phi - PI / 3);
		num = 3;
	}
	else /* one real solution */
	{
		double sqrt_D = FMath::Sqrt(D);
		double u = GetCubicRoot(sqrt_D - q);
		double v = -GetCubicRoot(sqrt_D + q);

		S0 = u + v;
		num = 1;
	}

	/* resubstitute */
	sub = 1.0 / 3 * A;

	if (num > 0)    S0 -= sub;
	if (num > 1)    S1 -= sub;
	if (num > 2)    S2 -= sub;

	return num;
}

int UTBallisticsCalculator::SolveQuartic(double C0, double C1, double C2, double C3, double C4, double& S0, double& S1, double& S2, double& S3)
{
	if (C0 == 0.0)
	{
		return 0;
	}

	TArray<double> coeffs;
	coeffs.Add(0.0);
	coeffs.Add(0.0);
	coeffs.Add(0.0);
	coeffs.Add(0.0);

	//double[] coeffs = new double[4];
	double  z, u, v, sub;
	double  A, B, C, D;
	double  sq_A, p, q, r;
	int32     num;

	/* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */
	A = C1 / C0;
	B = C2 / C0;
	C = C3 / C0;
	D = C4 / C0;

	/*  substitute x = y - A/4 to eliminate cubic term: x^4 + px^2 + qx + r = 0 */
	sq_A = A * A;
	p = -3.0 / 8 * sq_A + B;
	q = 1.0 / 8 * sq_A * A - 1.0 / 2 * A * B + C;
	r = -3.0 / 256 * sq_A * sq_A + 1.0 / 16 * sq_A * B - 1.0 / 4 * A * C + D;

	if (FMath::IsNearlyZero(r))
	{
		/* no absolute term: y(y^3 + py + q) = 0 */
		coeffs[0] = 1;
		coeffs[1] = 0;
		coeffs[2] = p;
		coeffs[3] = q;

		num = SolveCubic(coeffs[0], coeffs[1], coeffs[2], coeffs[3], S0, S1, S2);
	}
	else
	{
		/* solve the resolvent cubic ... */
		coeffs[3] = 1.0 / 2 * r * p - 1.0 / 8 * q * q;
		coeffs[2] = -r;
		coeffs[1] = -1.0 / 2 * p;
		coeffs[0] = 1;

		SolveCubic(coeffs[0], coeffs[1], coeffs[2], coeffs[3], S0, S1, S2);

		/* ... and take the one real solution ... */
		z = S0;

		/* ... to build two quadric equations */
		u = z * z - r;
		v = 2 * z - p;

		if (FMath::IsNearlyZero(u))
		{
			u = 0;
		}
		else if (u > 0)
		{
			u = FMath::Sqrt(u);
		}
		else
		{
			return 0;
		}


		if (FMath::IsNearlyZero(v))
		{
			v = 0;
		}
		else if (v > 0)
		{
			v = FMath::Sqrt(v);
		}
		else
		{
			return 0;
		}

		coeffs[2] = z - u;
		coeffs[1] = q < 0 ? -v : v;
		coeffs[0] = 1;

		num = SolveQuadric(coeffs[0], coeffs[1], coeffs[2], S0, S1);

		coeffs[2] = z + u;
		coeffs[1] = q < 0 ? v : -v;
		coeffs[0] = 1;

		if (num == 0) num += SolveQuadric(coeffs[0], coeffs[1], coeffs[2], S0, S1);
		else if (num == 1) num += SolveQuadric(coeffs[0], coeffs[1], coeffs[2], S1, S2);
		else if (num == 2) num += SolveQuadric(coeffs[0], coeffs[1], coeffs[2], S2, S3);
	}

	/* resubstitute */
	sub = 1.0 / 4 * A;

	if (num > 0)    S0 -= sub;
	if (num > 1)    S1 -= sub;
	if (num > 2)    S2 -= sub;
	if (num > 3)    S3 -= sub;

	return num;
}


float UTBallisticsCalculator::BallisticRange(float Speed, float Gravity, float InitialHeight)
{
	bool ValidParams = (Speed > 0) && (Gravity > 0) && (InitialHeight > 0);
	if (!ValidParams)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Error, TEXT("BallisticRange() called with invalid parameters!"));
#endif
		return 0;
	}


	// Derivation
	//   (1) x = Speed * time * cos O
	//   (2) y = InitialHeight + (Speed * time * sin O) - (.5 * Gravity*time*time)
	//   (3) via quadratic: t = (Speed*sin O)/Gravity + sqrt(Speed*Speed*sin O + 2*gravity*InitialHeight)/Gravity    [ignore smaller root]
	//   (4) solution: range = x = (Speed*cos O)/Gravity * sqrt(Speed*speed*sin O + 2*Gravity*InitialHeight)    [plug t back into x=speed*time*cos O]

	//float angle = 45 * Mathf.Deg2Rad; // no air resistence, so 45 degrees provides maximum range
	float angle = FMath::DegreesToRadians(45);
	float Cos = FMath::Cos(angle);
	float Sin = FMath::Sin(angle);

	float range = (Speed * Cos / Gravity) * (Speed * Sin + FMath::Sqrt(Speed * Speed * Sin * Sin + 2 * Gravity * InitialHeight));
	return range;
}


int32 UTBallisticsCalculator::SolveBallisticArc(FVector ProjectilePos, float ProjectileSpeed, FVector TargetPos, float Gravity, FBallisticArcSolution& Solution)
{
	bool ValidParams = (ProjectilePos != TargetPos) && (ProjectileSpeed > 0);
	if (!ValidParams)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Error, TEXT("SolveBallisticArc() called with invalid parameters!"));
#endif
		return 0;
	}


	// Derivation
	//   (1) x = v*t*cos O
	//   (2) y = v*t*sin O - .5*g*t^2
	// 
	//   (3) t = x/(cos O*v)                                        [solve t from (1)]
	//   (4) y = v*x*sin O/(cos O * v) - .5*g*x^2/(cos^2 O*v^2)     [plug t into y=...]
	//   (5) y = x*tan O - g*x^2/(2*v^2*cos^2 O)                    [reduce; cos/sin = tan]
	//   (6) y = x*tan O - (g*x^2/(2*v^2))*(1+tan^2 O)              [reduce; 1+tan O = 1/cos^2 O]
	//   (7) 0 = ((-g*x^2)/(2*v^2))*tan^2 O + x*tan O - (g*x^2)/(2*v^2) - y    [re-arrange]
	//   Quadratic! a*p^2 + b*p + c where p = tan O
	//
	//   (8) let gxv = -g*x*x/(2*v*v)
	//   (9) p = (-x +- sqrt(x*x - 4gxv*(gxv - y)))/2*gxv           [quadratic formula]
	//   (10) p = (v^2 +- sqrt(v^4 - g(g*x^2 + 2*y*v^2)))/gx        [multiply top/bottom by -2*v*v/x; move 4*v^4/x^2 into root]
	//   (11) O = atan(p)

	FVector diff = TargetPos - ProjectilePos;
	FVector diffXY = FVector(diff.X, diff.Y, 0.f);					//new Vector3(diff.x, 0f, diff.z);
	float groundDist = diffXY.Size();

	float speed2 = ProjectileSpeed * ProjectileSpeed;
	float speed4 = ProjectileSpeed * ProjectileSpeed * ProjectileSpeed * ProjectileSpeed;
	float z = diff.Z;
	float x = groundDist;
	float gx = Gravity * x;

	float root = speed4 - Gravity * (Gravity * x * x + 2 * z * speed2);

	// No solution
	if (root < 0)
	{
		return 0;
	}

	root = FMath::Sqrt(root);

	float lowAng =  FMath::Atan2(speed2 - root, gx);
	float highAng = FMath::Atan2(speed2 + root, gx);
	int32 numSolutions = lowAng != highAng ? 2 : 1;

	FVector groundDir = diffXY.GetSafeNormal();
	Solution.LowAngleFireVelocity = groundDir * FMath::Cos(lowAng) * ProjectileSpeed + FVector::UpVector * FMath::Sin(lowAng) * ProjectileSpeed;
	if (numSolutions > 1)
	{
		Solution.HighAngleFireVelocity = groundDir * FMath::Cos(highAng) * ProjectileSpeed + FVector::UpVector * FMath::Sin(highAng) * ProjectileSpeed;
	}

	return numSolutions;
}


int32 UTBallisticsCalculator::SolveBallisticArcMovingTarget(FVector ProjectilePos, float ProjectileSpeed, FVector TargetPos, FVector TargetVelocity, float Gravity, FBallisticArcSolution& Solution)
{
	bool ValidParams = (ProjectilePos != TargetPos) && (ProjectileSpeed > 0);
	if (!ValidParams)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Error, TEXT("SolveBallisticArcMovingTarget() called with invalid parameters!"));
#endif
		return 0;
	}

	// Derivation 
	//
	//  For full derivation see: blog.forrestthewoods.com
	//  Here is an abbreviated version.
	//
	//  Four equations, four unknowns (solution.x, solution.y, solution.z, time):
	//
	//  (1) proj_pos.x + solution.x*time = target_pos.x + target_vel.x*time
	//  (2) proj_pos.z + solution.z*time + .5*G*t = target_pos.z + target_vel.z*time
	//  (3) proj_pos.y + solution.y*time = target_pos.y + target_vel.y*time
	//  (4) proj_speed^2 = solution.x^2 + solution.z^2 + solution.y^2
	//
	//  (5) Solve for solution.x and solution.y in equations (1) and (3)
	//  (6) Square solution.x and solution.y from (5)
	//  (7) Solve solution.z^2 by plugging (6) into (4)
	//  (8) Solve solution.z by rearranging (2)
	//  (9) Square (8)
	//  (10) Set (8) = (7). All solution.xyz terms should be gone. Only time remains.
	//  (11) Rearrange 10. It will be of the form a*^4 + b*t^3 + c*t^2 + d*t * e. This is a quartic.
	//  (12) Solve the quartic using SolveQuartic.
	//  (13) If there are no positive, real roots there is no solution.
	//  (14) Each positive, real root is one valid solution
	//  (15) Plug each time value into (1) (2) and (3) to calculate solution.xyz
	//  (16) The end.

	double G = Gravity;

	double A = ProjectilePos.X;
	double B = ProjectilePos.Z;
	double C = ProjectilePos.Y;
	double M = TargetPos.X;
	double N = TargetPos.Z;
	double O = TargetPos.Y;
	double P = TargetVelocity.X;
	double Q = TargetVelocity.Z;
	double R = TargetVelocity.Y;
	double S = ProjectileSpeed;

	double H = M - A;
	double J = O - C;
	double K = N - B;
	double L = -.5f * G;

	// Quartic Coeffecients
	double c0 = L * L;
	double c1 = -2 * Q * L;
	double c2 = Q * Q - 2 * K * L - S * S + P * P + R * R;
	double c3 = 2 * K * Q + 2 * H * P + 2 * J * R;
	double c4 = K * K + H * H + J * J;

	// Solve quartic
	TArray<double> Times;
	Times.Add(0.0);
	Times.Add(0.0);
	Times.Add(0.0);
	Times.Add(0.0);
	int32 numTimes = SolveQuartic(c0, c1, c2, c3, c4, Times[0], Times[1], Times[2], Times[3]);


	// Sort so faster collision is found first
	//System.Array.Sort(times);
	Times.Sort([](const double& A, const double& B) { return A < B; });

	// Plug quartic solutions into base equations
	// There should never be more than 2 positive, real roots.
	TArray<FVector> Solutions;
	Solutions.Add(FVector::ZeroVector);
	Solutions.Add(FVector::ZeroVector);

	int32 numSolutions = 0;

	for (int32 i = 0; i < Times.Num() && numSolutions < 2; ++i)
	{
		double t = Times[i];
		if (t <= 0 || FMath::IsNaN(t))
			continue;

		Solutions[numSolutions].X = (float)((H + P * t) / t);
		Solutions[numSolutions].Z = (float)((K + Q * t - L * t * t) / t);
		Solutions[numSolutions].Y = (float)((J + R * t) / t);
		++numSolutions;
	}

	// Write out solutions
	if (numSolutions > 0)   Solution.LowAngleFireVelocity = Solutions[0];
	if (numSolutions > 1)   Solution.HighAngleFireVelocity = Solutions[1];

	return numSolutions;
}


bool UTBallisticsCalculator::SolveBallisticArcLateral(FVector ProjectilePos, float LateralSpeed, FVector TargetPos, float MaxHeightOffset, FBallisticArcLateralSolution& Solution)
{
	bool ValidParams = (ProjectilePos != TargetPos) && (LateralSpeed > 0);
	if (!ValidParams)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Error, TEXT("SolveBallisticArcLateral() called with invalid parameters!"));
#endif
		return false;
	}


	FVector diff = TargetPos - ProjectilePos;
	FVector diffXY = FVector(diff.X, diff.Y, 0.f);
	float lateralDist = diffXY.Size();

	if (lateralDist == 0)
		return false;

	float time = lateralDist / LateralSpeed;

	Solution.FireVelocity = diffXY.GetSafeNormal() * LateralSpeed;

	// System of equations. Hit max_height at t=.5*time. Hit target at t=time.
	//
	// peak = z0 + vertical_speed*halfTime + .5*gravity*halfTime^2
	// end = z0 + vertical_speed*time + .5*gravity*time^s
	// Wolfram Alpha: solve b = a + .5*v*t + .5*g*(.5*t)^2, c = a + vt + .5*g*t^2 for g, v
	float a = ProjectilePos.Z;       // initial
	float b = MaxHeightOffset;       // peak
	float c = TargetPos.Z;     // final

	Solution.Gravity = -4 * (a - 2 * b + c) / (time * time);
	Solution.FireVelocity.Z = -(3 * a - 4 * b + c) / time;

	return true;
}


bool UTBallisticsCalculator::SolveBallisticArcLateralMovingTarget(FVector ProjectilePos, float LateralSpeed, FVector TargetPos, FVector TargetVelocity, float MaxHeightOffset, FBallisticArcLateralMovingTargetSolution& Solution)
{

	bool ValidParams = (ProjectilePos != TargetPos) && (LateralSpeed > 0);
	if (!ValidParams)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Error, TEXT("SolveBallisticArcLateralMovingTarget() called with invalid parameters!"));
#endif
		return false;
	}


	// Ground plane terms
	FVector	TargetVelXY = FVector(TargetVelocity.X, TargetVelocity.Y, 0.f);
	FVector diffXY = TargetPos - ProjectilePos;
	diffXY.Z = 0;


	// Derivation
	//   (1) Base formula: |P + V*t| = S*t
	//   (2) Substitute variables: |diffXY + targetVelXY*t| = S*t
	//   (3) Square both sides: Dot(diffXY,diffXY) + 2*Dot(diffXY, targetVelXY)*t + Dot(targetVelXY, targetVelXY)*t^2 = S^2 * t^2
	//   (4) Quadratic: (Dot(targetVelXY,targetVelXY) - S^2)t^2 + (2*Dot(diffXY, targetVelXY))*t + Dot(diffXY, diffXY) = 0
	float c0 = FVector::DotProduct(TargetVelXY, TargetVelXY) - LateralSpeed * LateralSpeed;
	float c1 = 2.f * FVector::DotProduct(diffXY, TargetVelXY);
	float c2 = FVector::DotProduct(diffXY, diffXY);
	double t0, t1;
	int32 n = SolveQuadric(c0, c1, c2, t0, t1);

	// pick smallest, positive time
	bool valid0 = n > 0 && t0 > 0;
	bool valid1 = n > 1 && t1 > 0;

	float t;
	if (!valid0 && !valid1)
		return false;
	else if (valid0 && valid1)
		t = FMath::Min((float)t0, (float)t1);
	else
		t = valid0 ? (float)t0 : (float)t1;

	// Calculate impact point
	Solution.ImpactPoint = TargetPos + (TargetVelocity * t);

	// Calculate fire velocity along XY plane
	FVector dir = Solution.ImpactPoint - ProjectilePos;
	Solution.FireVelocity = FVector(dir.X, dir.Y, 0.f).GetSafeNormal() * LateralSpeed;

	// Solve system of equations. Hit max_height at t=.5*time. Hit target at t=time.
	//
	// peak = z0 + vertical_speed*halfTime + .5*gravity*halfTime^2
	// end = z0 + vertical_speed*time + .5*gravity*time^s
	// Wolfram Alpha: solve b = a + .5*v*t + .5*g*(.5*t)^2, c = a + vt + .5*g*t^2 for g, v
	float a = ProjectilePos.Z;       // initial
	float b = FMath::Max(ProjectilePos.Z, Solution.ImpactPoint.Z) + MaxHeightOffset;  // peak
	float c = Solution.ImpactPoint.Z;   // final

	Solution.Gravity = -4 * (a - 2 * b + c) / (t * t);
	Solution.FireVelocity.Z = -(3 * a - 4 * b + c) / t;

	return true;
}


